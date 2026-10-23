//
// Created by Merutilm on 10/24/26.
//

#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <gmp.h>
#include <immintrin.h>
#include <iosfwd>
#include <string>

namespace merutilm::rff2 {

    struct u256acc {
        static_assert(sizeof(uint64_t) == sizeof(unsigned long long));
        static_assert(GMP_LIMB_BITS == sizeof(unsigned long long) * 8);
        std::array<unsigned long long, 4> w;

        // no explicit
        u256acc() : w{0, 0, 0, 0} {}

        explicit u256acc(const std::string &s) : w{0, 0, 0, 0} {
            mpz_t z;
            mpz_init(z);
            if (mpz_set_str(z, s.data(), 10) == 0) {
                mpz_fdiv_r_2exp(z, z, 256);
                mpz_export(w.data(), nullptr, -1, sizeof(w[0]), 0, 0, z);
            }
            mpz_clear(z);
        }

        explicit u256acc(const uint64_t v) : w{v, 0, 0, 0} {}

        explicit operator uint64_t() const { return w[0]; }

        friend u256acc operator+(const u256acc &a, const uint64_t b) {
            u256acc r;
            unsigned char c = _addcarry_u64(0, a.w[0], b, &r.w[0]);
            c = _addcarry_u64(c, a.w[1], 0, &r.w[1]);
            c = _addcarry_u64(c, a.w[2], 0, &r.w[2]);
            _addcarry_u64(c, a.w[3], 0, &r.w[3]);
            return r;
        }


        friend u256acc operator+(const u256acc &a, const u256acc &b) {
            u256acc r;
            unsigned char c = _addcarry_u64(0, a.w[0], b.w[0], &r.w[0]);
            c = _addcarry_u64(c, a.w[1], b.w[1], &r.w[1]);
            c = _addcarry_u64(c, a.w[2], b.w[2], &r.w[2]);
            _addcarry_u64(c, a.w[3], b.w[3], &r.w[3]);
            return r;
        }

        friend u256acc &operator++(u256acc &v) {
            unsigned char c = _addcarry_u64(0, v.w[0], 1, &v.w[0]);
            c = _addcarry_u64(c, v.w[1], 0, &v.w[1]);
            c = _addcarry_u64(c, v.w[2], 0, &v.w[2]);
            _addcarry_u64(c, v.w[3], 0, &v.w[3]);
            return v;
        }

        friend std::ostream &operator<<(std::ostream &os, const u256acc &v) {
            mpz_t z;
            mpz_init(z);
            mpz_import(z, 4, -1, sizeof(v.w[0]), 0, 0, v.w.data());
            char *str = nullptr;
            gmp_asprintf(&str, "%Zd", z);
            mpz_clear(z);
            os << std::string(str);
            free(str);
            return os;
        }
    };


} // namespace merutilm::rff2
