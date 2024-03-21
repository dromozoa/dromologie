#include "common.hpp"

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>

#include <exception>
#include <iostream>
#include <string>
#include <vector>

using namespace dromologie;

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cerr << argv[0] << " size\n";
      return 1;
    }

    const auto size = std::stoi(argv[1]);
    if (size < 0) {
      std::cerr << "negative size\n";
      return 1;
    }

    context<mbedtls_entropy_context, mbedtls_entropy_init, mbedtls_entropy_free> entropy;
    context<mbedtls_ctr_drbg_context, mbedtls_ctr_drbg_init, mbedtls_ctr_drbg_free> ctr_drbg;

    check(mbedtls_ctr_drbg_seed(ctr_drbg.get(), mbedtls_entropy_func, entropy.get(), nullptr, 0));
    std::vector<unsigned char> buffer(size);
    check(mbedtls_ctr_drbg_random(ctr_drbg.get(), buffer.data(), buffer.size()));
    std::cout.write(reinterpret_cast<const char*>(buffer.data()), size);

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
