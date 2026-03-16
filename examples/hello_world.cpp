/**
 * @file 01_hello_world.cpp
 * @brief Simplest possible usage — generate and print to terminal.
 *
 * Build:
 *   g++ -std=c++20 -O2 -I../include -o 01_hello_world 01_hello_world.cpp
 *
 * Expected output:
 *   QR Code v1, EC=M, Mode=Byte, Size=21x21, Data="Hello, World!"
 *   (followed by the QR code in terminal block characters)
 */

#include <iostream>
#include "qr/qr.hpp"

int main() {
    // ── Minimal: one-liner print to stdout ───────────────────────────────────
    qr::print("Hello, World!");

    // ── Or step by step ──────────────────────────────────────────────────────
    auto qr = qr::generate("Hello, World!");

    // Print metadata
    std::cout << "\n" << qr.info() << "\n";
    std::cout << "Version : " << qr.version()  << "\n";
    std::cout << "Size    : " << qr.size() << "x" << qr.size() << " modules\n\n";

    // Print ASCII to stdout
    std::cout << qr.to_ascii() << "\n";

    return 0;
}
