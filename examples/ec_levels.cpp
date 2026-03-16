/**
 * @file 02_ec_levels.cpp
 * @brief Generate the same data at all four ECC levels and compare.
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 02_ec_levels 02_ec_levels.cpp
 *
 * This example shows that:
 *  - Higher ECC → higher version (bigger QR code)
 *  - L recovers  7% of damage
 *  - M recovers 15% of damage  (default, good balance)
 *  - Q recovers 25% of damage  (recommended for logos)
 *  - H recovers 30% of damage  (harsh environments)
 */

#include <array>
#include <iostream>
#include <string_view>
#include "qr/qr.hpp"

int main() {
    const std::string data = "https://example.com/products/42";

    constexpr std::array<qr::ECLevel, 4> levels = {
        qr::ECLevel::L, qr::ECLevel::M, qr::ECLevel::Q, qr::ECLevel::H
    };
    constexpr std::array<std::string_view, 4> names = {"L (7%)", "M (15%)", "Q (25%)", "H (30%)"};

    std::cout << "Data: \"" << data << "\"\n";
    std::cout << "══════════════════════════════════════\n\n";

    for (std::size_t i = 0; i < levels.size(); ++i) {
        auto qr = qr::generate(data, levels[i]);
        std::cout << "── ECC Level " << names[i] << " ──\n";
        std::cout << "  Version : " << qr.version() << "\n";
        std::cout << "  Size    : " << qr.size() << "x" << qr.size() << "\n\n";
        std::cout << qr.to_ascii() << "\n";
    }

    return 0;
}
