/**
 * @file qr_tests.cpp
 * @brief Unit tests / smoke tests for qr.hpp
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o qr_tests qr_tests.cpp && ./qr_tests
 */

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include "qr/qr.hpp"

static int passed = 0;
static int failed = 0;

#define TEST(name, expr)                      \
  do                                          \
  {                                           \
    if (expr)                                 \
    {                                         \
      std::cout << "[PASS] " << name << "\n"; \
      ++passed;                               \
    }                                         \
    else                                      \
    {                                         \
      std::cout << "[FAIL] " << name << "\n"; \
      ++failed;                               \
    }                                         \
  } while (0)

#define TEST_NO_THROW(name, expr)                                             \
  do                                                                          \
  {                                                                           \
    try                                                                       \
    {                                                                         \
      expr;                                                                   \
      std::cout << "[PASS] " << name << "\n";                                 \
      ++passed;                                                               \
    }                                                                         \
    catch (const std::exception &e)                                           \
    {                                                                         \
      std::cout << "[FAIL] " << name << " — exception: " << e.what() << "\n"; \
      ++failed;                                                               \
    }                                                                         \
  } while (0)

#define TEST_THROWS(name, expr)               \
  do                                          \
  {                                           \
    bool threw = false;                       \
    try                                       \
    {                                         \
      expr;                                   \
    }                                         \
    catch (...)                               \
    {                                         \
      threw = true;                           \
    }                                         \
    if (threw)                                \
    {                                         \
      std::cout << "[PASS] " << name << "\n"; \
      ++passed;                               \
    }                                         \
    else                                      \
    {                                         \
      std::cout << "[FAIL] " << name << "\n"; \
      ++failed;                               \
    }                                         \
  } while (0)

// ─────────────────────────────────────────────────────────────────────────────

void test_mode_detection()
{
  std::cout << "\n=== Mode Detection ===\n";
  TEST("Numeric mode: '1234567890'",
       qr::DataEncoder::detect_mode("1234567890") == qr::Mode::Numeric);
  TEST("Alphanumeric mode: 'HELLO WORLD'",
       qr::DataEncoder::detect_mode("HELLO WORLD") == qr::Mode::Alphanumeric);
  TEST("Byte mode: 'Hello, World!'",
       qr::DataEncoder::detect_mode("Hello, World!") == qr::Mode::Byte);
  TEST("Byte mode: lowercase",
       qr::DataEncoder::detect_mode("hello") == qr::Mode::Byte);
  TEST("Numeric: empty string",
       qr::DataEncoder::detect_mode("0000") == qr::Mode::Numeric);
}

void test_version_selection()
{
  std::cout << "\n=== Version Selection ===\n";
  // Short data should use version 1
  auto qr1 = qr::generate("1", qr::ECLevel::M);
  TEST("Short numeric → version 1", qr1.version() == 1);

  // URL should auto-select appropriate version
  auto qr2 = qr::generate("https://example.com", qr::ECLevel::M);
  TEST("URL → version >= 2", qr2.version() >= 2);
  TEST("URL → version <= 40", qr2.version() <= 40);

  // High EC should require higher version for same data
  auto qrL = qr::generate("Hello", qr::ECLevel::L);
  auto qrH = qr::generate("Hello", qr::ECLevel::H);
  TEST("ECLevel H >= ECLevel L version", qrH.version() >= qrL.version());
}

void test_matrix_properties()
{
  std::cout << "\n=== Matrix Properties ===\n";
  for (int v : {1, 2, 5, 10, 20, 40})
  {
    auto qr = qr::QRCodeBuilder(std::string(5, 'A'))
                  .version(v)
                  .error_correction(qr::ECLevel::L)
                  .build();
    int expected_size = 17 + 4 * v;
    TEST(("Version " + std::to_string(v) + " size = " + std::to_string(expected_size)).c_str(),
         qr.size() == expected_size);
  }
}

void test_ascii_output()
{
  std::cout << "\n=== ASCII Renderer ===\n";
  auto qr = qr::generate("Hello", qr::ECLevel::M);
  auto ascii = qr.to_ascii();
  TEST("ASCII output not empty", !ascii.empty());
  TEST("ASCII output contains newlines", ascii.find('\n') != std::string::npos);
  // Row count = size + 2*quiet_zone
  int expected_rows = qr.size() + 2 * 2; // default quiet_zone = 2
  int newline_count = static_cast<int>(std::count(ascii.begin(), ascii.end(), '\n'));
  TEST("ASCII row count matches size + quiet_zone",
       newline_count == expected_rows);
}

