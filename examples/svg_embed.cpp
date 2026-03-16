/**
 * @file 10_svg_embed.cpp
 * @brief Generate SVG strings and embed them inline in an HTML page.
 *
 * Produces a self-contained HTML file with multiple QR codes
 * displayed in a grid — no external files required.
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 10_svg_embed 10_svg_embed.cpp
 *
 * Output:
 *   qr_gallery.html
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "qr/qr.hpp"

struct QRItem {
    std::string label;
    std::string data;
    qr::ECLevel ec;
};

int main() {
    const std::vector<QRItem> items = {
        {"Homepage",     "https://example.com",                   qr::ECLevel::M},
        {"Wi-Fi",        "WIFI:T:WPA;S:MyNetwork;P:password123;;",qr::ECLevel::M},
        {"Email",        "mailto:hello@example.com",              qr::ECLevel::M},
        {"Phone",        "tel:+33612345678",                      qr::ECLevel::M},
        {"SMS",          "sms:+33612345678?body=Hello!",          qr::ECLevel::M},
        {"Location",     "geo:48.8566,2.3522",                    qr::ECLevel::M},
        {"GitHub",       "https://github.com",                    qr::ECLevel::Q},
        {"Numeric test", "0123456789",                            qr::ECLevel::L},
    };

    // Build SVG for each item with a dark modern style
    qr::SVGOptions opts;
    opts.module_size = 6;
    opts.quiet_zone  = 3;
    opts.dark_color  = "#1e293b";
    opts.bg_color    = "#f8fafc";

    std::ofstream html("qr_gallery.html");
    html << R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>QR Code Gallery</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    font-family: system-ui, -apple-system, sans-serif;
    background: #f1f5f9;
    padding: 2rem;
  }
  h1 {
    text-align: center;
    color: #0f172a;
    margin-bottom: 2rem;
    font-size: 1.75rem;
    font-weight: 700;
  }
  .grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(200px, 1fr));
    gap: 1.5rem;
    max-width: 960px;
    margin: 0 auto;
  }
  .card {
    background: white;
    border-radius: 12px;
    padding: 1.25rem;
    box-shadow: 0 1px 3px rgba(0,0,0,.08), 0 4px 16px rgba(0,0,0,.04);
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 0.75rem;
    transition: transform .15s, box-shadow .15s;
  }
  .card:hover {
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(0,0,0,.12);
  }
  .card svg { border-radius: 6px; width: 100%; height: auto; }
  .label {
    font-size: .8125rem;
    font-weight: 600;
    color: #64748b;
    text-transform: uppercase;
    letter-spacing: .05em;
  }
  .data {
    font-size: .75rem;
    color: #94a3b8;
    word-break: break-all;
    text-align: center;
    line-height: 1.4;
  }
  .badge {
    font-size: .6875rem;
    font-weight: 700;
    padding: .2em .5em;
    border-radius: 4px;
    background: #e2e8f0;
    color: #475569;
  }
</style>
</head>
<body>
<h1>QR Code Gallery</h1>
<div class="grid">
)";

    for (const auto& item : items) {
        auto qr  = qr::QRCodeBuilder(item.data)
                       .error_correction(item.ec)
                       .build();
        auto svg = qr.to_svg(opts);

        // Strip the XML declaration line for inline embedding
        auto pos = svg.find('\n');
        if (pos != std::string::npos) svg = svg.substr(pos + 1);

        html << "<div class=\"card\">\n";
        html << svg;
        html << "<div class=\"label\">" << item.label << "</div>\n";

        // Truncate long data strings for display
        std::string display = item.data;
        if (display.size() > 40) display = display.substr(0, 37) + "...";
        html << "<div class=\"data\">" << display << "</div>\n";

        // EC badge
        static constexpr std::array<const char*, 4> EC_NAMES = {"L","M","Q","H"};
        html << "<span class=\"badge\">EC-"
             << EC_NAMES[static_cast<int>(item.ec)]
             << " · v" << qr.version() << "</span>\n";
        html << "</div>\n";

        std::cout << "[OK] " << item.label << " — v" << qr.version()
                  << " " << EC_NAMES[static_cast<int>(item.ec)] << "\n";
    }

    html << "</div>\n</body>\n</html>\n";
    std::cout << "\n[OK] qr_gallery.html — open in browser\n";

    return 0;
}
