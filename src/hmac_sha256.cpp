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

    std::vector<unsigned char> source;
    if (argc < 3) {
      source = read_all<std::vector<unsigned char>>(std::cin);
    } else {
      source = decode_base64<std::vector<unsigned char>>(argv[2]);
    }
    const auto key = decode_base64<std::vector<unsigned char>>(argv[1]);

    std::vector<unsigned char> buffer(32);

    context<mbedtls_md_context_t, mbedtls_md_init, mbedtls_md_free> md;

    check(mbedtls_md_setup(md.get(), mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), true));
    check(mbedtls_md_hmac_starts(md.get(), key.data(), key.size()));
    check(mbedtls_md_hmac_update(md.get(), source.data(), source.size()));

    check(mbedtls_md_hmac_finish(md.get(), buffer.data()));
    std::cout.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
