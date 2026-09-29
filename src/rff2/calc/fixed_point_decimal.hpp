//
// Created by Merutilm on 2026-04-25.
//
#pragma once
#include <cmath>
#include <gmp.h>

#include "exponent.hpp"
namespace merutilm::rff2 {

    /**
     * fast fixed point arbitrary-precision decimal.
     * stores data * 2^(64*exp2div64).
     * the size of mp_limb must be 8. other case is undefined.
     *
     * The precision of the integer part is guaranteed up to 2^64 - 1 only at initialization.
     * Behavior for larger integer is undefined (precision loss). For the sake of fast computation,
     * many implementations assume that a decimal part is always exist.
     * Therefore, <code>exp10</code> must be <code>negative</code>. However, when these objects are used in calculations
     * with one another, the precision of the integer part is guaranteed.
     */
    struct fixed_point_decimal {
        static_assert(GMP_NUMB_BITS == 64);

        mp_limb_t *data = nullptr;
        int64_t size = 0;
        uint64_t alloc = 0;

        /**
         * must be negative
         */
        int64_t exp2div64 = 0;

        fixed_point_decimal() : fixed_point_decimal(0.0, -1) {}

        explicit fixed_point_decimal(double v, int64_t exp10);

        explicit fixed_point_decimal(const std::string &str, int64_t exp10);

        template<Number Exp, Number Mantissa, Number Bit>
        explicit fixed_point_decimal(exponent<Exp, Mantissa, Bit> v, int64_t exp10);

        ~fixed_point_decimal();

        fixed_point_decimal(const fixed_point_decimal &);

        fixed_point_decimal &operator=(const fixed_point_decimal &);

        fixed_point_decimal(fixed_point_decimal &&) noexcept;

        fixed_point_decimal &operator=(fixed_point_decimal &&) noexcept;

        bool try_realloc_inc(uint64_t new_limbs_alloc, bool preserveValue = true);

        template<typename F>
            requires std::is_invocable_r_v<int, F, mpf_t, int>
        void init_data(int64_t exp10, F &&setter_exp2_getter);

        static int64_t exp10_to_exp2div64(int64_t exp10);

        /**
         * Adds 1 to current instance.
         */
        void add_one();

