#include "common.hpp"

#include <exception>
#include <iostream>
#include <string>

using namespace dromologie;

int main(int, char*[]) {
  try {
    std::cout << encode_base64url(read_all<std::string>(std::cin));
  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
