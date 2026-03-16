/**
 * @file 06_custom_renderer.cpp
 * @brief Build custom renderers using for_each_module().
 *
 * Demonstrates three custom output formats built entirely from the
 * raw module iterator — without using the built-in SVG/ASCII renderers:
 *
 *   A) HTML table renderer  (inline CSS, browser-ready)
 *   B) CSV matrix export    (row,col,value — useful for spreadsheets / ML)
 *   C) JSON array export    (for web APIs / JavaScript integration)
 *   D) Unicode half-block   (▀▄ characters — double-density terminal output)
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 06_custom_renderer 06_custom_renderer.cpp
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "qr/qr.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// A) HTML table renderer
// ─────────────────────────────────────────────────────────────────────────────
void save_html(const qr::QRCode& qr, const std::string& path,
               int cell_px = 8, int quiet = 3,
               const std::string& dark = "#111827",
               const std::string& light = "#f9fafb") {
    std::ofstream f(path);
    if (!f) throw std::runtime_error("Cannot open: " + path);

    const int sz = qr.size();
    const int total = sz + 2 * quiet;
    const std::string td_size = "width:" + std::to_string(cell_px)
                              + "px;height:" + std::to_string(cell_px) + "px;";

    f << "<!DOCTYPE html>\n<html><head><meta charset=\"UTF-8\">\n"
      << "<style>table{border-collapse:collapse;background:" << light << "}"
      << "td{" << td_size << "padding:0;border:none}</style></head>\n"
      << "<body><table>\n";

    // top quiet zone rows
    for (int q = 0; q < quiet; ++q) {
        f << "<tr>";
        for (int c = 0; c < total; ++c)
            f << "<td style=\"background:" << light << "\"></td>";
        f << "</tr>\n";
    }

    for (int r = 0; r < sz; ++r) {
        f << "<tr>";
        // left quiet zone
        for (int q = 0; q < quiet; ++q)
            f << "<td style=\"background:" << light << "\"></td>";
        // data row
        for (int c = 0; c < sz; ++c) {
            bool dark_mod = qr.module(r, c);
            f << "<td style=\"background:" << (dark_mod ? dark : light) << "\"></td>";
        }
        // right quiet zone
        for (int q = 0; q < quiet; ++q)
            f << "<td style=\"background:" << light << "\"></td>";
        f << "</tr>\n";
    }

    // bottom quiet zone rows
    for (int q = 0; q < quiet; ++q) {
        f << "<tr>";
        for (int c = 0; c < total; ++c)
            f << "<td style=\"background:" << light << "\"></td>";
        f << "</tr>\n";
    }

    f << "</table></body></html>\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// B) CSV matrix export
// ─────────────────────────────────────────────────────────────────────────────
void save_csv(const qr::QRCode& qr, const std::string& path) {
    std::ofstream f(path);
    if (!f) throw std::runtime_error("Cannot open: " + path);

    f << "row,col,dark\n";
    qr.for_each_module([&](int row, int col, bool dark) {
        f << row << ',' << col << ',' << (dark ? 1 : 0) << '\n';
    });
}

// ─────────────────────────────────────────────────────────────────────────────
// C) JSON array export
// ─────────────────────────────────────────────────────────────────────────────
void save_json(const qr::QRCode& qr, const std::string& path) {
    std::ofstream f(path);
    if (!f) throw std::runtime_error("Cannot open: " + path);

    const int sz = qr.size();
    f << "{\n";
    f << "  \"version\": "  << qr.version() << ",\n";
    f << "  \"size\": "     << sz << ",\n";
    f << "  \"ec_level\": " << static_cast<int>(qr.ec_level()) << ",\n";
    f << "  \"modules\": [\n";

    for (int r = 0; r < sz; ++r) {
        f << "    [";
        for (int c = 0; c < sz; ++c) {
            f << (qr.module(r, c) ? '1' : '0');
            if (c < sz - 1) f << ',';
        }
        f << ']';
        if (r < sz - 1) f << ',';
        f << '\n';
    }
    f << "  ]\n}\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// D) Unicode half-block terminal renderer (▀ ▄ █ space)
//    Each character cell represents 2 rows of modules.
//    Top half = ▀, bottom half = ▄, both dark = █, both light = space.
// ─────────────────────────────────────────────────────────────────────────────
std::string to_halfblock(const qr::QRCode& qr, int quiet = 2) {
    const int sz  = qr.size();
    const int tot = sz + 2 * quiet;

    // Build a padded 2D grid (bool)
    std::vector<std::vector<bool>> grid(
        static_cast<std::size_t>(tot),
        std::vector<bool>(static_cast<std::size_t>(tot), false)
    );
    for (int r = 0; r < sz; ++r)
        for (int c = 0; c < sz; ++c)
            grid[static_cast<std::size_t>(r + quiet)][static_cast<std::size_t>(c + quiet)] = qr.module(r, c);

    // Pad to even number of rows
    if (tot % 2 != 0)
        grid.emplace_back(static_cast<std::size_t>(tot), false);

    std::string out;
    for (int r = 0; r < static_cast<int>(grid.size()); r += 2) {
        for (int c = 0; c < tot; ++c) {
            bool top = grid[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)];
            bool bot = grid[static_cast<std::size_t>(r+1)][static_cast<std::size_t>(c)];
            if      ( top &&  bot) out += "\xe2\x96\x88"; // █  U+2588 FULL BLOCK
            else if ( top && !bot) out += "\xe2\x96\x80"; // ▀  U+2580 UPPER HALF BLOCK
            else if (!top &&  bot) out += "\xe2\x96\x84"; // ▄  U+2584 LOWER HALF BLOCK
            else                   out += " ";
        }
        out += '\n';
    }
    return out;
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {
    const std::string DATA = "https://example.com/demo";
    auto qr = qr::generate(DATA, qr::ECLevel::M);

    std::cout << qr.info() << "\n\n";

    // A) HTML
    save_html(qr, "qr_custom.html", 8, 3, "#0f172a", "#f8fafc");
    std::cout << "[A] qr_custom.html — HTML table renderer\n";

    // B) CSV
    save_csv(qr, "qr_matrix.csv");
    std::cout << "[B] qr_matrix.csv  — " << (qr.size() * qr.size())
              << " rows (row,col,dark)\n";

    // C) JSON
    save_json(qr, "qr_matrix.json");
    std::cout << "[C] qr_matrix.json — full matrix as JSON 2D array\n";

    // D) Half-block terminal
    std::cout << "\n[D] Half-block terminal output (▀▄ encoding):\n\n";
    std::cout << to_halfblock(qr) << "\n";

    // E) Raw module dump (for custom integrations)
    std::cout << "[E] Raw module iteration — counting dark modules:\n";
    int dark = 0;
    qr.for_each_module([&](int, int, bool d) { if (d) ++dark; });
    std::cout << "    Total modules : " << qr.size() * qr.size() << "\n";
    std::cout << "    Dark modules  : " << dark << "\n";
    std::cout << "    Light modules : " << (qr.size() * qr.size() - dark) << "\n";
    std::cout << "    Dark ratio    : "
              << (100.0 * dark / (qr.size() * qr.size())) << "%\n";

    return 0;
}
