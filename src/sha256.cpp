#include "common.hpp"

#include <mbedtls/sha256.h>

#include <exception>
#include <iostream>

using namespace dromologie;

int main(int, char*[]) {
  try {
    std::vector<unsigned char> buffer(4096);

    context<mbedtls_sha256_context, mbedtls_sha256_init, mbedtls_sha256_free> sha256;
    check(mbedtls_sha256_starts(sha256.get(), false));
    while (!std::cin.eof()) {
      std::cin.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
      std::streamsize size = std::cin.gcount();
      if (size > 0) {
        check(mbedtls_sha256_update(sha256.get(), buffer.data(), size));
      }
    }
    check(mbedtls_sha256_finish(sha256.get(), buffer.data()));
    std::cout.write(reinterpret_cast<const char*>(buffer.data()), 32);

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
