/**
 * @file 02_svg_export.cpp
 * @brief Generate QR codes as SVG files with various styling options.
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 02_svg_export 02_svg_export.cpp
 *
 * Run:
 *   ./02_svg_export
 *   # Produces: qr_default.svg, qr_dark.svg, qr_inverted.svg, qr_large.svg
 */

#include <iostream>
#include "qr/qr.hpp"

int main() {
    // ── 1. Default SVG ──────────────────────────────────────────────────────
    {
        auto qr = qr::generate("https://example.com", qr::ECLevel::M);
        qr.save_svg("qr_default.svg");
        std::cout << "[OK] qr_default.svg  — " << qr.info() << "\n";
    }

    // ── 2. Dark theme ───────────────────────────────────────────────────────
    {
        qr::SVGOptions opts;
        opts.dark_color  = "#e2e8f0";   // light modules on dark bg
        opts.bg_color    = "#0f172a";   // dark navy background
        opts.light_color = "#0f172a";
        opts.module_size = 12;
        opts.quiet_zone  = 4;

        auto qr = qr::generate("https://example.com", qr::ECLevel::M);
        qr.save_svg("qr_dark.svg", opts);
        std::cout << "[OK] qr_dark.svg     — dark navy theme\n";
    }

    // ── 3. Inverted (white on black) ────────────────────────────────────────
    {
        qr::SVGOptions opts;
        opts.invert      = true;
        opts.bg_color    = "#000000";
        opts.dark_color  = "#000000";
        opts.light_color = "#ffffff";
        opts.module_size = 10;

        auto qr = qr::generate("INVERTED QR CODE", qr::ECLevel::H);
        qr.save_svg("qr_inverted.svg", opts);
        std::cout << "[OK] qr_inverted.svg — inverted, EC=H\n";
    }

    // ── 4. Large modules, brand colour ──────────────────────────────────────
    {
        qr::SVGOptions opts;
        opts.module_size = 24;
        opts.quiet_zone  = 6;
        opts.dark_color  = "#7c3aed";   // violet
        opts.bg_color    = "#faf5ff";   // pale lavender background

        auto qr = qr::generate("https://github.com", qr::ECLevel::Q);
        qr.save_svg("qr_large.svg", opts);
        std::cout << "[OK] qr_large.svg    — 24px modules, violet brand colour\n";
    }

    // ── 5. SVG string (in-memory, no file) ──────────────────────────────────
    {
        auto qr = qr::generate("In-memory SVG");
        std::string svg = qr.to_svg();
        std::cout << "[OK] to_svg()        — " << svg.size() << " bytes in memory\n";
    }

    // ── 6. One-liner free function ───────────────────────────────────────────
    {
        qr::make_svg("One-liner!", "qr_oneliner.svg", qr::ECLevel::L);
        std::cout << "[OK] qr_oneliner.svg — free function qr::make_svg()\n";
    }

    return 0;
}
