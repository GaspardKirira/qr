# qr
QR Code generator for C++.

`qr` is a complete, header-only QR Code generator written in modern C++20.
It implements the full ISO/IEC 18004 standard from scratch —
no wrappers, no external dependencies.

## Download
https://vixcpp.com/registry/pkg/gk/qr

## Why qr?

Many applications need to embed QR codes directly —
without pulling in a heavy library or calling an external tool.

Common examples include:

- product labeling systems
- ticketing and event management
- contactless payment flows
- asset tracking
- Wi-Fi onboarding
- document and invoice linking
- embedded systems with display output

`qr` solves this by generating valid QR codes entirely in-process,
from raw data to final matrix, with multiple output formats.

Example:

```cpp
auto code = qr::generate("https://example.com");
code.save_svg("qr.svg");
std::cout << code.to_ascii();
```

This produces a fully standard-compliant QR code
readable by any phone camera or scanner.

---

# Features

- Full ISO/IEC 18004 compliant QR code generation
- Numeric, Alphanumeric, Byte encoding modes
- Automatic mode detection
- Versions 1 to 40 with automatic selection
- Error correction levels L, M, Q, H
- Complete Reed-Solomon ECC implementation
- All 8 official mask patterns with penalty scoring
- All 4 penalty rules (runs, blocks, patterns, balance)
- Finder, alignment, timing, format and version patterns
- ASCII renderer for terminal output
- SVG renderer with full color and size customization
- PPM image export with zero dependencies
- Fluent builder API
- Module iteration via callback
- Header-only implementation
- No external dependencies
- Requires C++20

Supported output formats:

```text
ASCII   — terminal block characters or custom chars
SVG     — scalable vector, custom colors and sizes
PPM     — raw pixel image, convertible to PNG via ImageMagick
HTML    — inline SVG embedding via for_each_module()
```

---

# Installation

## Using Vix Registry

```bash
vix add @gk/qr
vix deps
```

## Manual

```bash
git clone https://github.com/Gaspardkirira/qr.git
```

Add the `include/` directory to your project.

## Dependency

- Requires **C++20** or newer.
- No external libraries required.

---

## Quick examples

### Generate and print

```cpp
#include <qr/qr.hpp>

int main()
{
    auto code = qr::generate("Hello, World!");
    std::cout << code.to_ascii() << "\n";
}
```

### Save as SVG

```cpp
#include <qr/qr.hpp>

int main()
{
    auto code = qr::generate("https://example.com");
    code.save_svg("qr.svg");
}
```

### Custom error correction level

```cpp
#include <qr/qr.hpp>

int main()
{
    // H level — recovers up to 30% damage
    auto code = qr::generate("https://example.com", qr::ECLevel::H);
    code.save_svg("qr_robust.svg");
}
```

### Builder API

```cpp
#include <qr/qr.hpp>

int main()
{
    auto code = qr::QRCodeBuilder("https://example.com")
                    .error_correction(qr::ECLevel::Q)
                    .version(4)
                    .build();
    code.save_svg("qr.svg");
}
```

### Custom SVG styling

```cpp
#include <qr/qr.hpp>

int main()
{
    qr::SVGOptions opts;
    opts.module_size = 16;
    opts.dark_color  = "#1a1a2e";
    opts.bg_color    = "#f5f0e8";
    opts.quiet_zone  = 4;

    qr::generate("https://example.com", qr::ECLevel::M)
        .save_svg("qr_branded.svg", opts);
}
```

### Custom ASCII rendering

```cpp
#include <qr/qr.hpp>

int main()
{
    qr::ASCIIOptions opts;
    opts.dark_char  = "##";
    opts.light_char = "  ";
    opts.quiet_zone = 2;

    auto code = qr::generate("Hello");
    std::cout << code.to_ascii(opts) << "\n";
}
```

### PPM image export

```cpp
#include <qr/qr.hpp>

int main()
{
    auto code = qr::generate("https://example.com");
    code.save_ppm("qr.ppm", 10, 4);
    // Convert: convert qr.ppm qr.png
}
```

### Special data formats

```cpp
#include <qr/qr.hpp>

int main()
{
    // Wi-Fi credential
    qr::generate("WIFI:T:WPA;S:MyNetwork;P:password123;;").save_svg("wifi.svg");

    // vCard contact
    std::string vcard =
        "BEGIN:VCARD\r\nVERSION:3.0\r\n"
        "FN:Alice Martin\r\nTEL:+33612345678\r\n"
        "EMAIL:alice@example.com\r\nEND:VCARD\r\n";
    qr::generate(vcard, qr::ECLevel::M).save_svg("contact.svg");

    // Geo location
    qr::generate("geo:48.8566,2.3522").save_svg("location.svg");

    // SMS
    qr::generate("sms:+33612345678?body=Hello!").save_svg("sms.svg");
}
```

### Module iteration

```cpp
#include <qr/qr.hpp>

int main()
{
    auto code = qr::generate("Hello");
    code.for_each_module([](int row, int col, bool dark) {
        // custom rendering, export, or processing
    });
}
```

### Batch generation

