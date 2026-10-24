//
// Created by Merutilm on 10/24/26.
//

#pragma once
#include <cassert>
#include <cstdint>
#include <string>
#include <gmp.h>
#include <array>

namespace merutilm::rff2::itt {
    using uint128_t = __uint128_t;
    using ref = uint64_t;
    using ptb = uint128_t;


    inline std::string u128tostr(const uint128_t value) {
        if (value == 0)
            return "0";

        const std::array limbs{static_cast<uint64_t>(value), static_cast<uint64_t>(value >> 64)};
        if (limbs[1] == 0) {
            return std::to_string(limbs[0]);
        }

        mpz_t z;
        mpz_init(z);
        mpz_import(z, 2, -1, sizeof(uint64_t), 0, 0, limbs.data());
        char *str;
        int len = gmp_asprintf(&str, "%Zd", z);
        mpz_clear(z);
        assert(len >= 0);
        std::string result = str;
        free(str);
        return result;
    }

    // std::string -> uint128_t
    inline uint128_t strtou128(const std::string &str) {
        mpz_t z;
        mpz_init(z);
        int success = mpz_set_str(z, str.data(), 10);
        assert(success == 0);
        mpz_mod_2exp(z, z, 128);
        std::array<uint64_t, 2> limbs{};
        mpz_export(limbs.data(), nullptr, -1, sizeof(uint64_t), 0, 0, z);
        mpz_clear(z);
        const uint128_t result = limbs[0] + (static_cast<uint128_t>(limbs[1]) << 64);
        return result;
    }
} // namespace merutilm::rff2::itt
