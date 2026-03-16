# qr — Examples

Each example is self-contained and demonstrates a specific feature of the library.

## Build all examples

```bash
cd examples
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Or build a single file manually:

```bash
g++ -std=c++20 -O2 -I../include -o 01_hello_world 01_hello_world.cpp
```

---

## Examples

| File | Description |
|------|-------------|
| `01_hello_world.cpp`    | Simplest usage — generate and print ASCII to stdout |
| `02_ec_levels.cpp`      | Compare all 4 ECC levels (L / M / Q / H) |
| `03_encoding_modes.cpp` | Numeric, Alphanumeric, Byte — auto-detect vs forced |
| `04_svg_output.cpp`     | SVG generation with custom colors, sizes, themes |
| `05_ascii_rendering.cpp`| ASCII styles: block chars, hash, dot matrix, ANSI |
| `06_ppm_export.cpp`     | Export PPM images (zero dependencies) |
| `07_builder_api.cpp`    | `QRCodeBuilder` fluent API, version/mode overrides |
| `08_custom_renderer.cpp`| `for_each_module()` — HTML table, CSV dump, ANSI colors |
| `09_batch_generation.cpp`| Batch: products, vCards, Wi-Fi, SMS, Geo, tickets |
| `10_svg_embed.cpp`      | Inline SVG in HTML — produces a full QR gallery page |

---

## Quick reference

```cpp
// Simplest
auto qr = qr::generate("Hello!");
std::cout << qr.to_ascii();
qr.save_svg("hello.svg");

// Custom EC level
auto qr = qr::generate("https://example.com", qr::ECLevel::H);

// Builder API
auto qr = qr::QRCodeBuilder("data")
              .error_correction(qr::ECLevel::Q)
              .version(4)           // optional: force version
              .mode(qr::Mode::Byte) // optional: force mode
              .build();

// Custom SVG
qr::SVGOptions opts;
opts.module_size = 16;
opts.dark_color  = "#1a1a2e";
opts.bg_color    = "#f5f0e8";
qr.save_svg("custom.svg", opts);

// Custom ASCII
qr::ASCIIOptions opts;
opts.dark_char  = "##";
opts.light_char = "  ";
std::cout << qr.to_ascii(opts);

// PPM image (no deps)
qr.save_ppm("qr.ppm", 10, 4);

// Iterate all modules
qr.for_each_module([](int row, int col, bool dark) {
    // custom rendering logic
});

// Special data formats
auto wifi    = "WIFI:T:WPA;S:MyNet;P:pass;;";
auto vcard   = "BEGIN:VCARD\r\nVERSION:3.0\r\nFN:Alice\r\nEND:VCARD\r\n";
auto geo     = "geo:48.8566,2.3522";
auto sms     = "sms:+33612345678?body=Hi!";
auto mailto  = "mailto:hello@example.com";
```

---

## ECC Level guide

| Level | Recovery | Use case |
|-------|----------|----------|
| L     | 7%       | Clean environments, max data density |
| M     | 15%      | **Default** — good balance |
| Q     | 25%      | Logo overlay, partial obstruction expected |
| H     | 30%      | Industrial, outdoor, damaged/dirty surfaces |
