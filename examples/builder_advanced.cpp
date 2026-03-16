/**
 * @file 07_builder_advanced.cpp
 * @brief Advanced QRCodeBuilder usage: version pinning, error handling,
 *        mode forcing, and introspection.
 */

#include <iostream>
#include <stdexcept>
#include "qr/qr.hpp"

static void section(const char *title)
{
  std::cout << "\n══ " << title << " ══\n";
}

int main()
{
  // 1. Builder with all options explicit
  section("1. Full builder — EC=Q, Byte mode, auto version");
  {
    auto qr = qr::QRCodeBuilder("Hello from the builder!")
                  .error_correction(qr::ECLevel::Q)
                  .mode(qr::Mode::Byte)
                  .build();
    std::cout << qr.info() << "\n";
    qr.print();
  }

  // 2. Version pinning
  section("2. Version pinning — force v5 for short data");
  {
    auto qr = qr::QRCodeBuilder("Hi")
                  .version(5)
                  .error_correction(qr::ECLevel::L)
                  .build();
    std::cout << "Forced version: " << qr.version()
              << "  size: " << qr.size() << "x" << qr.size() << "\n";
    qr.save_svg("qr_v5_forced.svg");
  }

  section("3. Auto version vs pinned");
  {
    const std::string DATA = "COMPARE";
    auto qr_auto = qr::QRCodeBuilder(DATA)
                       .error_correction(qr::ECLevel::M)
                       .build();
    auto qr_pin = qr::QRCodeBuilder(DATA)
                      .version(10)
                      .error_correction(qr::ECLevel::M)
                      .build();
    std::cout << "Auto   : v" << qr_auto.version()
              << "  " << qr_auto.size() << "x" << qr_auto.size() << "\n";
    std::cout << "Pinned : v" << qr_pin.version()
              << "  " << qr_pin.size() << "x" << qr_pin.size() << "\n";
  }

  section("4. Error handling");
  {
    // Empty data
    try
    {
      auto qr = qr::QRCodeBuilder("").build();
      std::cout << "[FAIL] should have thrown\n";
    }
    catch (const std::invalid_argument &e)
    {
      std::cout << "[OK] Empty data → std::invalid_argument: " << e.what() << "\n";
    }

    // Version out of range
    try
    {
      auto qr = qr::QRCodeBuilder("Hello").version(99).build();
      std::cout << "[FAIL] should have thrown\n";
    }
    catch (const std::out_of_range &e)
    {
      std::cout << "[OK] Version 99 → std::out_of_range: " << e.what() << "\n";
    }

    // Version 0
    try
    {
      auto qr = qr::QRCodeBuilder("Hello").version(0).build();
      std::cout << "[FAIL] should have thrown\n";
    }
    catch (const std::out_of_range &e)
    {
      std::cout << "[OK] Version 0  → std::out_of_range: " << e.what() << "\n";
    }

    // Bad SVG path
    try
    {
      auto qr = qr::generate("test");
      qr.save_svg("/nonexistent/path/qr.svg");
      std::cout << "[FAIL] should have thrown\n";
    }
    catch (const std::runtime_error &e)
    {
      std::cout << "[OK] Bad path   → std::runtime_error: " << e.what() << "\n";
    }
  }

  section("5. Mode forcing vs auto-detection");
  {
    const std::string all_digits = "31415926535";

    auto auto_qr = qr::QRCodeBuilder(all_digits).build();
    auto byte_qr = qr::QRCodeBuilder(all_digits).mode(qr::Mode::Byte).build();
    auto alpha_qr = qr::QRCodeBuilder(all_digits).mode(qr::Mode::Alphanumeric).build();

    auto mname = [](qr::Mode m) -> const char *
    {
      switch (m)
      {
      case qr::Mode::Numeric:
        return "Numeric";
      case qr::Mode::Alphanumeric:
        return "Alphanumeric";
      case qr::Mode::Byte:
        return "Byte";
      }
      return "?";
    };

    std::cout << "Data: \"" << all_digits << "\"\n";
    std::cout << "  Auto        → mode=" << mname(auto_qr.mode())
              << "  v" << auto_qr.version() << "\n";
    std::cout << "  Force Byte  → mode=" << mname(byte_qr.mode())
              << "  v" << byte_qr.version() << "  (larger)\n";
    std::cout << "  Force Alpha → mode=" << mname(alpha_qr.mode())
              << "  v" << alpha_qr.version() << "\n";
  }

  section("6. Introspection");
  {
    auto qr = qr::generate("Introspection demo 12345", qr::ECLevel::Q);
    std::cout << "info()    : " << qr.info() << "\n";
    std::cout << "version() : " << qr.version() << "\n";
    std::cout << "size()    : " << qr.size() << "\n";
    std::cout << "ec_level(): " << static_cast<int>(qr.ec_level()) << "  (0=L 1=M 2=Q 3=H)\n";
    std::cout << "mode()    : " << static_cast<int>(qr.mode()) << "  (0=Num 1=Alpha 2=Byte)\n";

    // Count function vs data modules
    int func = 0, data = 0;
    const auto &mat = qr.matrix();
    for (int r = 0; r < qr.size(); ++r)
      for (int c = 0; c < qr.size(); ++c)
        (mat.is_function(r, c) ? func : data)++;
    std::cout << "function modules: " << func << "\n";
    std::cout << "data modules    : " << data << "\n";
    std::cout << "total modules   : " << qr.size() * qr.size() << "\n";
  }

  return 0;
}
