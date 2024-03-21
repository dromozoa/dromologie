#include "common.hpp"

#include <mbedtls/gcm.h>

#include <cstddef>
#include <exception>
#include <iostream>
#include <vector>

using namespace dromologie;

int main(int argc, char* argv[]) {
  try {
    if (argc < 3) {
      std::cerr << argv[0] << " key iv <plain >cipher\n";
      return 1;
    }

    std::vector<unsigned char> source = read_all<std::vector<unsigned char>>(std::cin);
    if (source.size() > 3993) {
      std::cerr << "source too large\n";
      return 1;
    }
    const auto key = decode_base64<std::vector<unsigned char>>(argv[1]);
    const auto iv = decode_base64<std::vector<unsigned char>>(argv[2]);

    std::vector<unsigned char> buffer(4096);
    std::size_t size = 0;
    std::vector<unsigned char> tag(16);

    context<mbedtls_gcm_context, mbedtls_gcm_init, mbedtls_gcm_free> gcm;

    check(mbedtls_gcm_setkey(gcm.get(), MBEDTLS_CIPHER_ID_AES, key.data(), key.size() * 8));
    check(mbedtls_gcm_starts(gcm.get(), MBEDTLS_GCM_ENCRYPT, iv.data(), iv.size()));
    check(mbedtls_gcm_update(gcm.get(), source.data(), source.size(), buffer.data(), buffer.size(), &size));
    std::cout.write(reinterpret_cast<const char*>(buffer.data()), size);
    check(mbedtls_gcm_finish(gcm.get(), buffer.data(), buffer.size(), &size, &tag[0], tag.size()));
    std::cout.write(reinterpret_cast<const char*>(buffer.data()), size);
    std::cout.write(reinterpret_cast<const char*>(&tag[0]), tag.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