```cpp
#include <qr/qr.hpp>
#include <vector>
#include <string>

int main()
{
    std::vector<std::string> urls = {
        "https://example.com/product/1",
        "https://example.com/product/2",
        "https://example.com/product/3",
    };

    for (std::size_t i = 0; i < urls.size(); ++i) {
        qr::generate(urls[i], qr::ECLevel::M)
            .save_svg("product_" + std::to_string(i + 1) + ".svg");
    }
}
```

---

## API overview

### Free functions

```cpp
qr::generate(data)                       // → QRCode, EC=M, auto version
qr::generate(data, ec)                   // → QRCode, custom EC level
qr::make_svg(data, path)                 // generate + save SVG
qr::make_svg(data, path, ec, opts)       // generate + save SVG with options
qr::print(data)                          // generate + print ASCII to stdout
```

### QRCodeBuilder

```cpp
QRCodeBuilder(data)
    .error_correction(ECLevel)           // L, M, Q, H
    .version(int)                        // 1–40, optional
    .mode(Mode)                          // Numeric, Alphanumeric, Byte, optional
    .build()                             // → QRCode
```

### QRCode

```cpp
code.to_ascii()                          // → std::string
code.to_ascii(ASCIIOptions)              // → std::string
code.to_svg()                            // → std::string
code.to_svg(SVGOptions)                  // → std::string
code.save_svg(path)
code.save_svg(path, SVGOptions)
code.save_ppm(path, module_size, quiet_zone)
code.print()
code.for_each_module(fn)                 // fn(int row, int col, bool dark)
code.module(row, col)                    // → bool
code.version()                           // → int
code.size()                              // → int  (modules per side)
code.ec_level()                          // → ECLevel
code.mode()                              // → Mode
code.info()                              // → std::string  (human-readable summary)
```

### SVGOptions

```cpp
SVGOptions opts;
opts.module_size  = 10;           // pixels per module
opts.quiet_zone   = 4;            // modules of border
opts.dark_color   = "#000000";
opts.light_color  = "#ffffff";
opts.bg_color     = "#ffffff";
opts.invert       = false;
```

### ASCIIOptions

```cpp
ASCIIOptions opts;
opts.quiet_zone   = 2;
opts.dark_char    = "██";
opts.light_char   = "  ";
opts.invert       = false;
```

### ECLevel

| Value | Recovery | Use case |
|-------|----------|----------|
| `ECLevel::L` | 7%  | Max data density, clean environment |
| `ECLevel::M` | 15% | Default — good balance |
| `ECLevel::Q` | 25% | Logo overlay or partial obstruction |
| `ECLevel::H` | 30% | Industrial, outdoor, damaged surfaces |

### Mode

| Value | Charset | Efficiency |
|-------|---------|------------|
| `Mode::Numeric`      | 0–9 only                         | ~3.3 bits/char |
| `Mode::Alphanumeric` | 0–9 A–Z space `$%*+-./:` only   | ~5.5 bits/char |
| `Mode::Byte`         | Any UTF-8 data                   | 8 bits/char    |

Mode is detected automatically from the input data.
It can be forced via the builder if needed.

---

## Typical workflow

Typical QR code generation workflow:

1. Provide input data as a string
2. Library detects the optimal encoding mode
3. Library selects the minimum version that fits
4. Reed-Solomon ECC is computed and interleaved
5. Matrix is built with all function patterns
6. Best mask pattern is selected via penalty scoring
7. Format and version information are placed
8. Output is rendered to ASCII, SVG, or PPM

### Internal flow

```text
input data
    → mode detection  (Numeric / Alphanumeric / Byte)
    → version selection  (1–40)
    → data encoding  (bit stream)
    → Reed-Solomon ECC
    → block interleaving
    → matrix construction
        finder patterns
        alignment patterns
        timing patterns
        format information
        data placement
    → mask selection  (8 candidates, penalty scoring)
    → render  (ASCII / SVG / PPM)
```

---

## Complexity

| Operation             | Time complexity |
|-----------------------|-----------------|
| Data encoding         | O(n)            |
| Reed-Solomon ECC      | O(n · k)        |
| Matrix construction   | O(v²)           |
| Mask selection        | O(8 · v²)       |
| SVG rendering         | O(v²)           |
| ASCII rendering       | O(v²)           |

Where **n** is the number of input characters,
**k** is the number of ECC codewords per block,
and **v** is the QR version (matrix side = 17 + 4v).

---

## Design principles

- Standard-compliant implementation
- Header-only simplicity
- Zero external dependencies
- Modern C++20 throughout
- Deterministic output
- Automatic parameter selection

This library focuses strictly on **QR code generation**.
If you need:

- QR code decoding / scanning
- Micro QR codes (ISO/IEC 29158)
- Structured append mode
- Kanji encoding

Build them on top of this layer or open an issue.

---

## Tests

Run:

```bash
vix build
vix test
```

**Tests verify:**

- Mode detection
- Version selection
- Matrix size
- ASCII rendering
- SVG rendering
- PPM export
- All ECC levels
- Numeric / Alphanumeric / Byte modes
- Builder API
- Module iteration
- Error handling

---

## License

MIT License\
Copyright (c) Gaspard Kirira