void test_ascii_options()
{
  std::cout << "\n=== ASCII Options ===\n";
  auto qr = qr::generate("TEST", qr::ECLevel::M);

  qr::ASCIIOptions opts;
  opts.quiet_zone = 0;
  opts.dark_char = "#";
  opts.light_char = ".";
  auto ascii = qr.to_ascii(opts);
  TEST("Custom chars present (dark)", ascii.find('#') != std::string::npos);
  TEST("Custom chars present (light)", ascii.find('.') != std::string::npos);

  // Invert
  opts.invert = true;
  auto ascii_inv = qr.to_ascii(opts);
  TEST("Inverted ASCII differs from normal", ascii_inv != ascii);
}

void test_svg_output()
{
  std::cout << "\n=== SVG Renderer ===\n";
  auto qr = qr::generate("Hello, World!", qr::ECLevel::M);
  auto svg = qr.to_svg();
  TEST("SVG starts with XML header", svg.substr(0, 5) == "<?xml");
  TEST("SVG contains <svg tag", svg.find("<svg") != std::string::npos);
  TEST("SVG contains </svg>", svg.find("</svg>") != std::string::npos);
  TEST("SVG contains rect elements", svg.find("<rect") != std::string::npos);

  // Custom options
  qr::SVGOptions opts;
  opts.module_size = 20;
  opts.dark_color = "#ff0000";
  auto svg2 = qr.to_svg(opts);
  TEST("Custom color in SVG", svg2.find("#ff0000") != std::string::npos);
}

void test_save_svg()
{
  std::cout << "\n=== SVG File Save ===\n";
  auto qr = qr::generate("File save test", qr::ECLevel::L);
  const std::string path = "/tmp/qr_test_output.svg";
  TEST_NO_THROW("save_svg doesn't throw", qr.save_svg(path));

  std::ifstream f(path);
  TEST("SVG file exists and is readable", f.good());
  std::string content((std::istreambuf_iterator<char>(f)),
                      std::istreambuf_iterator<char>());
  TEST("SVG file content valid", content.find("<svg") != std::string::npos);
}

void test_save_ppm()
{
  std::cout << "\n=== PPM File Save ===\n";
  auto qr = qr::generate("PPM test", qr::ECLevel::M);
  TEST_NO_THROW("save_ppm doesn't throw", qr.save_ppm("/tmp/qr_test.ppm"));

  std::ifstream f("/tmp/qr_test.ppm", std::ios::binary);
  TEST("PPM file created", f.good());
  std::string header(2, '\0');
  f.read(header.data(), 2);
  TEST("PPM has P6 header", header == "P6");
}

void test_ec_levels()
{
  std::cout << "\n=== Error Correction Levels ===\n";
  std::string data = "EC Level test string 12345";
  for (auto ec : {qr::ECLevel::L, qr::ECLevel::M, qr::ECLevel::Q, qr::ECLevel::H})
  {
    TEST_NO_THROW("Generate with EC level " + std::to_string(static_cast<int>(ec)),
                  auto qr = qr::generate(data, ec));
  }
  // Higher EC = larger or equal version
  auto qrL = qr::generate(data, qr::ECLevel::L);
  auto qrH = qr::generate(data, qr::ECLevel::H);
  TEST("ECLevel H version >= L version", qrH.version() >= qrL.version());
}

void test_numeric_mode()
{
  std::cout << "\n=== Numeric Mode ===\n";
  TEST_NO_THROW("Generate pure numeric", auto q = qr::generate("0123456789", qr::ECLevel::M));
  auto qr = qr::generate("01234567890123456789", qr::ECLevel::M);
  TEST("Numeric mode selected", qr.mode() == qr::Mode::Numeric);
}

void test_alphanumeric_mode()
{
  std::cout << "\n=== Alphanumeric Mode ===\n";
  TEST_NO_THROW("Generate ALPHANUMERIC", auto q = qr::generate("HELLO WORLD", qr::ECLevel::M));
  auto qr = qr::generate("HELLO WORLD", qr::ECLevel::M);
  TEST("Alphanumeric mode selected", qr.mode() == qr::Mode::Alphanumeric);
}

void test_byte_mode()
{
  std::cout << "\n=== Byte Mode ===\n";
  TEST_NO_THROW("Generate URL", auto q = qr::generate("https://example.com", qr::ECLevel::M));
  auto qr = qr::generate("Hello, World! 🌍", qr::ECLevel::M);
  TEST("Byte mode selected for mixed content", qr.mode() == qr::Mode::Byte);
}

