#include "common.hpp"

using namespace dromologie;

#include <mbedtls/bignum.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdsa.h>
#include <mbedtls/entropy.h>
#include <mbedtls/md.h>
#include <mbedtls/pk.h>

#include <exception>
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char* argv[]) {
  try {
    if (argc < 3) {
      std::cerr << argv[0] << " key.pem hash\n";
      return 1;
    }

    const std::vector<unsigned char> hash = decode_hex(argv[2]);

    mbedtls_entropy_context entropy_context;
    std::unique_ptr<mbedtls_entropy_context, decltype(&mbedtls_entropy_free)> entropy(&entropy_context, mbedtls_entropy_free);
    mbedtls_entropy_init(entropy.get());

    mbedtls_ctr_drbg_context ctr_drbg_context;
    std::unique_ptr<mbedtls_ctr_drbg_context, decltype(&mbedtls_ctr_drbg_free)> ctr_drbg(&ctr_drbg_context, mbedtls_ctr_drbg_free);
    mbedtls_ctr_drbg_init(ctr_drbg.get());

    mbedtls_pk_context pk_context;
    std::unique_ptr<mbedtls_pk_context, decltype(&mbedtls_pk_free)> pk(&pk_context, mbedtls_pk_free);
    mbedtls_pk_init(pk.get());

    mbedtls_ecdsa_context ecdsa_context;
    std::unique_ptr<mbedtls_ecdsa_context, decltype(&mbedtls_ecdsa_free)> ecdsa(&ecdsa_context, mbedtls_ecdsa_free);
    mbedtls_ecdsa_init(ecdsa.get());

    mbedtls_mpi r_;
    std::unique_ptr<mbedtls_mpi, decltype(&mbedtls_mpi_free)> r(&r_, mbedtls_mpi_free);
    mbedtls_mpi_init(r.get());

    mbedtls_mpi s_;
    std::unique_ptr<mbedtls_mpi, decltype(&mbedtls_mpi_free)> s(&s_, mbedtls_mpi_free);
    mbedtls_mpi_init(s.get());

    check(mbedtls_ctr_drbg_seed(ctr_drbg.get(), mbedtls_entropy_func, entropy.get(), nullptr, 0));
    check(mbedtls_pk_parse_keyfile(pk.get(), argv[1], nullptr, mbedtls_ctr_drbg_random, ctr_drbg.get()));
    const mbedtls_ecp_keypair* keypair = mbedtls_pk_ec(*pk);
    if (!keypair) {
      std::cerr << "ecp keypair not found\n";
      return 1;
    }
    check(mbedtls_ecdsa_from_keypair(ecdsa.get(), keypair));

    // ecdsa.cを参考にする
    // mbedtls_ecdsa_write_signature
    // mbedtls_ecdsa_write_signature_restartable
    // mbedtls_ecdsa_sign_det_restartable

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

    std::vector<char> buffer(256);
    size_t size1 = 0;
    check(mbedtls_mpi_write_string(r.get(), 16, &buffer[0], buffer.size(), &size1));
    size_t size2 = 0;
    check(mbedtls_mpi_write_string(s.get(), 16, &buffer[0] + size1 - 1, buffer.size() - size1 + 1, &size2));
    std::cout << &buffer[0] << "\n";

  } catch (const std::exception& e) {
    std::cerr << "caught exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
