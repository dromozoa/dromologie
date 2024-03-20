#include "common.hpp"

#include <exception>
#include <iostream>

using namespace dromologie;

int main(int, char*[]) {
  try {
    std::vector<char> data = read_all<char>(std::cin);
    std::cout << encode_base64url(data.data(), data.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
