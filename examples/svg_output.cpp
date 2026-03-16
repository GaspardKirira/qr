/**
 * @file 04_svg_output.cpp
 * @brief Generate styled SVG QR codes — default, branded, dark theme, inverted.
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 04_svg_output 04_svg_output.cpp
 *
 * Output files:
 *   qr_default.svg   — standard black on white
 *   qr_branded.svg   — custom brand colors, larger modules
 *   qr_dark.svg      — dark theme (white on dark background)
 *   qr_compact.svg   — small module size, tight quiet zone
 */

#include <iostream>
#include "qr/qr.hpp"

int main() {
    const std::string url = "https://example.com";

    // ── 1. Default SVG ───────────────────────────────────────────────────────
    {
        auto qr = qr::generate(url, qr::ECLevel::M);
        qr.save_svg("qr_default.svg");
        std::cout << "[OK] qr_default.svg  (" << qr.size() << "x" << qr.size() << " modules)\n";
    }

    // ── 2. Branded SVG ───────────────────────────────────────────────────────
    {
        qr::SVGOptions opts;
        opts.module_size = 16;
        opts.quiet_zone  = 4;
        opts.dark_color  = "#1a1a2e";   // deep navy
        opts.bg_color    = "#f5f0e8";   // warm off-white
        opts.light_color = "#f5f0e8";

        auto qr = qr::generate(url, qr::ECLevel::Q);  // Q for logo overlay headroom
        qr.save_svg("qr_branded.svg", opts);
        std::cout << "[OK] qr_branded.svg  (16px modules, Q ECC, navy on cream)\n";
    }

    // ── 3. Dark theme ────────────────────────────────────────────────────────
    {
        qr::SVGOptions opts;
        opts.module_size = 12;
        opts.quiet_zone  = 4;
        opts.dark_color  = "#e2e8f0";   // near-white modules
        opts.bg_color    = "#0f172a";   // dark slate background
        opts.light_color = "#0f172a";

        auto qr = qr::generate(url, qr::ECLevel::M);
        qr.save_svg("qr_dark.svg", opts);
        std::cout << "[OK] qr_dark.svg     (dark theme, slate bg)\n";
    }

    // ── 4. Inverted (light modules on dark bg via invert flag) ───────────────
    {
        qr::SVGOptions opts;
        opts.module_size = 10;
        opts.quiet_zone  = 3;
        opts.invert      = true;
        opts.bg_color    = "#111111";

        auto qr = qr::generate(url, qr::ECLevel::M);
        qr.save_svg("qr_inverted.svg", opts);
        std::cout << "[OK] qr_inverted.svg (inverted, dark bg)\n";
    }

    // ── 5. High-density compact ──────────────────────────────────────────────
    {
        qr::SVGOptions opts;
        opts.module_size = 4;
        opts.quiet_zone  = 2;

        auto qr = qr::generate(url, qr::ECLevel::L);
        qr.save_svg("qr_compact.svg", opts);
        const int px = (qr.size() + 2 * opts.quiet_zone) * opts.module_size;
        std::cout << "[OK] qr_compact.svg  (" << px << "x" << px << " px total)\n";
    }

    // ── 6. One-liner free function ───────────────────────────────────────────
    {
        qr::SVGOptions opts;
        opts.module_size = 10;
        qr::make_svg("https://github.com", "qr_github.svg", qr::ECLevel::M, opts);
        std::cout << "[OK] qr_github.svg   (free function make_svg)\n";
    }

    std::cout << "\nAll SVG files written to current directory.\n";
    return 0;
}
