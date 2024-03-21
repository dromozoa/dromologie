#include "common.hpp"

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/ecp.h>
#include <mbedtls/entropy.h>
#include <mbedtls/pk.h>

#include <cstddef>
#include <exception>
#include <iostream>
#include <vector>

using namespace dromologie;

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cerr << argv[0] << " pub <key.pem\n";
      return 1;
    }

    auto key = read_all<std::vector<unsigned char>>(std::cin);
    key.push_back(0);
    const auto pub = decode_base64<std::vector<unsigned char>>(argv[1]);

    std::vector<unsigned char> buffer(32);
    std::size_t size = 0;

    context<mbedtls_entropy_context, mbedtls_entropy_init, mbedtls_entropy_free> entropy;
    context<mbedtls_ctr_drbg_context, mbedtls_ctr_drbg_init, mbedtls_ctr_drbg_free> ctr_drbg;
    context<mbedtls_pk_context, mbedtls_pk_init, mbedtls_pk_free> pk;
    context<mbedtls_ecp_point, mbedtls_ecp_point_init, mbedtls_ecp_point_free> q;
    context<mbedtls_ecp_keypair, mbedtls_ecp_keypair_init, mbedtls_ecp_keypair_free> keypair;
    context<mbedtls_ecdh_context, mbedtls_ecdh_init, mbedtls_ecdh_free> ecdh;

    check(mbedtls_ctr_drbg_seed(ctr_drbg.get(), mbedtls_entropy_func, entropy.get(), nullptr, 0));
    check(mbedtls_pk_parse_key(pk.get(), key.data(), key.size(), nullptr, 0, mbedtls_ctr_drbg_random, ctr_drbg.get()));
    const mbedtls_ecp_keypair* our_keypair = mbedtls_pk_ec(*pk);
    if (!our_keypair) {
      std::cerr << "ecp keypair not found\n";
      return 1;
    }
    check(mbedtls_ecp_point_read_binary(&our_keypair->MBEDTLS_PRIVATE(grp), q.get(), pub.data(), pub.size()));
    check(mbedtls_ecp_group_copy(&keypair->MBEDTLS_PRIVATE(grp), &our_keypair->MBEDTLS_PRIVATE(grp)));
    check(mbedtls_ecp_copy(&keypair->MBEDTLS_PRIVATE(Q), q.get()));

    check(mbedtls_ecdh_setup(ecdh.get(), MBEDTLS_ECP_DP_SECP256R1));
    check(mbedtls_ecdh_get_params(ecdh.get(), our_keypair, MBEDTLS_ECDH_OURS));
    check(mbedtls_ecdh_get_params(ecdh.get(), keypair.get(), MBEDTLS_ECDH_THEIRS));

    check(mbedtls_ecdh_calc_secret(ecdh.get(), &size, buffer.data(), buffer.size(), mbedtls_ctr_drbg_random, ctr_drbg.get()));
    std::cout.write(reinterpret_cast<const char*>(buffer.data()), size);

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
