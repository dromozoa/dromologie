#include "common.hpp"

#include <mbedtls/bignum.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/entropy.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>

#include <exception>
#include <iostream>
#include <vector>

using namespace dromologie;

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cerr << argv[0] << " hash <key.pem\n";
      return 1;
    }

    auto key = read_all<std::vector<unsigned char>>(std::cin);
    key.push_back(0);
    const auto hash = decode_base64<std::vector<unsigned char>>(argv[1]);

    context<mbedtls_entropy_context, mbedtls_entropy_init, mbedtls_entropy_free> entropy;
    context<mbedtls_ctr_drbg_context, mbedtls_ctr_drbg_init, mbedtls_ctr_drbg_free> ctr_drbg;
    context<mbedtls_pk_context, mbedtls_pk_init, mbedtls_pk_free> pk;
    context<mbedtls_ecdsa_context, mbedtls_ecdsa_init, mbedtls_ecdsa_free> ecdsa;
    context<mbedtls_mpi, mbedtls_mpi_init, mbedtls_mpi_free> r;
    context<mbedtls_mpi, mbedtls_mpi_init, mbedtls_mpi_free> s;

    check(mbedtls_ctr_drbg_seed(ctr_drbg.get(), mbedtls_entropy_func, entropy.get(), nullptr, 0));
    check(mbedtls_pk_parse_key(pk.get(), key.data(), key.size(), nullptr, 0, mbedtls_ctr_drbg_random, ctr_drbg.get()));
    const mbedtls_ecp_keypair* keypair = mbedtls_pk_ec(*pk);
    if (!keypair) {
      std::cerr << "ecp keypair not found\n";
      return 1;
    }
    check(mbedtls_ecdsa_from_keypair(ecdsa.get(), keypair));

    check(mbedtls_ecdsa_sign_det_ext(
        &ecdsa->MBEDTLS_PRIVATE(grp),
        r.get(),
        s.get(),
        &ecdsa->MBEDTLS_PRIVATE(d),
        &hash[0],
        hash.size(),
        MBEDTLS_MD_SHA256,
        mbedtls_ctr_drbg_random,
        ctr_drbg.get()));

    std::vector<unsigned char> buffer(32);
    check(mbedtls_mpi_write_binary(r.get(), &buffer[0], buffer.size()));
    std::cout.write(reinterpret_cast<const char*>(&buffer[0]), buffer.size());
    check(mbedtls_mpi_write_binary(s.get(), &buffer[0], buffer.size()));
    std::cout.write(reinterpret_cast<const char*>(&buffer[0]), buffer.size());

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
