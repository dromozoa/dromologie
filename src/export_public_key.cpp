#include "common.hpp"

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecp.h>
#include <mbedtls/entropy.h>
#include <mbedtls/pk.h>

#include <exception>
#include <iostream>
#include <vector>

using namespace dromologie;

int main(int, char*[]) {
  try {
    auto key = read_all<std::vector<unsigned char>>(std::cin);
    key.push_back(0);

    context<mbedtls_entropy_context, mbedtls_entropy_init, mbedtls_entropy_free> entropy;
    context<mbedtls_ctr_drbg_context, mbedtls_ctr_drbg_init, mbedtls_ctr_drbg_free> ctr_drbg;
    context<mbedtls_pk_context, mbedtls_pk_init, mbedtls_pk_free> pk;
    check(mbedtls_ctr_drbg_seed(ctr_drbg.get(), mbedtls_entropy_func, entropy.get(), nullptr, 0));
    check(mbedtls_pk_parse_key(pk.get(), key.data(), key.size(), nullptr, 0, mbedtls_ctr_drbg_random, ctr_drbg.get()));
    const mbedtls_ecp_keypair* keypair = mbedtls_pk_ec(*pk);
    if (!keypair) {
      std::cerr << "ecp keypair not found\n";
      return 1;
    }

    std::vector<unsigned char> buffer(128);
    std::size_t size = 0;
    check(mbedtls_ecp_point_write_binary(
        &keypair->MBEDTLS_PRIVATE(grp),
        &keypair->MBEDTLS_PRIVATE(Q),
        MBEDTLS_ECP_PF_UNCOMPRESSED,
        &size,
        buffer.data(),
        buffer.size()));
    std::cout.write(reinterpret_cast<const char*>(buffer.data()), size);

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
