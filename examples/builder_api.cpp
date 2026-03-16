/**
 * @file 07_builder_api.cpp
 * @brief QRCodeBuilder fluent API — fine-grained control over generation.
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 07_builder_api 07_builder_api.cpp
 */

#include <iostream>
#include "qr/qr.hpp"

int main() {
    // ── 1. Builder with EC level ──────────────────────────────────────────────
    {
        auto qr = qr::QRCodeBuilder("https://example.com")
                      .error_correction(qr::ECLevel::H)
                      .build();
        std::cout << "H-level URL:\n  " << qr.info() << "\n\n";
    }

    // ── 2. Force a specific version ───────────────────────────────────────────
    {
        // Force version 5 even if version 2 would suffice
        auto qr = qr::QRCodeBuilder("Hello")
                      .version(5)
                      .error_correction(qr::ECLevel::M)
                      .build();
        std::cout << "Forced version 5 for 'Hello':\n  " << qr.info() << "\n\n";
        std::cout << qr.to_ascii() << "\n";
    }

    // ── 3. Force encoding mode ────────────────────────────────────────────────
    {
        // Force Byte mode on numeric data (larger but sometimes needed for compatibility)
        auto qr = qr::QRCodeBuilder("01234567")
                      .mode(qr::Mode::Byte)
                      .error_correction(qr::ECLevel::M)
                      .build();
        std::cout << "Forced Byte mode on numeric:\n  " << qr.info() << "\n\n";
    }

    // ── 4. All options combined ───────────────────────────────────────────────
    {
        auto qr = qr::QRCodeBuilder("PRODUCT-SKU-XYZ-001")
                      .error_correction(qr::ECLevel::Q)
                      .version(4)
                      .build();
        std::cout << "Product SKU, Q-level, v4:\n  " << qr.info() << "\n\n";
        qr.save_svg("qr_product_sku.svg");
        std::cout << "  → saved qr_product_sku.svg\n\n";
    }

    // ── 5. Error: version too small for data ──────────────────────────────────
    {
        try {
            auto qr = qr::QRCodeBuilder("This string is way too long for version 1 with H error correction level")
                          .version(1)
                          .error_correction(qr::ECLevel::H)
                          .build();
        } catch (const std::exception& e) {
            std::cout << "Expected error caught: " << e.what() << "\n\n";
        }
    }

    // ── 6. Error: empty data ──────────────────────────────────────────────────
    {
        try {
            auto qr = qr::QRCodeBuilder("").build();
        } catch (const std::invalid_argument& e) {
            std::cout << "Empty data error: " << e.what() << "\n\n";
        }
    }

    return 0;
}
