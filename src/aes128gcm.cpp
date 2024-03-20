#include "common.hpp"

#include <mbedtls/gcm.h>

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

using namespace dromologie;

int main(int argc, char* argv[]) {
  try {
    if (argc < 3) {
      std::cerr << argv[0] << " hex-key hex-iv <input >output\n";
      return 1;
    }

    const std::vector<unsigned char> key = decode_hex(argv[1]);
    const std::vector<unsigned char> iv = decode_hex(argv[2]);

    std::vector<unsigned char> input;
    std::copy(std::istreambuf_iterator<char>(std::cin), std::istreambuf_iterator<char>(), std::back_inserter(input));

    if (input.size() > 3993) {
      std::cerr << "input too large\n";
      return 1;
    }

    std::vector<unsigned char> output(4096);
    std::size_t output_size1 = 0;
    std::size_t output_size2 = 0;
    std::vector<unsigned char> tag(16);

    mbedtls_gcm_context gcm_context;
    std::unique_ptr<mbedtls_gcm_context, decltype(&mbedtls_gcm_free)> gcm(&gcm_context, mbedtls_gcm_free);
    mbedtls_gcm_init(gcm.get());

    check(mbedtls_gcm_setkey(gcm.get(), MBEDTLS_CIPHER_ID_AES, &key[0], key.size() * 8));
    check(mbedtls_gcm_starts(gcm.get(), MBEDTLS_GCM_ENCRYPT, &iv[0], iv.size()));
    check(mbedtls_gcm_update(gcm.get(), &input[0], input.size(), &output[0], output.size(), &output_size1));
    check(mbedtls_gcm_finish(gcm.get(), &output[output_size1], output.size() - output_size1, &output_size2, &tag[0], tag.size()));

    std::cout.write(reinterpret_cast<const char*>(&output[0]), output_size1 + output_size2);
    std::cout.write(reinterpret_cast<const char*>(&tag[0]), tag.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
