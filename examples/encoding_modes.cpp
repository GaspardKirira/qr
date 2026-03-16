/**
 * @file 03_encoding_modes.cpp
 * @brief Demonstrate automatic mode detection and explicit mode selection.
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 03_encoding_modes 03_encoding_modes.cpp
 *
 * QR mode hierarchy (most compact first):
 *   Numeric      — digits 0-9 only           (~3.3 bits/char)
 *   Alphanumeric — 0-9 A-Z space $%*+-./: only (~5.5 bits/char)
 *   Byte         — any UTF-8 data            (8 bits/char)
 */

#include <iostream>
#include <string_view>
#include "qr/qr.hpp"

static std::string_view mode_name(qr::Mode m) {
    switch (m) {
        case qr::Mode::Numeric:      return "Numeric";
        case qr::Mode::Alphanumeric: return "Alphanumeric";
        case qr::Mode::Byte:         return "Byte";
    }
    return "Unknown";
}

static void show(std::string_view label, std::string_view data) {
    auto qr = qr::generate(data, qr::ECLevel::M);
    std::cout << "┌─ " << label << "\n";
    std::cout << "│  Data    : \"" << data << "\"\n";
    std::cout << "│  Mode    : " << mode_name(qr.mode()) << "\n";
    std::cout << "│  Version : " << qr.version() << "  (" << qr.size() << "x" << qr.size() << ")\n";
    std::cout << "└─────────────────────────────────\n\n";
    std::cout << qr.to_ascii() << "\n";
}

int main() {
    // Auto-detected as Numeric (digits only)
    show("Numeric — phone number", "0123456789012");

    // Auto-detected as Alphanumeric (uppercase + allowed symbols)
    show("Alphanumeric — short URL", "HTTPS://EXAMPLE.COM");

    // Auto-detected as Byte (lowercase, special chars, UTF-8)
    show("Byte — full URL with path", "https://example.com/products?id=42&ref=qr");

    // Byte mode — emoji / unicode
    show("Byte — unicode", "QR: 二次元コード 🔳");

    // Explicit builder with forced mode override
    auto qr = qr::QRCodeBuilder("01234")
                  .mode(qr::Mode::Byte)   // force Byte even though Numeric would fit
                  .error_correction(qr::ECLevel::M)
                  .build();
    std::cout << "── Forced Byte mode on numeric data ──\n";
    std::cout << "  Mode    : " << mode_name(qr.mode()) << "\n";
    std::cout << "  Version : " << qr.version() << "\n\n";
    std::cout << qr.to_ascii() << "\n";

    return 0;
}
