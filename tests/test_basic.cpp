#include <qr/qr.hpp>
#include <iostream>

int main()
{
  auto nodes = qr::make_chain(5);
  std::cout << "nodes=" << nodes.size() << "\n";
  return nodes.size() == 5 ? 0 : 1;
}
