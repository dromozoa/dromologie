#ifndef DROMOLOGIE_COMMON_HPP
#define DROMOLOGIE_COMMON_HPP

#include <mbedtls/build_info.h>
#include <mbedtls/error.h>

#include <cstddef>
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
}

#endif
