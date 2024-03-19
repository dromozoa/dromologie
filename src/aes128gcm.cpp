#include <mbedtls/build_info.h>
#include <mbedtls/cipher.h>
#include <mbedtls/error.h>
#include <mbedtls/gcm.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

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
      std::cerr << argv[0] << " hex-key hex-iv <input >output\n";
      return 1;
    }

    const auto key = decode_hex(argv[1]);
    const auto iv = decode_hex(argv[2]);

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

    mbedtls_gcm_context context;
    std::unique_ptr<mbedtls_gcm_context, decltype(&mbedtls_gcm_free)> ctx(&context, mbedtls_gcm_free);
    mbedtls_gcm_init(ctx.get());

    check(mbedtls_gcm_setkey(ctx.get(), MBEDTLS_CIPHER_ID_AES, &key[0], key.size() * 8));
    check(mbedtls_gcm_starts(ctx.get(), MBEDTLS_GCM_ENCRYPT, &iv[0], iv.size()));
    check(mbedtls_gcm_update(ctx.get(), &input[0], input.size(), &output[0], output.size(), &output_size1));
    check(mbedtls_gcm_finish(ctx.get(), &output[output_size1], output.size() - output_size1, &output_size2, &tag[0], tag.size()));

    std::cout.write(reinterpret_cast<const char*>(&output[0]), output_size1 + output_size2);
    std::cout.write(reinterpret_cast<const char*>(&tag[0]), tag.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
