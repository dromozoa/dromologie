#include "common.hpp"

#include <exception>
#include <iostream>
#include <vector>

using namespace dromologie;

int main(int, char*[]) {
  try {
    std::vector<char> source = read_all<std::vector<char>>(std::cin);
    std::cout << encode_base64url(source.data(), source.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
