//
// Created by Merutilm on 2026-04-25.
//
#pragma once
#include <cmath>
#include <gmp.h>
#include <iostream>

#include "exponent.hpp"


namespace merutilm::rff2 {
    /**
     * fast fixed point arbitrary-precision decimal.
     * the size of mp_limb must be 8. other case is undefined.
     */
    struct fixed_point_decimal {

        mpz_t data;
        /**
         * must be negative
         */
        mp_size_t exp2div64;

        explicit fixed_point_decimal() : fixed_point_decimal(0.0, 0) {}

        explicit fixed_point_decimal(double v, int dec_exp10);

        explicit fixed_point_decimal(const std::string &str, int dec_exp10);

        template<Number Exp, Number Mantissa, Number Bit>
        explicit fixed_point_decimal(exponent<Exp, Mantissa, Bit> v, int dec_exp10);

        ~fixed_point_decimal();

        fixed_point_decimal(const fixed_point_decimal &);

        fixed_point_decimal &operator=(const fixed_point_decimal &);

        fixed_point_decimal(fixed_point_decimal &&) noexcept;

        fixed_point_decimal &operator=(fixed_point_decimal &&) noexcept;

        template<typename F>
            requires std::is_invocable_r_v<int, F, mpf_t, int>
        void init_data(int dec_exp10, F &&setter_exp2_getter);

        static int exp10_to_exp2div64(int exp10);

