/**
 * @file 05_batch_generator.cpp
 * @brief Generate a batch of QR codes (e.g. product URLs, serials, tickets).
 *
 * Demonstrates:
 *  - Loop generation
 *  - Naming output files dynamically
 *  - Generating both SVG and PPM in one pass
 *  - Measuring generation time
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 05_batch_generator 05_batch_generator.cpp
 */

#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "qr/qr.hpp"

int main() {
    // ── 1. Batch: product URLs ───────────────────────────────────────────────
    {
        std::cout << "=== Batch: Product URLs ===\n";
        const std::string BASE_URL = "https://shop.example.com/product/";
        const int COUNT = 5;

        auto t0 = std::chrono::steady_clock::now();

        for (int i = 1; i <= COUNT; ++i) {
            std::string url  = BASE_URL + std::to_string(i);
            std::string file = "product_" + std::to_string(i) + ".svg";

            qr::SVGOptions opts;
            opts.module_size = 10;
            opts.dark_color  = "#111827";
            opts.bg_color    = "#f9fafb";

            auto qr = qr::generate(url, qr::ECLevel::M);
            qr.save_svg(file, opts);
            std::cout << "  [" << std::setw(2) << i << "] " << file
                      << "  v" << qr.version()
                      << "  " << qr.size() << "x" << qr.size() << "\n";
        }

        auto t1  = std::chrono::steady_clock::now();
        auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::cout << "  Generated " << COUNT << " QR codes in " << ms << " ms\n\n";
    }

    // ── 2. Batch: ticket serials (numeric mode → compact) ───────────────────
    {
        std::cout << "=== Batch: Event Tickets (numeric serials) ===\n";
        const std::vector<std::string> SERIALS = {
            "100001", "100002", "100003", "100004", "100005",
            "100006", "100007", "100008", "100009", "100010",
        };

        qr::SVGOptions opts;
        opts.module_size = 8;
        opts.dark_color  = "#1e3a5f";
        opts.bg_color    = "#ffffff";
        opts.quiet_zone  = 3;

        for (auto& serial : SERIALS) {
            auto qr   = qr::generate(serial, qr::ECLevel::H); // H for durability
            std::string file = "ticket_" + serial + ".svg";
            qr.save_svg(file, opts);
        }
        std::cout << "  Generated " << SERIALS.size() << " ticket QR codes (EC=H, Numeric)\n\n";
    }

    // ── 3. Batch: WiFi credentials per room ─────────────────────────────────
    {
        std::cout << "=== Batch: WiFi QR codes ===\n";
        struct Room { const char* name; const char* ssid; const char* pass; };
        static constexpr Room ROOMS[] = {
            { "lobby",    "Hotel-Lobby",    "Welcome2024!" },
            { "room101",  "Hotel-Room101",  "Room101#Pass" },
            { "room102",  "Hotel-Room102",  "Room102#Pass" },
            { "pool",     "Hotel-Pool",     "PoolWifi99"   },
            { "conf_a",   "Hotel-ConfA",    "ConfA@2024"   },
        };

        qr::SVGOptions opts;
        opts.module_size = 12;
        opts.dark_color  = "#064e3b";   // hotel green
        opts.bg_color    = "#ecfdf5";
        opts.quiet_zone  = 4;

        for (auto& r : ROOMS) {
            // Standard WiFi QR format
            std::string wifi = std::string("WIFI:T:WPA;S:") + r.ssid
                             + ";P:" + r.pass + ";;";
            auto qr   = qr::generate(wifi, qr::ECLevel::M);
            std::string file = std::string("wifi_") + r.name + ".svg";
            qr.save_svg(file, opts);
            std::cout << "  " << std::left << std::setw(12) << r.name
                      << " → " << file
                      << "  v" << qr.version() << "\n";
        }
        std::cout << "\n";
    }

    // ── 4. Batch: vCards ────────────────────────────────────────────────────
    {
        std::cout << "=== Batch: vCards ===\n";
        struct Person {
            const char* id;
            const char* fn;
            const char* tel;
            const char* email;
        };
        static constexpr Person PEOPLE[] = {
            { "alice", "Alice Martin",  "+33600000001", "alice@example.com"  },
            { "bob",   "Bob Dupont",    "+33600000002", "bob@example.com"    },
            { "carol", "Carol Nguyen",  "+33600000003", "carol@example.com"  },
        };

        for (auto& p : PEOPLE) {
            std::string vcard =
                std::string("BEGIN:VCARD\nVERSION:3.0\n")
                + "FN:" + p.fn + "\n"
                + "TEL:" + p.tel + "\n"
                + "EMAIL:" + p.email + "\n"
                + "END:VCARD";

            auto qr   = qr::generate(vcard, qr::ECLevel::M);
            std::string file = std::string("vcard_") + p.id + ".svg";

            qr::SVGOptions opts;
            opts.module_size = 10;
            opts.dark_color  = "#1e1b4b";
            opts.bg_color    = "#f5f3ff";

            qr.save_svg(file, opts);
            std::cout << "  " << std::left << std::setw(8) << p.id
                      << " → " << file
                      << "  v" << qr.version()
                      << "  " << vcard.size() << " bytes\n";
        }
        std::cout << "\n";
    }

    std::cout << "All done. Check *.svg files in current directory.\n";
    return 0;
}
