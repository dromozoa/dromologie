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
    context<mbedtls_entropy_context, mbedtls_entropy_init, mbedtls_entropy_free> entropy;
    context<mbedtls_ctr_drbg_context, mbedtls_ctr_drbg_init, mbedtls_ctr_drbg_free> ctr_drbg;
    context<mbedtls_pk_context, mbedtls_pk_init, mbedtls_pk_free> pk;

    check(mbedtls_ctr_drbg_seed(ctr_drbg.get(), mbedtls_entropy_func, entropy.get(), nullptr, 0));
    check(mbedtls_pk_setup(pk.get(), mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY)));
    check(mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, mbedtls_pk_ec(*pk), mbedtls_ctr_drbg_random, ctr_drbg.get()));

    std::vector<unsigned char> buffer(4096);
    check(mbedtls_pk_write_key_pem(pk.get(), buffer.data(), buffer.size()));
    std::cout << reinterpret_cast<const char*>(buffer.data());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
