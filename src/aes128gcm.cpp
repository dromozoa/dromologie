#include <mbedtls/build_info.h>
#include <mbedtls/cipher.h>
#include <mbedtls/error.h>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// key
// nonce
// salt
// plain text



std::vector<unsigned char> decode_hex(const std::string& source) {
  std::vector<unsigned char> result;
  for (std::size_t i = 0; i < source.size(); i += 2) {
    result.push_back(std::stoi(source.substr(i, 2), nullptr, 16));
  }
  return result;
}

void check(int result) {
  if (result != 0) {
    std::vector<char> buffer(256);
    mbedtls_strerror(result, &buffer[0], buffer.size());
    throw std::runtime_error(&buffer[0]);
  }
}

int main(int argc, char* argv[]) {
  try {
    if (argc < 3) {
      std::cerr << argv[0] << " hex-key hex-iv\n";
      return 1;
    }

    const auto key = decode_hex(argv[1]);
    const auto iv = decode_hex(argv[2]);

    mbedtls_cipher_context_t context;
    std::unique_ptr<mbedtls_cipher_context_t, decltype(&mbedtls_cipher_free)> ctx(&context, mbedtls_cipher_free);
    mbedtls_cipher_init(ctx.get());

    check(mbedtls_cipher_setup(ctx.get(), mbedtls_cipher_info_from_type(MBEDTLS_CIPHER_AES_128_GCM)));
    check(mbedtls_cipher_setkey(ctx.get(), &key[0], key.size() * 8, MBEDTLS_ENCRYPT));
    check(mbedtls_cipher_set_iv(ctx.get(), &iv[0], iv.size() * 8));

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
