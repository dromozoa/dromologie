#include "common.hpp"

#include <exception>
#include <iostream>
#include <string>

using namespace dromologie;

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cout << decode_base64<std::string>(read_all<std::string>(std::cin));
    } else {
      std::cout << decode_base64<std::string>(argv[1]);
    }
  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
