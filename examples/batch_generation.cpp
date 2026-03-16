/**
 * @file 09_batch_generation.cpp
 * @brief Generate multiple QR codes in a loop — URLs, products, vCards.
 */

#include <filesystem>
#include <format>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "qr/qr.hpp"

namespace fs = std::filesystem;

struct Product
{
  int id;
  std::string name;
  std::string sku;
  std::string url;
};

std::string make_vcard(std::string_view name, std::string_view phone,
                       std::string_view email, std::string_view url = "")
{
  std::string v;
  v = "BEGIN:VCARD\r\nVERSION:3.0\r\n";
  v += "FN:" + std::string(name) + "\r\n";
  v += "TEL:" + std::string(phone) + "\r\n";
  v += "EMAIL:" + std::string(email) + "\r\n";
  if (!url.empty())
    v += "URL:" + std::string(url) + "\r\n";
  v += "END:VCARD\r\n";
  return v;
}

std::string make_wifi(std::string_view ssid, std::string_view password,
                      std::string_view security = "WPA")
{
  return "WIFI:T:" + std::string(security) +
         ";S:" + std::string(ssid) +
         ";P:" + std::string(password) + ";;";
}

std::string make_sms(std::string_view phone, std::string_view message = "")
{
  std::string s = "sms:" + std::string(phone);
  if (!message.empty())
    s += "?body=" + std::string(message);
  return s;
}

std::string make_geo(double lat, double lon)
{
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(6);
  oss << "geo:" << lat << "," << lon;
  return oss.str();
}

int main()
{
  fs::create_directories("qr_batch");

  qr::SVGOptions opts;
  opts.module_size = 10;
  opts.quiet_zone = 4;
  opts.dark_color = "#000000";

  std::cout << "── Product Catalog QR Codes ──\n";
  const std::vector<Product> products = {
      {1, "Widget Pro", "WGT-001", "https://shop.example.com/products/1"},
      {2, "Gadget Plus", "GDG-002", "https://shop.example.com/products/2"},
      {3, "Doohickey", "DKY-003", "https://shop.example.com/products/3"},
      {42, "Premium Kit", "PRK-042", "https://shop.example.com/products/42"},
  };

  for (const auto &p : products)
  {
    std::string filename = "qr_batch/product_" + p.sku + ".svg";
    qr::QRCodeBuilder(p.url)
        .error_correction(qr::ECLevel::M)
        .build()
        .save_svg(filename, opts);
    std::cout << "  [OK] " << filename << "  (" << p.name << ")\n";
  }

  std::cout << "\n── vCard QR Codes ──\n";
  struct Contact
  {
    std::string name, phone, email;
  };
  const std::vector<Contact> contacts = {
      {"Alice Martin", "+33 6 12 34 56 78", "alice@example.com"},
      {"Bob Dupont", "+33 6 87 65 43 21", "bob@example.com"},
      {"Carol Schmidt", "+49 30 12345678", "carol@example.com"},
  };

  for (const auto &c : contacts)
  {
    std::string vcard = make_vcard(c.name, c.phone, c.email);
    std::string filename = "qr_batch/vcard_" + c.name.substr(0, c.name.find(' ')) + ".svg";
    qr::QRCodeBuilder(vcard)
        .error_correction(qr::ECLevel::M)
        .build()
        .save_svg(filename, opts);
    std::cout << "  [OK] " << filename << "\n";
  }

  std::cout << "\n── Wi-Fi QR Codes ──\n";
  {
    auto wifi_home = make_wifi("HomeNetwork", "mypassword123", "WPA");
    auto wifi_guest = make_wifi("GuestNetwork", "welcome2024", "WPA");
    auto wifi_open = make_wifi("PublicHotspot", "", "nopass");

    qr::QRCodeBuilder(wifi_home).error_correction(qr::ECLevel::M).build().save_svg("qr_batch/wifi_home.svg", opts);
    qr::QRCodeBuilder(wifi_guest).error_correction(qr::ECLevel::M).build().save_svg("qr_batch/wifi_guest.svg", opts);
    qr::QRCodeBuilder(wifi_open).error_correction(qr::ECLevel::M).build().save_svg("qr_batch/wifi_open.svg", opts);
    std::cout << "  [OK] qr_batch/wifi_*.svg\n";
  }

  std::cout << "\n── SMS QR Codes ──\n";
  {
    auto sms = make_sms("+33612345678", "Hello from QR code!");
    qr::generate(sms, qr::ECLevel::M).save_svg("qr_batch/sms_hello.svg", opts);
    std::cout << "  [OK] qr_batch/sms_hello.svg\n";
  }

  std::cout << "\n── Geo QR Codes ──\n";
  struct Location
  {
    std::string name;
    double lat, lon;
  };
  const std::vector<Location> locs = {
      {"paris", 48.8566, 2.3522},
      {"new_york", 40.7128, -74.0060},
      {"tokyo", 35.6762, 139.6503},
  };
  for (const auto &loc : locs)
  {
    std::string geo = make_geo(loc.lat, loc.lon);
    std::string file = "qr_batch/geo_" + loc.name + ".svg";
    qr::generate(geo, qr::ECLevel::M).save_svg(file, opts);
    std::cout << "  [OK] " << file << "\n";
  }

  std::cout << "\n── Ticket ID QR Codes (numeric mode) ──\n";
  for (int ticket = 1001; ticket <= 1005; ++ticket)
  {
    std::string id = std::to_string(ticket);
    std::string file = "qr_batch/ticket_" + id + ".svg";
    qr::QRCodeBuilder(id)
        .mode(qr::Mode::Numeric)
        .error_correction(qr::ECLevel::H) // H for durability
        .build()
        .save_svg(file, opts);
    std::cout << "  [OK] " << file << "\n";
  }

  std::cout << "\nAll files in qr_batch/\n";
  return 0;
}