        static void add(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        static void sub(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        static void sqr(fixed_point_decimal &result, const fixed_point_decimal &v);


        /**
         * Fast-multiplication. decimal limbs count must be the same for all, and limbs length must be result ==
         * lhs >= rhs. If an overflow occurs, the most significant limbs are discarded. [CAUTION] in-place operation is
         * not supported.
         * @param result the reference of result.
         * @param lhs left operand
         * @param rhs right operand
         */
        static void mul(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        static void div(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        static void dbl(fixed_point_decimal &result, const fixed_point_decimal &v);

        static void hlv(fixed_point_decimal &result, const fixed_point_decimal &v);

        static void neg(fixed_point_decimal &v);

        void set_exp10(int dec_exp10);

        explicit operator float() const;

        explicit operator double() const;

        template<Number Exp, Number Mantissa, Number Bit>
        explicit operator exponent<Exp, Mantissa, Bit>() const;

        std::string to_string() const;

        void export_value(uint64_t &mantissa_bit, mp_size_t &f_exp2) const;
    };


    inline fixed_point_decimal::fixed_point_decimal(double v, const int dec_exp10) {
        init_data(dec_exp10, [v](mpf_t val, const int exp2div64) {
            mpf_set_d(val, v);
            return exp2div64 * 64;
        });
    }


    inline fixed_point_decimal::fixed_point_decimal(const std::string &str, const int dec_exp10) {
        init_data(dec_exp10, [str](mpf_t val, const int exp2div64) {
            mpf_set_str(val, str.data(), 10);
            return exp2div64 * 64;
        });
    }

    template<Number Exp, Number Mantissa, Number Bit>
    fixed_point_decimal::fixed_point_decimal(const exponent<Exp, Mantissa, Bit> v, const int dec_exp10) {
        init_data(dec_exp10, [v](mpf_t val, const int exp2div64) {
            mpf_set_d(val, v.get_mantissa());
            return exp2div64 * 64 - v.get_exp2();
        });
    }


    inline fixed_point_decimal::~fixed_point_decimal() { mpz_clear(this->data); }


    inline fixed_point_decimal::fixed_point_decimal(const fixed_point_decimal &other) : exp2div64(other.exp2div64) {
        mpz_init(this->data);
        mpz_set(this->data, other.data);
    }


    inline fixed_point_decimal &fixed_point_decimal::operator=(const fixed_point_decimal &other) {
        if (&other == this)
            return *this;

        this->exp2div64 = other.exp2div64;
        mpz_set(this->data, other.data);
        return *this;
    }


    inline fixed_point_decimal::fixed_point_decimal(fixed_point_decimal &&other) noexcept : exp2div64(other.exp2div64) {
        mpz_init(this->data);
        mpz_swap(this->data, other.data);
    }


    inline fixed_point_decimal &fixed_point_decimal::operator=(fixed_point_decimal &&other) noexcept {
        if (&other == this)
            return *this;

        mpz_swap(this->data, other.data);
        this->exp2div64 = other.exp2div64;
        return *this;
    }


    template<typename F>
        requires std::is_invocable_r_v<int, F, mpf_t, int>
    void fixed_point_decimal::init_data(const int dec_exp10, F &&setter_exp2_getter) {
        mpz_init(this->data);
        exp2div64 = exp10_to_exp2div64(dec_exp10);
        mpf_t val;

        mpf_init2(val, -exp2div64 * 64);
        const int exp2 = setter_exp2_getter(val, exp2div64);

        if (exp2 < 0) {
            mpf_mul_2exp(val, val, -exp2);
        } else if (exp2 > 0) {
            mpf_div_2exp(val, val, exp2);
        }


        mpz_set_f(data, val);
        mpf_clear(val);
    }


    inline int fixed_point_decimal::exp10_to_exp2div64(const int exp10) {
        constexpr double log10_2 = 0.301029995663981;
        auto exp2div64 = static_cast<int>(static_cast<double>(exp10) / log10_2);
        exp2div64 = (exp2div64 - 63) / 64;
        return exp2div64;
    }


    inline void fixed_point_decimal::add(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        assert(result.exp2div64 == lhs.exp2div64);
        assert(result.exp2div64 == rhs.exp2div64);
        mpz_add(result.data, lhs.data, rhs.data);
    }


    inline void fixed_point_decimal::sub(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        assert(result.exp2div64 == lhs.exp2div64);
        assert(result.exp2div64 == rhs.exp2div64);
        mpz_sub(result.data, lhs.data, rhs.data);
    }


    inline void fixed_point_decimal::sqr(fixed_point_decimal &result, const fixed_point_decimal &v) {
        assert(result.exp2div64 == v.exp2div64);

        /*
           This function contains modified gmp source code under mpz_mul.

           mpz_mul -- Multiply two integers.

           Copyright 1991, 1993, 1994, 1996, 2000, 2001, 2005, 2009, 2011, 2012,
           2015 Free Software Foundation, Inc.

           This file is part of the GNU MP Library.

           The GNU MP Library is free software; you can redistribute it and/or modify
           it under the terms of either:

           * the GNU Lesser General Public License as published by the Free
             Software Foundation; either version 3 of the License, or (at your
            option) any later version.

           or

            * the GNU General Public License as published by the Free Software
             Foundation; either version 2 of the License, or (at your option) any
             later version.

           or both in parallel, as here.

           The GNU MP Library is distributed in the hope that it will be useful, but
           WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
           or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
           for more details.

           You should have received copies of the GNU General Public License and the
           GNU Lesser General Public License along with the GNU MP Library.  If not,
           see https://www.gnu.org/licenses/.
           */

        const mpz_srcptr l = v.data;
        mp_size_t size = l->_mp_size;
        size = std::abs(size);
        mp_size_t result_size = size * 2;

        if (size == 0) {
            mpz_set_ui(result.data, 0);
            return;
        }

        const mp_ptr ptr = l->_mp_d;

        if (result.data->_mp_alloc < result_size) {
            mpz_realloc2(result.data, result_size * 64);
        }
        const mp_ptr result_ptr = result.data->_mp_d;

        mpn_sqr(result_ptr, ptr, size);
        const mp_limb_t top = result_ptr[result_size - 1];
        result_size -= top == 0;

        mpn_copyi(result_ptr, result_ptr - result.exp2div64, result_size + result.exp2div64);
        result_size += result.exp2div64;
        result.data->_mp_size = result_size;
    }


    inline void fixed_point_decimal::mul(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        assert(result.exp2div64 == lhs.exp2div64);
        assert(result.exp2div64 == rhs.exp2div64);

        /*
         *  This function contains modified gmp source code under mpz_mul.

            mpz_mul -- Multiply two integers.

            Copyright 1991, 1993, 1994, 1996, 2000, 2001, 2005, 2009, 2011, 2012,
            2015 Free Software Foundation, Inc.

            This file is part of the GNU MP Library.

            The GNU MP Library is free software; you can redistribute it and/or modify
            it under the terms of either:

            * the GNU Lesser General Public License as published by the Free
              Software Foundation; either version 3 of the License, or (at your
             option) any later version.

            or

             * the GNU General Public License as published by the Free Software
              Foundation; either version 2 of the License, or (at your option) any
              later version.

            or both in parallel, as here.

            The GNU MP Library is distributed in the hope that it will be useful, but
            WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
            or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
            for more details.

            You should have received copies of the GNU General Public License and the
            GNU Lesser General Public License along with the GNU MP Library.  If not,
            see https://www.gnu.org/licenses/.
            */

        mpz_srcptr l = lhs.data;
        mpz_srcptr r = rhs.data;
        mp_size_t lhs_size = mpz_size(l);
        mp_size_t rhs_size = mpz_size(r);
        const mp_size_t sgn = lhs_size ^ rhs_size;
        lhs_size = std::abs(lhs_size);
        rhs_size = std::abs(rhs_size);
        mp_size_t result_size = lhs_size + rhs_size;

        if (lhs_size < rhs_size) {
            std::swap(l, r);
            std::swap(lhs_size, rhs_size);
        }

        if (rhs_size == 0) {
            mpz_set_ui(result.data, 0);
            return;
        }

        const mp_ptr lhs_ptr = l->_mp_d;
        const mp_ptr rhs_ptr = r->_mp_d;

        if (result.data->_mp_alloc < result_size) {
            mpz_realloc2(result.data, result_size * 64);
        }
        const mp_ptr result_ptr = result.data->_mp_d;

        const mp_limb_t top = mpn_mul(result_ptr, lhs_ptr, lhs_size, rhs_ptr, rhs_size);
        result_size -= top == 0;

        mpn_copyi(result_ptr, result_ptr - result.exp2div64, result_size + result.exp2div64);
        result_size += result.exp2div64;
        result_size = sgn < 0 ? -result_size : result_size;
        result.data->_mp_size = result_size;
    }


    inline void fixed_point_decimal::div(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        assert(result.exp2div64 == lhs.exp2div64);
        assert(result.exp2div64 == rhs.exp2div64);
        mpz_mul_2exp(result.data, lhs.data, -lhs.exp2div64 * 64);
        mpz_div(result.data, result.data, rhs.data);
    }


    inline void fixed_point_decimal::dbl(fixed_point_decimal &result, const fixed_point_decimal &v) {
        mpz_mul_2exp(result.data, v.data, 1);
    }


    inline void fixed_point_decimal::hlv(fixed_point_decimal &result, const fixed_point_decimal &v) {
        mpz_div_2exp(result.data, v.data, 1);
    }

    inline void fixed_point_decimal::neg(fixed_point_decimal &v) { mpz_neg(v.data, v.data); }

    inline void fixed_point_decimal::set_exp10(const int dec_exp10) {
        const int new_exp2div64 = exp10_to_exp2div64(dec_exp10);
        if (exp2div64 < new_exp2div64) {
            mpz_div_2exp(data, data, (new_exp2div64 - exp2div64) * 64);
        } else if (exp2div64 > new_exp2div64) {
            mpz_mul_2exp(data, data, (exp2div64 - new_exp2div64) * 64);
        }
        exp2div64 = new_exp2div64;
    }


    inline fixed_point_decimal::operator float() const { return static_cast<float>(operator double()); }

    inline fixed_point_decimal::operator double() const {
        if (mpz_sgn(data) == 0) {
            return 0;
        }

        uint64_t mantissa_bit;
        mp_size_t f_exp2;

        export_value(mantissa_bit, f_exp2);
        // 0100 0000 0000 : 2^1
        // 0000 0000 0000 : 2^-1023
        // 0111 1111 1111 : 2^1024
#ifndef __FINITE_MATH_ONLY__
        if (f_exp2 > 0x03ff) {
            return sgn == 1 ? INFINITY : -static_cast<double>(INFINITY);
        }
#endif
#ifdef SAFE_EXP_OPERATOR
        const int mantissa_shift = f_exp2 <= -0x03ff ? -0x03ff - f_exp2 + 1 : 0;
        mantissa_bit = mantissa_bit >> mantissa_shift;
        const uint64_t exponent = f_exp2 <= -0x03ff ? 0 : 0x3ff0000000000000ULL + (static_cast<uint64_t>(f_exp2) << 52);
#else
        const uint64_t exponent = 0x3ff0000000000000ULL + (static_cast<uint64_t>(f_exp2) << 52);
#endif
        const uint64_t sig = mpz_sgn(data) == 1 ? 0 : 0x8000000000000000ULL;
        return std::bit_cast<double>(sig | exponent | mantissa_bit);
    }

    template<Number Exp, Number Mantissa, Number Bit>
    fixed_point_decimal::operator exponent<Exp, Mantissa, Bit>() const {
        if (mpz_sgn(data) == 0) {
            return exponent<Exp, Mantissa, Bit>::ZERO;
        }
        uint64_t mantissa_bit;
        mp_size_t f_exp2;
        export_value(mantissa_bit, f_exp2);

        const auto mantissa = std::bit_cast<double>(0x3ff0000000000000ULL | mantissa_bit);

        return exponent<Exp, Mantissa, Bit>(mpz_sgn(data)) *
               exponent<Exp, Mantissa, Bit>::mul_2exp(exponent<Exp, Mantissa, Bit>(static_cast<Mantissa>(mantissa)),
                                                      static_cast<int>(f_exp2));
    }

    inline std::string fixed_point_decimal::to_string() const {
        mpf_t d;
        mpf_init2(d, -exp2div64 * 64);
        mpf_set_z(d, data);
        mpf_div_2exp(d, d, -exp2div64 * 64);

        char *str;
        gmp_asprintf(&str, "%.Ff", d);
        std::string result(str);

        // gmp_asprinf uses malloc(), Do not remove this
        free(str);
        mpf_clear(d);
        return result;
    }


    inline void fixed_point_decimal::export_value(uint64_t &mantissa_bit, mp_size_t &f_exp2) const {

        static constexpr auto MANTISSA_MASK = 0x000fffffffffffffULL;
        const mp_limb_t *src_ptr = data->_mp_d;
        const mp_size_t nlc = std::abs(data->_mp_size);

        assert(nlc > 0);

        const mp_limb_t top = *(src_ptr + nlc - 1);
        const size_t len = nlc * 64 - std::countl_zero(top);

        const int32_t shift = static_cast<int32_t>(len) - 53;
        if (shift <= 0) {
            assert(shift > -53);
            mantissa_bit = *src_ptr << -shift & MANTISSA_MASK;
        } else {
            const mp_size_t limb_skip = shift / 64;
            const mp_size_t shift_small = shift - limb_skip * 64;
            const auto dst0 = src_ptr + limb_skip;
            if (shift_small <= 12) {
                mantissa_bit = *dst0 >> shift_small & MANTISSA_MASK;
            } else {
                const auto dst1 = dst0 + 1;
                mantissa_bit = (*dst1 << (64 - shift_small) | *dst0 >> shift_small) & MANTISSA_MASK;
            }
        }
        f_exp2 = exp2div64 * 64 + shift + 52;
    }
} // namespace merutilm::rff2
