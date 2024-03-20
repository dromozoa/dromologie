#include "common.hpp"

using namespace dromologie;

#include <exception>
#include <iostream>

int main(int, char*[]) {
  try {
    std::vector<char> source;
    std::copy(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>(), std::back_inserter(source));
    std::cout << encode_base64url(source.data(), source.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
