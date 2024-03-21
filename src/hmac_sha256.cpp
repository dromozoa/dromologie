#include "common.hpp"

#include <mbedtls/md.h>

#include <exception>
#include <iostream>
#include <vector>

using namespace dromologie;

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cerr << argv[0] << " key <message\n";
      return 1;
    }

    const auto key = decode_base64<std::vector<unsigned char>>(argv[1]);

    context<mbedtls_md_context_t, mbedtls_md_init, mbedtls_md_free> md;

    check(mbedtls_md_setup(md.get(), mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), true));
    check(mbedtls_md_hmac_starts(md.get(), key.data(), key.size()));
    std::vector<unsigned char> buffer(4096);
    while (!std::cin.eof()) {
      std::cin.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
      std::streamsize size = std::cin.gcount();
      if (size > 0) {
        check(mbedtls_md_hmac_update(md.get(), buffer.data(), size));
      }
    }
    check(mbedtls_md_hmac_finish(md.get(), buffer.data()));
    std::cout.write(reinterpret_cast<const char*>(buffer.data()), 32);

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
