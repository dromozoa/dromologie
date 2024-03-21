#include "common.hpp"

#include <exception>
#include <iostream>

using namespace dromologie;

int main(int, char*[]) {
  try {

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