        static void add(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        void normalize_size(int64_t known_size);

        static void sub(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        void zero();
        void one();
        int32_t sgn() const;
        int32_t compare_abs(uint64_t value) const;
        int32_t compare(int64_t value) const;
        void set(int64_t value, bool make_decimal_zero);

        /**
         * Fast-square.
         * [CAUTION] in-place operation is not supported.
         * @param result the reference of result.
         * @param v operand
         */
        static void sqr(fixed_point_decimal &result, const fixed_point_decimal &v);


        static void mul(fixed_point_decimal &result, const fixed_point_decimal &lhs, uint64_t rhs);
        /**
         * Fast-multiplication.
         * [CAUTION] in-place operation is not supported.
         * @param result the reference of result.
         * @param lhs left operand
         * @param rhs right operand
         */
        static void mul(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        /**
         * Fast-division.
         * [CAUTION] in-place operation is not supported.
         * @param result the reference of result.
         * @param lhs left operand
         * @param rhs right operand
         */
        static void div(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        static void limbs_lshift(fixed_point_decimal &result, fixed_point_decimal const &v, int64_t limb_shift);

        static void limbs_rshift(fixed_point_decimal &result, fixed_point_decimal const &v, int64_t limb_shift);

        static void dbl(fixed_point_decimal &result, const fixed_point_decimal &v);

        static void hlv(fixed_point_decimal &result, const fixed_point_decimal &v);

        void neg();

        void set_exp10(int64_t exp10, bool preserveValue = true);
        void set_exp2div64(int64_t new_exp2div64, bool preserveValue);

        explicit operator float() const;

        explicit operator double() const;

        template<Number Exp, Number Mantissa, Number Bit>
        explicit operator exponent<Exp, Mantissa, Bit>() const;

        [[nodiscard]] std::string to_string() const;

        void export_value(uint64_t &mantissa_bit, mp_size_t &f_exp2) const;
    };


    inline fixed_point_decimal::fixed_point_decimal(double v, const int64_t exp10) {
        init_data(exp10, [v](mpf_t val, const int64_t exp2div64) {
            mpf_set_d(val, v);
            return exp2div64 * 64;
        });
    }


    inline fixed_point_decimal::fixed_point_decimal(const std::string &str, const int64_t exp10) {
        init_data(exp10, [str](mpf_t val, const int64_t exp2div64) {
            mpf_set_str(val, str.data(), 10);
            return exp2div64 * 64;
        });
    }

    template<Number Exp, Number Mantissa, Number Bit>
    fixed_point_decimal::fixed_point_decimal(const exponent<Exp, Mantissa, Bit> v, const int64_t exp10) {
        init_data(exp10, [v](mpf_t val, const int64_t exp2div64) {
            mpf_set_d(val, v.get_mantissa());
            return exp2div64 * 64 - v.get_exp2();
        });
    }


    inline fixed_point_decimal::~fixed_point_decimal() { delete[] data; }


    inline fixed_point_decimal::fixed_point_decimal(const fixed_point_decimal &other) :
        data(new mp_limb_t[other.alloc]), size(other.size), alloc(other.alloc), exp2div64(other.exp2div64) {
        memcpy(data, other.data, std::abs(other.size) * sizeof(mp_limb_t));
    }


    inline fixed_point_decimal &fixed_point_decimal::operator=(const fixed_point_decimal &other) {
        if (&other == this)
            return *this;

        if (this->alloc < other.alloc) {
            delete[] data;
            data = new mp_limb_t[other.alloc];
            alloc = other.alloc;
        }
        size = other.size;
        exp2div64 = other.exp2div64;
        memcpy(data, other.data, std::abs(other.size) * sizeof(mp_limb_t));
        return *this;
    }


    inline fixed_point_decimal::fixed_point_decimal(fixed_point_decimal &&other) noexcept :
        size(other.size), alloc(other.alloc), exp2div64(other.exp2div64) {
        std::swap(data, other.data);
    }


    inline fixed_point_decimal &fixed_point_decimal::operator=(fixed_point_decimal &&other) noexcept {
        if (&other == this)
            return *this;

        size = other.size;
        alloc = other.alloc;
        exp2div64 = other.exp2div64;
        std::swap(data, other.data);
        return *this;
    }


    inline bool fixed_point_decimal::try_realloc_inc(const uint64_t new_limbs_alloc, const bool preserveValue) {
        if (alloc >= new_limbs_alloc)
            return false;
        const auto temp = new mp_limb_t[new_limbs_alloc];
        alloc = new_limbs_alloc;
        if (preserveValue)
            memcpy(temp, data, std::abs(size) * sizeof(mp_limb_t));
        delete[] data;
        data = temp;
        return true;
    }

    template<typename F>
        requires std::is_invocable_r_v<int, F, mpf_t, int>
    void fixed_point_decimal::init_data(const int64_t exp10, F &&setter_exp2_getter) {

        mpz_t temp;
        mpz_init(temp);
        exp2div64 = exp10_to_exp2div64(exp10);
        mpf_t val;

        mpf_init2(val, (1 - exp2div64) * 64);
        const int64_t exp2 = setter_exp2_getter(val, exp2div64);

        if (exp2 < 0) {
            mpf_mul_2exp(val, val, -exp2);
        } else if (exp2 > 0) {
            mpf_div_2exp(val, val, exp2);
        }


        mpz_set_f(temp, val);

        const mp_limb_t *lmb1 = mpz_limbs_read(temp);

        size = static_cast<int64_t>(mpz_size(temp));
        alloc = size;
        data = new mp_limb_t[alloc];
        mpn_copyi(data, lmb1, size);
        size *= mpz_sgn(temp);

        mpf_clear(val);
        mpz_clear(temp);
    }


    inline int64_t fixed_point_decimal::exp10_to_exp2div64(const int64_t exp10) {
        constexpr double log10_2 = 0.301029995663981;
        auto exp2div64 = static_cast<int>(static_cast<double>(exp10) / log10_2);
        exp2div64 = (exp2div64 - 63) / 64;
        return exp2div64;
    }


    inline void fixed_point_decimal::add_one() {
        const int64_t dec_limbs = -exp2div64;
        const int64_t required_minimum = dec_limbs + 1;
        const bool neg = size < 0;
        const int64_t limbs_cnt = std::abs(size);


        if (limbs_cnt < required_minimum) {
            // smaller than 1

            try_realloc_inc(required_minimum);
            if (neg) {
                // after this operation, the result sign will be changed
                // example: -0.3 + 1 = 0.7
                mpn_zero(data + limbs_cnt, required_minimum - limbs_cnt);
                mpn_neg(data, data, required_minimum - 1); // invert decimal parts
                normalize_size(required_minimum);
            } else {
                // 0.xxx + 1 = 1.xxx, preserving decimal limbs.
                mpn_zero(data + limbs_cnt, required_minimum - limbs_cnt);
                size = required_minimum;
                data[required_minimum - 1] = 1;
            }
            return;
        }

        if (neg) {
            uint32_t carries = 0;
            while (--data[dec_limbs + carries] == UINT64_MAX) {
                ++carries;
            }
            // mp_size is negative.
            // truncating size if msb is zero
            size += dec_limbs + carries == limbs_cnt - 1 && data[dec_limbs + carries] == 0;

            // solve -1.xxx + 1
            if (-size == dec_limbs) {
                normalize_size(size);
            }
        } else {
            for (uint32_t carries = 0; ++data[dec_limbs + carries] == 0; ++carries) {

                // solve (2^64n - 1) + 1
                if (dec_limbs + carries == limbs_cnt - 1) [[unlikely]] {
                    try_realloc_inc(alloc + 1);
                    data[size++] = 1;
                    return;
                }
            }
        }
    }

    inline void fixed_point_decimal::add(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        assert(result.exp2div64 == lhs.exp2div64);
        assert(result.exp2div64 == rhs.exp2div64);

        int64_t lhs_size = std::abs(lhs.size);
        int64_t rhs_size = std::abs(rhs.size);


        if (lhs.size == 0) {
            if (&result != &rhs) {
                result.try_realloc_inc(rhs_size, true);
                memcpy(result.data, rhs.data, rhs_size * sizeof(mp_limb_t));
                result.size = rhs.size;
            }
            return;
        }
        if (rhs.size == 0) {
            if (&result != &lhs) {
                result.try_realloc_inc(lhs_size);
                memcpy(result.data, lhs.data, lhs_size * sizeof(mp_limb_t));
                result.size = lhs.size;
            }
            return;
        }


        bool lhs_neg = lhs.size < 0;
        bool rhs_neg = rhs.size < 0;
        result.try_realloc_inc(std::max(lhs_size, rhs_size) + 1);

        mp_limb_t *l = lhs.data;
        mp_limb_t *r = rhs.data;
        if (lhs_size < rhs_size) {
            std::swap(l, r);
            std::swap(lhs_size, rhs_size);
            std::swap(lhs_neg, rhs_neg);
        }


        if (lhs_neg == rhs_neg) {
            result.data[lhs_size] = mpn_add(result.data, l, lhs_size, r, rhs_size);
            result.size = lhs_size + (result.data[lhs_size] != 0);
            result.size = lhs_neg ? -result.size : result.size;
        } else {

            if (mpn_sub(result.data, l, lhs_size, r, rhs_size)) {
                mpn_neg(result.data, result.data, lhs_size);
                result.normalize_size(lhs_size);
                result.size *= rhs_neg ? -1 : 1;
            } else {
                result.normalize_size(lhs_size);
                result.size *= lhs_neg ? -1 : 1;
            }
        }
    }

    inline void fixed_point_decimal::normalize_size(const int64_t known_size) {
        size = std::abs(known_size) - 1;
        while (size >= 0 && data[size] == 0) {
            --size;
        }
        ++size;
        size = known_size < 0 ? -size : size;
    }


    inline void fixed_point_decimal::sub(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        assert(result.exp2div64 == lhs.exp2div64);
        assert(result.exp2div64 == rhs.exp2div64);

        int64_t lhs_size = std::abs(lhs.size);
        int64_t rhs_size = std::abs(rhs.size);


        if (lhs.size == 0) {
            if (&result != &rhs) {
                result.try_realloc_inc(rhs_size);
                memcpy(result.data, rhs.data, rhs_size * sizeof(mp_limb_t));
                result.size = -rhs.size;
            }
            return;
        }
        if (rhs.size == 0) {
            if (&result != &lhs) {
                result.try_realloc_inc(lhs_size);
                memcpy(result.data, lhs.data, lhs_size * sizeof(mp_limb_t));
                result.size = lhs.size;
            }
            return;
        }


        bool lhs_neg = lhs.size < 0;
        bool rhs_neg = rhs.size < 0;
        result.try_realloc_inc(std::max(lhs_size, rhs_size) + 1);

        mp_limb_t *l = lhs.data;
        mp_limb_t *r = rhs.data;
        if (lhs_size < rhs_size) {
            std::swap(l, r);
            std::swap(lhs_size, rhs_size);
            std::swap(lhs_neg, rhs_neg);
            lhs_neg = !lhs_neg;
            rhs_neg = !rhs_neg;
        }


        if (lhs_neg == rhs_neg) {
            result.size = lhs_size;
            if (mpn_sub(result.data, l, lhs_size, r, rhs_size)) {
                mpn_neg(result.data, result.data, lhs_size);
                result.normalize_size(lhs_size);
                result.size *= rhs_neg ? 1 : -1;
            } else {
                result.normalize_size(lhs_size);
                result.size *= lhs_neg ? -1 : 1;
            }
        } else {
            result.data[lhs_size] = mpn_add(result.data, l, lhs_size, r, rhs_size);
            result.size = lhs_size + (result.data[lhs_size] != 0);
            result.size = lhs_neg ? -result.size : result.size;
        }
    }

    inline void fixed_point_decimal::zero() { size = 0; }

    inline void fixed_point_decimal::one() {
        const int64_t min_size = -exp2div64 + 1;
        try_realloc_inc(min_size);
        size = min_size;
        data[min_size - 1] = 1;
        mpn_zero(data, min_size - 1);
    }

    inline int32_t fixed_point_decimal::sgn() const {
        return static_cast<int32_t>(size > 0) - static_cast<int32_t>(size < 0);
    }

    inline int32_t fixed_point_decimal::compare_abs(const uint64_t value) const {
        if (value == 0)
            return sgn();

        const int64_t min_size = -exp2div64 + 1;
        const int64_t limbs_cnt = std::abs(size);

        if (limbs_cnt != min_size)
            return limbs_cnt > min_size ? 1 : -1;

        if (data[-exp2div64] != value)
            return data[-exp2div64] > value ? 1 : -1;


        int64_t idx = -exp2div64 - 1;
        while (idx >= 0 && data[idx] == 0)
            --idx;


        return idx == -1 ? 0 : 1;
    }
    inline int32_t fixed_point_decimal::compare(const int64_t value) const {
        if (value == 0)
            return sgn();
        const int64_t min_size = -exp2div64 + 1;
        const int64_t limbs_cnt = std::abs(size);

        if (limbs_cnt != min_size)
            return (limbs_cnt > min_size) == (size > 0) ? 1 : -1;

        const uint64_t value_scale = std::abs(value);
        if (data[-exp2div64] != value_scale)
            return (data[-exp2div64] > value_scale) == (size > 0) ? 1 : -1;


        int64_t idx = -exp2div64 - 1;
        while (idx >= 0 && data[idx] == 0)
            --idx;

        if (idx == -1)
            return 0;

        return size < 0 ? -1 : 1;
    }

    inline void fixed_point_decimal::set(const int64_t value, const bool make_decimal_zero) {
        if (value == 0) {
            zero();
            return;
        }
        const int64_t min_size = -exp2div64 + 1;
        try_realloc_inc(min_size);
        size = value < 0 ? -min_size : min_size;
        data[min_size - 1] = std::abs(value);
        if (make_decimal_zero)
            mpn_zero(data, min_size - 1);
    }

    inline void fixed_point_decimal::sqr(fixed_point_decimal &result, const fixed_point_decimal &v) {
        assert(result.exp2div64 == v.exp2div64);
        assert(&result != &v);

        const int64_t size = std::abs(v.size);
        int64_t result_size = size * 2;

        if (size == 0 || result_size + result.exp2div64 <= 0) {
            result.zero();
            return;
        }

        const mp_limb_t *ptr = v.data;

        result.try_realloc_inc(result_size);
        mp_limb_t *result_ptr = result.data;

        mpn_sqr(result_ptr, ptr, size);
        const mp_limb_t top = result_ptr[result_size - 1];
        result_size -= top == 0;

        mpn_copyi(result_ptr, result_ptr - result.exp2div64, result_size + result.exp2div64);
        result_size += result.exp2div64;
        result.size = result_size;
    }

    inline void fixed_point_decimal::mul(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const uint64_t rhs) {
        assert(result.exp2div64 == lhs.exp2div64);
        const int64_t lhs_size = std::abs(lhs.size);

        if (rhs == 0 || lhs_size == 0) {
            result.size = 0;
            return;
        }

        result.try_realloc_inc(lhs_size + 1);
        result.data[lhs_size] = mpn_mul_1(result.data, lhs.data, lhs_size, rhs);
        result.size = lhs_size + (result.data[lhs_size] != 0);
        result.size = lhs.size < 0 ? -result.size : result.size;
    }

    inline void fixed_point_decimal::mul(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        assert(result.exp2div64 == lhs.exp2div64);
        assert(result.exp2div64 == rhs.exp2div64);
        assert(&result != &lhs && &result != &rhs);

        mp_limb_t *l = lhs.data;
        mp_limb_t *r = rhs.data;
        int64_t lhs_size = lhs.size;
        int64_t rhs_size = rhs.size;
        const mp_size_t sgn = lhs_size * rhs_size;
        lhs_size = std::abs(lhs_size);
        rhs_size = std::abs(rhs_size);
        int64_t result_size = lhs_size + rhs_size;

        if (lhs_size < rhs_size) {
            std::swap(l, r);
            std::swap(lhs_size, rhs_size);
        }
        if (rhs_size == 0 || result_size + result.exp2div64 <= 0) {
            result.zero();
            return;
        }


        result.try_realloc_inc(result_size);
        mp_limb_t *result_ptr = result.data;
        const mp_limb_t top = mpn_mul(result_ptr, l, lhs_size, r, rhs_size);
        result_size -= top == 0;

        mpn_copyi(result_ptr, result_ptr - result.exp2div64, result_size + result.exp2div64);
        result_size += result.exp2div64;
        result_size = sgn < 0 ? -result_size : result_size;
        result.size = result_size;
    }


    inline void fixed_point_decimal::div(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        assert(result.exp2div64 == lhs.exp2div64);
        assert(result.exp2div64 == rhs.exp2div64);

        limbs_lshift(result, lhs, -lhs.exp2div64);
        const int64_t lhs_size = std::abs(result.size);
        const int64_t rhs_size = std::abs(rhs.size);

        if (lhs_size < rhs_size) {
            result.size = 0;
            return;
        }

        result.try_realloc_inc(lhs_size * 2 + 1);
        mpn_tdiv_qr(result.data + lhs_size, result.data + lhs_size * 2 - rhs_size + 1, 0, result.data, lhs_size,
                    rhs.data, rhs_size);
        mpn_copyi(result.data, result.data + lhs_size, lhs_size - rhs_size + 1);
        result.size = lhs_size - rhs_size;
        result.size += result.data[result.size] != 0;
        if (rhs.size * lhs.size < 0)
            result.size = -result.size;
    }

    inline void fixed_point_decimal::limbs_lshift(fixed_point_decimal &result, const fixed_point_decimal &v,
                                                  const int64_t limb_shift) {
        if (v.size == 0) {
            result.size = 0;
            return;
        }

        const int64_t limbs_cnt = std::abs(v.size);
        const int64_t result_limbs_cnt = limbs_cnt + limb_shift;
        result.try_realloc_inc(result_limbs_cnt);
        mpn_copyd(result.data + limb_shift, v.data, limbs_cnt);
        mpn_zero(result.data, limb_shift);
        result.size = v.size < 0 ? -result_limbs_cnt : result_limbs_cnt;
    }

    inline void fixed_point_decimal::limbs_rshift(fixed_point_decimal &result, const fixed_point_decimal &v,
                                                  const int64_t limb_shift) {
        if (v.size == 0) {
            result.size = 0;
            return;
        }


        const int64_t limbs_cnt = std::abs(v.size);
        const int64_t result_limbs_cnt = std::max(static_cast<int64_t>(0), limbs_cnt - limb_shift);
        result.try_realloc_inc(result_limbs_cnt);
        result.size = v.size < 0 ? -result_limbs_cnt : result_limbs_cnt;
        if (result_limbs_cnt > 0)
            mpn_copyi(result.data, v.data + limb_shift, result_limbs_cnt);
    }


    inline void fixed_point_decimal::dbl(fixed_point_decimal &result, const fixed_point_decimal &v) {
        assert(result.exp2div64 == v.exp2div64);
        result.size = v.size;
        const int64_t limbs_cnt = std::abs(result.size);
        if (limbs_cnt == 0) {
            result.size = 0;
            return;
        }
        result.try_realloc_inc(limbs_cnt + 1);
        const mp_limb_t carry = mpn_lshift(result.data, v.data, limbs_cnt, 1);
        if (carry > 0) {
            result.data[limbs_cnt] = carry;
            result.size = result.size < 0 ? result.size - 1 : result.size + 1;
        }
    }


    inline void fixed_point_decimal::hlv(fixed_point_decimal &result, const fixed_point_decimal &v) {
        assert(result.exp2div64 == v.exp2div64);
        result.size = v.size;
        const int64_t limbs_cnt = std::abs(result.size);
        if (limbs_cnt == 0) {
            result.size = 0;
            return;
        }
        result.try_realloc_inc(limbs_cnt);
        mpn_rshift(result.data, v.data, limbs_cnt, 1);
        if (result.data[limbs_cnt - 1] == 0)
            result.size = result.size < 0 ? result.size + 1 : result.size - 1;
    }

    inline void fixed_point_decimal::neg() { size = -size; }

    inline void fixed_point_decimal::set_exp10(const int64_t exp10, const bool preserveValue) {
        const int64_t new_exp2div64 = exp10_to_exp2div64(exp10);
        set_exp2div64(new_exp2div64, preserveValue);
    }

    inline void fixed_point_decimal::set_exp2div64(const int64_t new_exp2div64, const bool preserveValue) {
        if (preserveValue) {
            if (exp2div64 < new_exp2div64) {
                limbs_rshift(*this, *this, new_exp2div64 - exp2div64);
            } else if (exp2div64 > new_exp2div64) {
                limbs_lshift(*this, *this, exp2div64 - new_exp2div64);
            }
        } else {
            size = 0;
        }
        exp2div64 = new_exp2div64;
    }


    inline fixed_point_decimal::operator float() const { return static_cast<float>(operator double()); }

    inline fixed_point_decimal::operator double() const {
        if (size == 0) {
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
        const uint64_t exponent = 0x3ff0000000000000ULL + (static_cast<uint64_t>(f_exp2) << 52u);
#endif
        const uint64_t sig = size > 0 ? 0 : 0x8000000000000000ULL;
        return std::bit_cast<double>(sig | exponent | mantissa_bit);
    }

    template<Number Exp, Number Mantissa, Number Bit>
    fixed_point_decimal::operator exponent<Exp, Mantissa, Bit>() const {
        if (size == 0) {
            return exponent<Exp, Mantissa, Bit>::ZERO;
        }
        uint64_t mantissa_bit;
        mp_size_t f_exp2;
        export_value(mantissa_bit, f_exp2);

        const auto mantissa = std::bit_cast<double>(0x3ff0000000000000ULL | mantissa_bit);

        auto result = exponent<Exp, Mantissa, Bit>::mul_2exp(
                exponent<Exp, Mantissa, Bit>(static_cast<Mantissa>(mantissa)), static_cast<int>(f_exp2));
        return size > 0 ? result : -result;
    }

    inline std::string fixed_point_decimal::to_string() const {
        mpf_t f;
        mpz_t z;
        mpf_init2(f, -exp2div64 * 64);
        mpz_init(z);
        const int64_t limbs_cnt = std::abs(size);
        mp_limb_t *limbs = mpz_limbs_write(z, limbs_cnt);
        memcpy(limbs, data, limbs_cnt * sizeof(mp_limb_t));
        mpz_limbs_finish(z, limbs_cnt);
        if (size < 0)
            mpz_neg(z, z);
        mpf_set_z(f, z);
        mpf_div_2exp(f, f, -exp2div64 * 64);

        const auto digits =
                static_cast<int>(-static_cast<double>(exp2div64) * 64 * std::numbers::ln2 / std::numbers::ln10);
        char *str;
        gmp_asprintf(&str, "%.*Ff", digits, f);
        std::string result(str);

        // gmp_asprinf uses malloc(), Do not remove this
        free(str);
        mpf_clear(f);
        return result;
    }


    inline void fixed_point_decimal::export_value(uint64_t &mantissa_bit, mp_size_t &f_exp2) const {

        const mp_limb_t *src_ptr = data;
        static constexpr auto MANTISSA_MASK = 0x000fffffffffffffULL;
        const int64_t limbs_cnt = std::abs(size);

        assert(limbs_cnt > 0);

        const int64_t shift = limbs_cnt * 64 - std::countl_zero(*(src_ptr + limbs_cnt - 1)) - 53;
        if (shift <= 0) {
            assert(shift > -53);
            mantissa_bit = *src_ptr << static_cast<uint64_t>(-shift) & MANTISSA_MASK;
        } else {
            const uint64_t limb_skip = static_cast<uint64_t>(shift) >> 6u;
            const uint64_t shift_small = shift - limb_skip * 64;
            const auto dst0 = src_ptr + limb_skip;
            if (shift_small <= 12) {
                mantissa_bit = *dst0 >> shift_small & MANTISSA_MASK;
            } else {
                assert(limbs_cnt > limb_skip + 1);
                const auto dst1 = *(dst0 + 1);
                mantissa_bit = (dst1 << (64 - shift_small) | *dst0 >> shift_small) & MANTISSA_MASK;
            }
        }
        f_exp2 = exp2div64 * 64 + shift + 52;
    }
} // namespace merutilm::rff2
