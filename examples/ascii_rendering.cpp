/**
 * @file 05_ascii_rendering.cpp
 * @brief Various ASCII rendering styles for terminal output.
 */

#include <iostream>
#include "qr/qr.hpp"

int main()
{
  auto qr = qr::generate("https://example.com", qr::ECLevel::M);

  std::cout << "── Default (██ / spaces) ──\n";
  std::cout << qr.to_ascii() << "\n";

  {
    qr::ASCIIOptions opts;
    opts.dark_char = "##";
    opts.light_char = "  ";
    opts.quiet_zone = 1;
    std::cout << "── Hash style ──\n";
    std::cout << qr.to_ascii(opts) << "\n";
  }

  {
    qr::ASCIIOptions opts;
    opts.dark_char = "▓▓";
    opts.light_char = "░░";
    opts.quiet_zone = 2;
    std::cout << "── Dot matrix (░/▓) ──\n";
    std::cout << qr.to_ascii(opts) << "\n";
  }

  {
    qr::ASCIIOptions opts;
    opts.dark_char = "X";
    opts.light_char = " ";
    opts.quiet_zone = 1;
    std::cout << "── Single char compact (X) ──\n";
    std::cout << qr.to_ascii(opts) << "\n";
  }

  {
    qr::ASCIIOptions opts;
    opts.invert = true;
    opts.quiet_zone = 2;
    std::cout << "── Inverted (dark/light swapped) ──\n";
    std::cout << qr.to_ascii(opts) << "\n";
  }

  {
    qr::ASCIIOptions opts;
    opts.quiet_zone = 0;
    std::cout << "── No quiet zone ──\n";
    std::cout << qr.to_ascii(opts) << "\n";
  }

  return 0;
}
