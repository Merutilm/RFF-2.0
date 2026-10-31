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

#include "rff_constraints.hpp"

namespace merutilm::rff2 {

    struct uint256 {
        static_assert(sizeof(uint64_t) == sizeof(unsigned long long));
        static_assert(GMP_LIMB_BITS == sizeof(unsigned long long) * 8);
        std::array<unsigned long long, 4> w;

        // no explicit
        uint256() : w{0, 0, 0, 0} {}

        explicit uint256(const uint64_t v) : w{v, 0, 0, 0} {}

        explicit operator uint64_t() const { return w[0]; }

        friend uint256 operator+(const uint256 &a, const uint64_t b) {
            uint256 r;
            unsigned char c = _addcarry_u64(0, a.w[0], b, &r.w[0]);
            c = _addcarry_u64(c, a.w[1], 0, &r.w[1]);
            c = _addcarry_u64(c, a.w[2], 0, &r.w[2]);
            _addcarry_u64(c, a.w[3], 0, &r.w[3]);
            return r;
        }


        friend uint256 operator+(const uint256 &a, const uint256 &b) {
            uint256 r;
            unsigned char c = _addcarry_u64(0, a.w[0], b.w[0], &r.w[0]);
            c = _addcarry_u64(c, a.w[1], b.w[1], &r.w[1]);
            c = _addcarry_u64(c, a.w[2], b.w[2], &r.w[2]);
            _addcarry_u64(c, a.w[3], b.w[3], &r.w[3]);
            return r;
        }


        friend uint256 operator-(const uint256 &a, const uint256 &b) {
            uint256 r;
            unsigned char c = _subborrow_u64(0, a.w[0], b.w[0], &r.w[0]);
            c = _subborrow_u64(c, a.w[1], b.w[1], &r.w[1]);
            c = _subborrow_u64(c, a.w[2], b.w[2], &r.w[2]);
            c = _subborrow_u64(c, a.w[3], b.w[3], &r.w[3]);
            return r;
        }

        friend uint256 &operator--(uint256 &v) {
            unsigned char c = _subborrow_u64(0, v.w[0], 0, &v.w[0]);
            c = _subborrow_u64(c, v.w[1], 0, &v.w[1]);
            c = _subborrow_u64(c, v.w[2], 0, &v.w[2]);
            c = _subborrow_u64(c, v.w[3], 0, &v.w[3]);
            return v;
        }

        friend uint256 &operator++(uint256 &v) {
            unsigned char c = _addcarry_u64(0, v.w[0], 1, &v.w[0]);
            c = _addcarry_u64(c, v.w[1], 0, &v.w[1]);
            c = _addcarry_u64(c, v.w[2], 0, &v.w[2]);
            _addcarry_u64(c, v.w[3], 0, &v.w[3]);
            return v;
        }

        friend uint256 &operator--(uint256 &v, uint256 &v2) {
            v = v2;
            unsigned char c = _subborrow_u64(0, v2.w[0], 0, &v2.w[0]);
            c = _subborrow_u64(c, v2.w[1], 0, &v2.w[1]);
            c = _subborrow_u64(c, v2.w[2], 0, &v2.w[2]);
            c = _subborrow_u64(c, v2.w[3], 0, &v2.w[3]);
            return v;
        }

        friend uint256 &operator++(uint256 &v, uint256 &v2) {
            v = v2;
            unsigned char c = _addcarry_u64(0, v2.w[0], 1, &v2.w[0]);
            c = _addcarry_u64(c, v2.w[1], 0, &v2.w[1]);
            c = _addcarry_u64(c, v2.w[2], 0, &v2.w[2]);
            _addcarry_u64(c, v2.w[3], 0, &v2.w[3]);
            return v;
        }


        friend uint256 operator<<(const uint256 &v, const uint64_t shift) {
            std::array<unsigned long long, 8> nw{};
            uint256 r;

            const uint64_t limb_shift = shift >> 6;
            const uint64_t actual_shift = shift & 63;

            RFF_ASSERT(limb_shift < 4);

            for (uint64_t i = 4; i > 0; --i) {
                nw[i + 3] = v.w[i - 1];
            }
            for (uint64_t i = 4; i > 0; --i) {
                nw[i + 3] = nw[i + 3 - limb_shift];
            }
            for (uint64_t i = 4; i > 0; --i) {
                r.w[i - 1] = (nw[i + 2] >> ((64 - actual_shift) & 63)) | (nw[i + 3] << actual_shift);
            }

            return r;
        }

        friend uint256 operator>>(const uint256 &v, const uint64_t shift) {
            std::array<unsigned long long, 8> nw{};
            uint256 r;

            const uint64_t limb_shift = shift >> 6;
            const uint64_t actual_shift = shift & 63;

            RFF_ASSERT(limb_shift < 4);

            for (uint64_t i = 0; i < 4; ++i) {
                nw[i + 3 - limb_shift] = v.w[i];
            }
            for (uint64_t i = 0; i < 4; ++i) {
                r.w[i] = (nw[i + 4] << ((64 - actual_shift) & 63)) | (nw[i + 3] >> actual_shift);
            }

            return r;
        }

        friend std::ostream &operator<<(std::ostream &os, const uint256 &v) {
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
