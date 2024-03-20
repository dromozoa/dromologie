#include "common.hpp"

#include <exception>
#include <iostream>
#include <string>

using namespace dromologie;

int main(int, char*[]) {
  try {
    std::vector<char> result = decode_base64url<char>(read_all<std::string>(std::cin));
    std::cout.write(result.data(), result.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
