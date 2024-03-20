#ifndef DROMOLOGIE_COMMON_HPP
#define DROMOLOGIE_COMMON_HPP

#include <mbedtls/build_info.h>
#include <mbedtls/error.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace dromologie {
  inline std::vector<unsigned char> decode_hex(const std::string& source) {
    std::vector<unsigned char> result;
    for (std::size_t i = 0; i < source.size(); i += 2) {
      result.push_back(std::stoi(source.substr(i, 2), nullptr, 16));
    }
    return result;
  }

  inline void check(int result) {
    if (result != 0) {
      std::vector<char> buffer(256);
      mbedtls_strerror(result, &buffer[0], buffer.size());
      throw std::runtime_error(&buffer[0]);
    }
  }

  inline std::vector<char> make_base64url_encoder() {
    std::vector<char> result(64);
    for (std::size_t i = 0; i < 26; ++i) {
      result[i] = 'A' + i;
    }
    for (std::size_t i = 26; i < 52; ++i) {
      result[i] = 'a' + i - 26;
    }
    for (std::size_t i = 52; i < 62; ++i) {
      result[i] = '0' + i - 52;
    }
    result[62] = '-';
    result[63] = '_';
    return result;
  }

  inline std::string encode_base64url(const void* data, std::size_t size) {
    static const std::vector<char> encoder = make_base64url_encoder();

    const std::uint8_t* source = static_cast<const std::uint8_t*>(data);
    std::string result;
    for (std::size_t i = 0; i < size; i += 3) {
      if (i + 2 < size) {
        std::uint32_t v = source[i] << 16 | source[i + 1] << 8 | source[i + 2];
        result.push_back(encoder[v >> 18]);
        result.push_back(encoder[v >> 12 & 0x3F]);
        result.push_back(encoder[v >> 6 & 0x3F]);
        result.push_back(encoder[v & 0x3F]);
      } else if (i + 1 < size) {
        std::uint32_t v = source[i] << 10 | source[i + 1] << 2;
        result.push_back(encoder[v >> 12]);
        result.push_back(encoder[v >> 6 & 0x3F]);
        result.push_back(encoder[v & 0x3F]);
      } else {
        std::uint32_t v = source[i] << 4;
        result.push_back(encoder[v >> 6 & 0x3F]);
        result.push_back(encoder[v & 0x3F]);
      }
    }
    return result;
  }

  inline std::vector<std::int32_t> make_base64url_decoder() {
    std::vector<std::int32_t> result(256, -1);
    for (std::size_t i = 0; i < 26; ++i) {
      result['A' + i] = i;
    }
    for (std::size_t i = 26; i < 52; ++i) {
      result['a' + i - 26] = i;
    }
    for (std::size_t i = 52; i < 62; ++i) {
      result['0' + i - 52] = i;
    }
    result['-'] = 62;
    result['_'] = 63;
    return result;
  }

  inline std::uint32_t decode_base64url_impl(char c) {
    static const std::vector<std::int32_t> decoder = make_base64url_decoder();

    std::int32_t v = decoder[static_cast<std::uint8_t>(c)];
    if (v < 0) {
      throw std::runtime_error("invalid character found");
    }
    return v;
  }

  template <class T>
  inline std::vector<T> decode_base64url(const std::string& source) {
    std::vector<T> result;
    for (std::size_t i = 0; i < source.size(); ++i) {
      if (i + 3 < source.size()) {
        std::uint32_t v
          = decode_base64url_impl(source[i]) << 18
          | decode_base64url_impl(source[i + 1]) << 12
          | decode_base64url_impl(source[i + 2]) << 6
          | decode_base64url_impl(source[i + 3]);
        result.push_back(v >> 16);
        result.push_back(v >> 8 & 0xFF);
        result.push_back(v & 0xFF);
      } else if (i + 2 < source.size()) {
        std::uint32_t v
          = decode_base64url_impl(source[i]) << 10
          | decode_base64url_impl(source[i + 1]) << 4
          | decode_base64url_impl(source[i + 2]) >> 2;
        result.push_back(v >> 8);
        result.push_back(v & 0xFF);
      } else {
        std::uint32_t v
          = decode_base64url_impl(source[i]) << 2
          | decode_base64url_impl(source.at(i + 1)) >> 4;
        result.push_back(v);
      }
    }
    return result;
  }
}

#endif