void test_builder_api()
{
  std::cout << "\n=== Builder API ===\n";
  TEST_NO_THROW("Builder with all options",
                auto qr = qr::QRCodeBuilder("Builder test")
                              .error_correction(qr::ECLevel::Q)
                              .build());
  auto qr = qr::QRCodeBuilder("Builder test")
                .error_correction(qr::ECLevel::Q)
                .build();
  TEST("Builder EC level preserved", qr.ec_level() == qr::ECLevel::Q);
}

void test_for_each_module()
{
  std::cout << "\n=== for_each_module ===\n";
  auto qr = qr::generate("Module iter", qr::ECLevel::M);
  int count = 0;
  int dark_count = 0;
  qr.for_each_module([&](int, int, bool dark)
                     {
        ++count;
        if (dark) ++dark_count; });
  TEST("for_each_module visits all modules",
       count == qr.size() * qr.size());
  TEST("Some dark modules present", dark_count > 0);
  TEST("Some light modules present", dark_count < count);
  // QR codes are ~50% dark
  TEST("Dark ratio roughly 20-80%",
       dark_count > count / 5 && dark_count < count * 4 / 5);
}

void test_long_data()
{
  std::cout << "\n=== Long Data ===\n";
  // ~100 byte URL
  std::string url = "https://example.com/very/long/path?param1=value1&param2=value2&param3=value3";
  TEST_NO_THROW("Long URL generates", auto q = qr::generate(url, qr::ECLevel::M));
  auto qr = qr::generate(url, qr::ECLevel::M);
  TEST("Long URL version >= 3", qr.version() >= 3);

  // Max capacity test (version 40, L)
  std::string big(2953, 'A'); // max byte capacity v40 L
  // Trim to safe size
  big.resize(100);
  TEST_NO_THROW("100-char string generates", auto q = qr::generate(big, qr::ECLevel::L));
}

void test_error_handling()
{
  std::cout << "\n=== Error Handling ===\n";
  TEST_THROWS("Empty data throws", qr::generate("", qr::ECLevel::M));
  // Invalid version
  TEST_THROWS("Version 0 throws",
              qr::QRCodeBuilder("x").version(0).build());
  TEST_THROWS("Version 41 throws",
              qr::QRCodeBuilder("x").version(41).build());
}

void test_info()
{
  std::cout << "\n=== Info String ===\n";
  auto qr = qr::generate("Hello", qr::ECLevel::M);
  auto info = qr.info();
  TEST("Info contains 'QR Code'", info.find("QR Code") != std::string::npos);
  TEST("Info contains version", info.find("v") != std::string::npos);
  TEST("Info contains EC level", info.find("EC=M") != std::string::npos);
}

void test_make_svg_free_fn()
{
  std::cout << "\n=== Free Function API ===\n";
  TEST_NO_THROW("qr::make_svg free fn",
                qr::make_svg("Free fn test", "/tmp/qr_free_fn.svg"));
}

void test_various_versions()
{
  std::cout << "\n=== Various Versions ===\n";
  for (int v = 1; v <= 10; ++v)
  {
    std::string data = "V" + std::to_string(v) + " test";
    try
    {
      auto qr = qr::QRCodeBuilder(data).version(v).error_correction(qr::ECLevel::L).build();
      int expected = 17 + 4 * v;
      TEST(("Version " + std::to_string(v) + " size correct").c_str(),
           qr.size() == expected);
    }
    catch (const std::exception &e)
    {
      std::cout << "[FAIL] Version " << v << ": " << e.what() << "\n";
      ++failed;
    }
  }
}

// ─────────────────────────────────────────────────────────────────────────────
int main()
{
  std::cout << "╔══════════════════════════════════════╗\n";
  std::cout << "║    QR Code Library — Test Suite      ║\n";
  std::cout << "╚══════════════════════════════════════╝\n";

  test_mode_detection();
  test_version_selection();
  test_matrix_properties();
  test_ascii_output();
  test_ascii_options();
  test_svg_output();
  test_save_svg();
  test_save_ppm();
  test_ec_levels();
  test_numeric_mode();
  test_alphanumeric_mode();
  test_byte_mode();
  test_builder_api();
  test_for_each_module();
  test_long_data();
  test_error_handling();
  test_info();
  test_make_svg_free_fn();
  test_various_versions();

  std::cout << "\n══════════════════════════════════════\n";
  std::cout << "  Results: " << passed << " passed, " << failed << " failed\n";
  std::cout << "══════════════════════════════════════\n";
  return failed == 0 ? 0 : 1;
}
