#ifndef DROMOLOGIE_COMMON_HPP
#define DROMOLOGIE_COMMON_HPP

#include <mbedtls/build_info.h>
#include <mbedtls/error.h>

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace dromologie {
  template <class T>
  inline T read_all(std::istream& stream) {
    T result;
    std::copy(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>(), std::back_inserter(result));
    return result;
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

  template <class T>
  inline std::string encode_base64url(const T& data) {
    return encode_base64url(data.data(), data.size());
  }

  inline std::vector<std::int32_t> make_base64_decoder() {
    std::vector<std::int32_t> result(256, -2);
    for (std::size_t i = 0; i < 26; ++i) {
      result['A' + i] = i;
    }
    for (std::size_t i = 26; i < 52; ++i) {
      result['a' + i - 26] = i;
    }
    for (std::size_t i = 52; i < 62; ++i) {
      result['0' + i - 52] = i;
    }

    result['+'] = 62;
    result['/'] = 63;

    // base64url
    result['-'] = 62;
    result['_'] = 63;

    result['\t'] = -1;
    result['\n'] = -1;
    result['\v'] = -1;
    result['\f'] = -1;
    result['\r'] = -1;
    result[' '] = -1;
    result['='] = -1;

    return result;
  }

  template <class T>
  inline T decode_base64(const std::string& source) {
    static const std::vector<std::int32_t> decoder = make_base64_decoder();

    std::vector<std::uint32_t> buffer;
    T result;
    for (char c : source) {
      std::int32_t v = decoder[static_cast<std::uint8_t>(c)];
      if (v == -1) {
        continue;
      } else if (v < -1) {
        throw std::runtime_error("non-encoding character found");
      }
      buffer.push_back(v);

      if (buffer.size() == 4) {
        std::uint32_t v = buffer[0] << 18 | buffer[1] << 12 | buffer[2] << 6 | buffer[3];
        result.push_back(v >> 16);
        result.push_back(v >> 8 & 0xFF);
        result.push_back(v & 0xFF);
        buffer.clear();
      }
    }

    switch (buffer.size()) {
      case 3:
        {
          std::uint32_t v = buffer[0] << 10 | buffer[1] << 4 | buffer[2] >> 2;
          result.push_back(v >> 8);
          result.push_back(v & 0xFF);
        }
        break;
      case 2:
        result.push_back(buffer[0] << 2 | buffer[1] >> 4);
        break;
      case 0:
        break;
      default:
        throw std::runtime_error("base64 stream truncated");
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

  template <class T, void (*T_init)(T*), void (*T_free)(T*)>
  class context {
  public:
    context() : context_() { T_init(&context_); }
    ~context() { T_free(&context_); }
    context(const context&) = delete;
    context& operator=(const context&) = delete;
    T* get() { return &context_; }
    T* operator->() { return &context_; }
    T& operator*() { return context_; }
  private:
    T context_;
  };

  inline std::vector<unsigned char> decode_hex(const std::string& source) {
    std::vector<unsigned char> result;
    for (std::size_t i = 0; i < source.size(); i += 2) {
      result.push_back(std::stoi(source.substr(i, 2), nullptr, 16));
    }
    return result;
  }

}

#endif
