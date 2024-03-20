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
      std::cerr << argv[0] << " key iv <input >output\n";
      return 1;
    }

    std::vector<unsigned char> input = read_all<std::vector<unsigned char>>(std::cin);
    if (input.size() > 3993) {
      std::cerr << "input too large\n";
      return 1;
    }
    const auto key = decode_base64<std::vector<unsigned char>>(argv[1]);
    const auto iv = decode_base64<std::vector<unsigned char>>(argv[2]);

    std::vector<unsigned char> output(4096);
    std::size_t output_size = 0;
    std::vector<unsigned char> tag(16);

    context<mbedtls_gcm_context, mbedtls_gcm_init, mbedtls_gcm_free> gcm;
    check(mbedtls_gcm_setkey(gcm.get(), MBEDTLS_CIPHER_ID_AES, key.data(), key.size() * 8));
    check(mbedtls_gcm_starts(gcm.get(), MBEDTLS_GCM_ENCRYPT, iv.data(), iv.size()));
    check(mbedtls_gcm_update(gcm.get(), input.data(), input.size(), output.data(), output.size(), &output_size));
    std::cout.write(reinterpret_cast<const char*>(output.data()), output_size);
    check(mbedtls_gcm_finish(gcm.get(), output.data(), output.size(), &output_size, &tag[0], tag.size()));
    std::cout.write(reinterpret_cast<const char*>(output.data()), output_size);
    std::cout.write(reinterpret_cast<const char*>(&tag[0]), tag.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
