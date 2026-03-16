/**
 * @file 06_ppm_export.cpp
 * @brief Export QR codes as PPM images (no dependencies, pure C++).
 *
 * PPM (Portable PixMap) is a simple uncompressed image format.
 * It can be opened with:
 *   - GIMP, Photoshop, Preview (macOS)
 *   - ImageMagick: convert qr.ppm qr.png
 *   - FFmpeg:      ffmpeg -i qr.ppm qr.png
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 06_ppm_export 06_ppm_export.cpp
 */

#include <iostream>
#include "qr/qr.hpp"

int main() {
    // ── Standard export ──────────────────────────────────────────────────────
    {
        auto qr = qr::generate("https://example.com", qr::ECLevel::M);
        qr.save_ppm("qr_standard.ppm");
        const int px = (qr.size() + 2*4) * 10;  // default: 10px module, 4 quiet zone
        std::cout << "[OK] qr_standard.ppm  — " << px << "x" << px << " px\n";
    }

    // ── Large modules (print-friendly) ───────────────────────────────────────
    {
        auto qr = qr::generate("PRINT READY QR CODE", qr::ECLevel::Q);
        qr.save_ppm("qr_large.ppm", /*module_size=*/20, /*quiet_zone=*/6);
        const int px = (qr.size() + 2*6) * 20;
        std::cout << "[OK] qr_large.ppm     — " << px << "x" << px << " px (20px/module)\n";
    }

    // ── Tiny thumbnail ───────────────────────────────────────────────────────
    {
        auto qr = qr::generate("TINY", qr::ECLevel::L);
        qr.save_ppm("qr_tiny.ppm", /*module_size=*/3, /*quiet_zone=*/2);
        const int px = (qr.size() + 2*2) * 3;
        std::cout << "[OK] qr_tiny.ppm      — " << px << "x" << px << " px (3px/module)\n";
    }

    // ── High ECC for damaged/printed QR ─────────────────────────────────────
    {
        auto qr = qr::generate("https://example.com/very/important/link", qr::ECLevel::H);
        qr.save_ppm("qr_high_ecc.ppm", 12, 5);
        std::cout << "[OK] qr_high_ecc.ppm  — v" << qr.version() << " H-level, 12px/module\n";
    }

    std::cout << "\nConvert to PNG with ImageMagick:\n";
    std::cout << "  convert qr_standard.ppm qr_standard.png\n";

    return 0;
}
