//
// Created by Merutilm on 2026-04-25.
//
#pragma once
#include <cmath>
#include <gmp.h>
#include <stdexcept>

#include "dex.h"


namespace merutilm::rff2 {
    struct fixed_point_decimal {
        static constexpr size_t RAW_ARR_LEN = 7;
        // 0~1 : add, sub, mul(hi), div(hi) | 2~6 temp
        mpz_t temp = {};
        int sgn;
        size_t int_limbs_count;
        size_t dec_limbs_count;

        size_t int_limbs_to_read;

        int offset = 0;
        mp_limb_t *raw = nullptr;

        explicit fixed_point_decimal() : fixed_point_decimal(0.0, 0, 0) {
        }

        explicit fixed_point_decimal(double v, int dec_exp10, int int_exp10);

        explicit fixed_point_decimal(const std::string &str, int dec_exp10, int int_exp10);

        explicit fixed_point_decimal(const dex &v, int dec_exp10, int int_exp10);

        ~fixed_point_decimal();

        fixed_point_decimal(const fixed_point_decimal &);

        fixed_point_decimal &operator=(const fixed_point_decimal &);

        fixed_point_decimal(fixed_point_decimal &&) noexcept;

        fixed_point_decimal &operator=(fixed_point_decimal &&) noexcept;

        template<typename F>
            requires std::is_invocable_r_v<int, F, mpf_t, int>
        void init_data(int dec_exp10, int int_exp10, F &&setter_exp2_getter);

        mp_limb_t *get_limbs_from_mpf(mpf_t src, int limbs_count);

        static int dec_exp10_to_exp2div64(int exp10);

        /**
         * Fast-addition. decimal limbs count must be the same for all, and int limbs to be read must be result == lhs >= rhs.
         * If an overflow occurs, the most significant limbs are discarded.
         * in-place operation is supported.
         * @param result the reference of result.
         * @param lhs left operand
         * @param rhs right operand
         */
        static void add(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        /**
         * Fast-subtraction. decimal limbs count must be the same for all, and int limbs to be read must be result == lhs >= rhs.
         * If an overflow occurs, the most significant limbs are discarded.
         * in-place operation is supported.
         * @param result the reference of result.
         * @param lhs left operand
         * @param rhs right operand
         */
        static void sub(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        /**
         * Fast-square. decimal limbs count must be the same for all, and int limbs to be read must be result == v.
         * If an overflow occurs, the most significant limbs are discarded.
         * [CAUTION] in-place operation is not supported.
         * @param result the reference of result.
         * @param v operand
         */
        static void sqr(fixed_point_decimal &result, const fixed_point_decimal &v);


        /**
         * Fast-multiplication. decimal limbs count must be the same for all, and int limbs to be read must be result == lhs >= rhs.
         * If an overflow occurs, the most significant limbs are discarded.
         * [CAUTION] in-place operation is not supported.
         * @param result the reference of result.
         * @param lhs left operand
         * @param rhs right operand
         */
        static void mul(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);


        /**
         * Fast-division. decimal limbs count must be the same for all, and int limbs to be read must be result == lhs >= rhs.
         * If an overflow occurs, the most significant limbs are discarded.
         * in-place operation is supported.
         * @param result the reference of result.
         * @param lhs left operand
         * @param rhs right operand
         */
        static void div(fixed_point_decimal &result, const fixed_point_decimal &lhs, const fixed_point_decimal &rhs);

        /**
         * Fast-doubling.
         * If an overflow occurs, the most significant limbs are discarded.
         * in-place operation is supported.
         * @param result the reference of result
         * @param v operand
         */
        static void dbl(fixed_point_decimal &result, const fixed_point_decimal &v);

        /**
         * Fast-halving.
         * in-place operation is supported.
         * @param result the reference of result
         * @param v operand
         */
        static void hlv(fixed_point_decimal &result, const fixed_point_decimal &v);

        static void neg(fixed_point_decimal &v);

        void set_limbs_count(size_t new_dec_limbs_count, size_t new_int_limbs_count);

        void set_exp10(int dec_exp10, int int_exp10);

        static int normalized_limbs_count(const mp_limb_t *limbs, int limbs_count);

        [[nodiscard]] mp_limb_t *get_value_ptr() const;

        void temp_write_limbs(const mp_limb_t *limbs, int limbs_read_count);

        static int int_exp10_to_exp2div64(int exp10);

        double double_value();

        void dex_value(dex &result);

        std::string to_string();

        size_t limbs_read_count() const;

        size_t limbs_count() const;


        static void make_operation_compatible(fixed_point_decimal &result, const fixed_point_decimal &v);

        void set_int_limbs_to_read(size_t new_int_limbs_to_read);

        void export_value(int *exp2, int *shift, uint64_t *mantissa_bit, size_t *cnt, int *f_exp2);
    };


    inline fixed_point_decimal::fixed_point_decimal(double v, const int dec_exp10, const int int_exp10) {
        init_data(dec_exp10, int_exp10, [v](mpf_t val, const int exp2div64) {
            mpf_set_d(val, v);
            return exp2div64 * 64;
        });
    }


    inline fixed_point_decimal::fixed_point_decimal(const std::string &str, const int dec_exp10, const int int_exp10) {
        init_data(dec_exp10, int_exp10, [str](mpf_t val, const int exp2div64) {
            mpf_set_str(val, str.data(), 10);
            return exp2div64 * 64;
        });
    }

    inline fixed_point_decimal::fixed_point_decimal(const dex &v, const int dec_exp10, const int int_exp10) {
        init_data(dec_exp10, int_exp10, [v](mpf_t val, const int exp2div64) {
            mpf_set_d(val, v.get_mantissa());
            return exp2div64 * 64 - v.get_exp2();
        });
    }


    inline fixed_point_decimal::~fixed_point_decimal() {
        delete[] this->raw;
        mpz_clear(this->temp);
    }


    inline fixed_point_decimal::fixed_point_decimal(const fixed_point_decimal &other) : sgn(other.sgn),
        int_limbs_count(other.int_limbs_count), dec_limbs_count(other.dec_limbs_count),
        int_limbs_to_read(other.int_limbs_to_read),
        offset(other.offset), raw(new mp_limb_t[limbs_count() * RAW_ARR_LEN]) {
        mpz_init(this->temp);
        memcpy(this->raw, other.raw, limbs_count() * RAW_ARR_LEN * sizeof(mp_limb_t));
    }


    inline fixed_point_decimal &fixed_point_decimal::operator=(const fixed_point_decimal &other) {
        if (&other == this)
            return *this;

        if (this->limbs_count() != other.limbs_count()) {
            delete[] this->raw;
            this->raw = new mp_limb_t[other.limbs_count() * RAW_ARR_LEN];
        }

        this->sgn = other.sgn;
        this->int_limbs_count = other.int_limbs_count;
        this->dec_limbs_count = other.dec_limbs_count;
        this->int_limbs_to_read = other.int_limbs_to_read;
        this->offset = other.offset;
        memcpy(this->raw, other.raw, limbs_count() * RAW_ARR_LEN * sizeof(mp_limb_t));
        return *this;
    }


    inline fixed_point_decimal::fixed_point_decimal(fixed_point_decimal &&other) noexcept : sgn(other.sgn),
        int_limbs_count(other.int_limbs_count), dec_limbs_count(other.dec_limbs_count),
        int_limbs_to_read(other.int_limbs_to_read), offset(other.offset), raw(other.raw) {
        mpz_init(this->temp);
        mpz_swap(this->temp, other.temp);
        other.raw = nullptr;
    }


    inline fixed_point_decimal &fixed_point_decimal::operator=(fixed_point_decimal &&other) noexcept {
        if (&other == this)
            return *this;

        mpz_swap(this->temp, other.temp);

        this->sgn = other.sgn;
        this->int_limbs_count = other.int_limbs_count;
        this->dec_limbs_count = other.dec_limbs_count;
        this->int_limbs_to_read = other.int_limbs_to_read;
        this->offset = other.offset;
        std::swap(this->raw, other.raw);
        return *this;
    }


    template<typename F>
        requires std::is_invocable_r_v<int, F, mpf_t, int>
    void fixed_point_decimal::init_data(const int dec_exp10, const int int_exp10, F &&setter_exp2_getter) {
        mpz_init(this->temp);
        const int dec_exp2div64 = dec_exp10_to_exp2div64(dec_exp10);
        const int int_exp2div64 = int_exp10_to_exp2div64(int_exp10);
        mpf_t val;

        dec_limbs_count = -dec_exp2div64;
        int_limbs_count = int_exp2div64;
        int_limbs_to_read = int_limbs_count;

        mpf_init2(val, (dec_limbs_count + int_limbs_count) * 64);
        const int exp2 = setter_exp2_getter(val, dec_exp2div64);

        if (exp2 < 0) {
            mpf_mul_2exp(val, val, -exp2);
        } else {
            mpf_div_2exp(val, val, exp2);
        }


        mpz_set_f(temp, val);
        const mp_limb_t *lmb1 = mpz_limbs_read(temp);
        const int size = mpz_size(temp);
        if (size > limbs_count())
            throw std::overflow_error("limbs overflow");
        auto limbs = std::vector<mp_limb_t>(limbs_count());
        mpn_copyi(limbs.data(), lmb1, size);
        mpf_clear(val);

        this->sgn = mpf_sgn(val);
        this->raw = new mp_limb_t[limbs_count() * RAW_ARR_LEN];
        memcpy(raw, limbs.data(), limbs_count() * sizeof(mp_limb_t));
    }


    inline mp_limb_t *fixed_point_decimal::get_limbs_from_mpf(mpf_t src, const int limbs_count) {
        mpz_set_f(temp, src);
        const mp_limb_t *lmb1 = mpz_limbs_read(temp);
        const int size = mpz_size(temp);
        if (size > limbs_count)
            throw std::overflow_error("limbs overflow");
        const auto limbs = new mp_limb_t[limbs_count]();
        mpn_copyi(limbs, lmb1, size);
        return limbs;
    }


    inline void fixed_point_decimal::temp_write_limbs(const mp_limb_t *limbs, const int limbs_read_count) {
        const size_t lc = limbs_count();
        mp_limb_t *v = mpz_limbs_write(temp, lc);
        memcpy(v, limbs, limbs_read_count * sizeof(mp_limb_t));
        std::fill(v + limbs_read_count, v + lc, 0);
        mpz_limbs_finish(temp, lc);
    }

    inline int fixed_point_decimal::int_exp10_to_exp2div64(const int exp10) {
        constexpr double log10_2 = 0.301029995663981;
        auto exp2div64 = static_cast<int>(static_cast<double>(exp10) / log10_2);
        exp2div64 = (exp2div64 + 63) / 64;
        return exp2div64;
    }


    inline int fixed_point_decimal::dec_exp10_to_exp2div64(const int exp10) {
        constexpr double log10_2 = 0.301029995663981;
        auto exp2div64 = static_cast<int>(static_cast<double>(exp10) / log10_2);
        exp2div64 = (exp2div64 - 63) / 64;
        return exp2div64;
    }


    inline void fixed_point_decimal::add(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {

        const int l_lc = lhs.limbs_read_count();
        const int r_lc = rhs.limbs_read_count();

        if (lhs.sgn == 0) {
            if (&result != &rhs) {
                memcpy(result.raw, rhs.get_value_ptr(), r_lc * sizeof(mp_limb_t));
                result.sgn = rhs.sgn;
                std::fill(result.raw + rhs.int_limbs_to_read, result.raw + result.int_limbs_to_read, 0);
                result.offset = 0;
            }
            return;
        }
        if (rhs.sgn == 0) {
            if (&result != &lhs) {
                memcpy(result.raw, lhs.get_value_ptr(), l_lc * sizeof(mp_limb_t));
                result.sgn = lhs.sgn;
                std::fill(result.raw + lhs.int_limbs_to_read, result.raw + result.int_limbs_to_read, 0);
                result.offset = 0;
            }
            return;
        }

        const auto l_value = lhs.get_value_ptr();
        const auto r_value = rhs.get_value_ptr();

        result.offset = 0;

        if (lhs.sgn == rhs.sgn) {
            result.sgn = lhs.sgn;
            mpn_add(result.raw, l_value, l_lc, r_value, r_lc);
        } else {
            const int borrow = mpn_sub(result.raw, l_value, l_lc, r_value, r_lc);
            if (borrow == 0) {
                result.sgn = lhs.sgn;
            } else {
                mpn_neg(result.raw, result.raw, l_lc);
                result.sgn = rhs.sgn;
            }
        }
    }


    inline void fixed_point_decimal::sub(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        const int l_lc = lhs.limbs_read_count();
        const int r_lc = rhs.limbs_read_count();

        if (lhs.sgn == 0) {
            if (&result != &rhs) {
                memcpy(result.raw, rhs.get_value_ptr(), r_lc * sizeof(mp_limb_t));
                result.sgn = -rhs.sgn;
                std::fill(result.raw + rhs.int_limbs_to_read, result.raw + result.int_limbs_to_read, 0);
                result.offset = 0;
            }
            return;
        }
        if (rhs.sgn == 0) {
            if (&result != &lhs) {
                memcpy(result.raw, lhs.get_value_ptr(), l_lc * sizeof(mp_limb_t));
                result.sgn = lhs.sgn;
                std::fill(result.raw + lhs.int_limbs_to_read, result.raw + result.int_limbs_to_read, 0);
                result.offset = 0;
            }
            return;
        }

        const auto l_value = lhs.get_value_ptr();
        const auto r_value = rhs.get_value_ptr();

        result.offset = 0;

        if (lhs.sgn == rhs.sgn) {
            const int borrow = mpn_sub(result.raw, l_value, l_lc, r_value, r_lc);
            if (borrow == 0) {
                result.sgn = lhs.sgn;
            } else {
                mpn_neg(result.raw, result.raw, l_lc);
                result.sgn = -rhs.sgn;
            }
        } else {
            result.sgn = lhs.sgn;
            mpn_add(result.raw, l_value, l_lc, r_value, r_lc);
        }
    }


    inline void fixed_point_decimal::sqr(fixed_point_decimal &result, const fixed_point_decimal &v) {

        const int lc = result.limbs_count();
        const int lrc = result.limbs_read_count();
        const mp_limb_t *lhs_value = v.get_value_ptr();

        if (v.sgn == 0) {
            mpn_zero(result.raw, lrc);
            result.offset = 0;
            result.sgn = 0;
            return;
        }

        mpn_sqr(result.raw + lc + result.int_limbs_count, lhs_value, lrc);

        result.offset = lc * 2;
        result.sgn = 1;
    }


    inline void fixed_point_decimal::mul(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        const int lc = result.limbs_count();
        const int l_lc = lhs.limbs_read_count();
        const int r_lc = rhs.limbs_read_count();
        const mp_limb_t *lhs_value = lhs.get_value_ptr();
        const mp_limb_t *rhs_value = rhs.get_value_ptr();

        if (lhs.sgn == 0 || rhs.sgn == 0) {
            mpn_zero(result.raw, l_lc);
            result.offset = 0;
            result.sgn = 0;
            return;
        }
        // offset 0 : [D][D][Z][Z][Z]
        // offset lc : [D][D][D][D][Z][Z][Z][Z][Z][Z]
        // [D][D][Z][Z][Z] | [X][X][X][D][D] | [D][D][Z][Z][Z] | [Z][Z][Z][X][X]
        //                                      ^start      ^end

        mpn_mul(result.raw + lc + result.int_limbs_count, lhs_value, l_lc, rhs_value, r_lc);

        result.offset = lc * 2;
        result.sgn = lhs.sgn * rhs.sgn;
    }


    inline void fixed_point_decimal::div(fixed_point_decimal &result, const fixed_point_decimal &lhs,
                                         const fixed_point_decimal &rhs) {
        const int lc = result.limbs_count();

        const auto l_lc = lhs.limbs_read_count();
        const auto r_lc = rhs.limbs_read_count();
        const mp_limb_t *lhs_value = lhs.get_value_ptr();
        const mp_limb_t *rhs_value = rhs.get_value_ptr();
        if (lhs.sgn == 0) {
            mpn_zero(result.raw, lc);
            result.sgn = 0;
            return;
        }
        if (rhs.sgn == 0) {
            throw std::overflow_error("divide by zero");
        }
        const int dividend_size = result.dec_limbs_count + l_lc;
        const int divisor_size = normalized_limbs_count(rhs_value, r_lc);

        mpn_zero(result.raw + lc * 3, result.dec_limbs_count);
        mpn_copyi(result.raw + lc * 3 + result.dec_limbs_count, lhs_value, l_lc);
        mpn_tdiv_qr(result.raw, result.raw + lc * 5, 0, result.raw + lc * 3, dividend_size, rhs_value, divisor_size);

        const int result_size = dividend_size - divisor_size + 1;
        // result.dec_limbs_count + l_lc - divisor_size + 1
        const int cpy_cnt = l_lc - result_size;
        // cpy_cnt = divisor_size - result.dec_limbs_count - 1
        // if cpy_cnt < 0, limbs can be overflowed
        if (cpy_cnt > 0) std::fill_n(result.raw + result_size, cpy_cnt, 0);

        result.sgn = lhs.sgn * rhs.sgn;
        result.offset = 0;
    }


    inline void fixed_point_decimal::dbl(fixed_point_decimal &result, const fixed_point_decimal &v) {
        mpn_lshift(result.raw, v.get_value_ptr(), v.limbs_read_count(), 1);
        result.sgn = v.sgn;
        result.offset = 0;
    }


    inline void fixed_point_decimal::hlv(fixed_point_decimal &result, const fixed_point_decimal &v) {
        mpn_rshift(result.raw, v.get_value_ptr(), v.limbs_read_count(), 1);
        result.sgn = v.sgn;
        result.offset = 0;
    }

    inline void fixed_point_decimal::neg(fixed_point_decimal &v) { v.sgn = -v.sgn; }


    inline void fixed_point_decimal::set_limbs_count(const size_t new_dec_limbs_count,
                                                     const size_t new_int_limbs_count) {
        if ((new_dec_limbs_count == SIZE_MAX || new_dec_limbs_count == dec_limbs_count) &&
            (new_int_limbs_count == SIZE_MAX || new_int_limbs_count == int_limbs_count))
            return;

        const int dec_copy_count = std::min(new_dec_limbs_count, dec_limbs_count);
        const int int_copy_count = std::min(new_int_limbs_count, int_limbs_to_read);
        const int src_offset = dec_limbs_count - dec_copy_count;
        const int dst_offset = new_dec_limbs_count - dec_copy_count;
        const auto new_raw = new mp_limb_t[(new_dec_limbs_count + new_int_limbs_count) * RAW_ARR_LEN]();
        memcpy(new_raw + dst_offset, get_value_ptr() + src_offset,
               sizeof(mp_limb_t) * (dec_copy_count + int_copy_count));

        delete[] raw;
        raw = new_raw;
        offset = 0;
        dec_limbs_count = new_dec_limbs_count;
        int_limbs_count = new_int_limbs_count;
        int_limbs_to_read = new_int_limbs_count;
    }


    inline void fixed_point_decimal::set_exp10(const int dec_exp10, const int int_exp10) {
        const int dec_exp2div64 = dec_exp10_to_exp2div64(dec_exp10);
        const int int_exp2div64 = int_exp10_to_exp2div64(int_exp10);

        set_limbs_count(-dec_exp2div64, int_exp2div64);
    }


    inline int fixed_point_decimal::normalized_limbs_count(const mp_limb_t *limbs, const int limbs_count) {
        int cnt = limbs_count;
        while (cnt > 0 && limbs[cnt - 1] == 0)
            --cnt;
        return cnt;
    }


    inline mp_limb_t *fixed_point_decimal::get_value_ptr() const { return raw + offset; }


    inline double fixed_point_decimal::double_value() {
        if (sgn == 0) {
            return 0;
        }

        uint64_t mantissa_bit;
        size_t cnt;
        int shift;
        int exp2;
        int f_exp2;

        export_value(&exp2, &shift, &mantissa_bit, &cnt, &f_exp2);
        // 0100 0000 0000 : 2^1
        // 0000 0000 0000 : 2^-1023
        // 0111 1111 1111 : 2^1024
        if (f_exp2 > 0x03ff) {
            return sgn == 1 ? INFINITY : -INFINITY;
        }
        const int mantissa_shift = f_exp2 <= -0x03ff ? -0x03ff - f_exp2 + 1 : 0;
        mantissa_bit = mantissa_bit >> mantissa_shift & 0x800fffffffffffffULL;

        const uint64_t exponent = f_exp2 <= -0x03ff ? 0 : 0x3ff0000000000000ULL + (static_cast<uint64_t>(f_exp2) << 52);
        const uint64_t sig = sgn == 1 ? 0 : 0x8000000000000000ULL;
        return std::bit_cast<double>(sig | exponent | mantissa_bit);
    }

    inline void fixed_point_decimal::dex_value(dex &result) {
        if (sgn == 0) {
            dex::cpy(result, dex::ZERO);
            return;
        }
        uint64_t mantissa_bit;
        size_t cnt;
        int shift;
        int exp2;
        int f_exp2;

        export_value(&exp2, &shift, &mantissa_bit, &cnt, &f_exp2);

        const double mantissa = std::bit_cast<double>(0x3ff0000000000000ULL | mantissa_bit);
        dex::cpy(result, mantissa);
        dex::mul_2exp(result, result, f_exp2);
        if (sgn == -1)
            dex::neg(result);
    }


    inline std::string fixed_point_decimal::to_string() {
        mpf_t d;
        const int exp2 = -dec_limbs_count * 64;
        temp_write_limbs(get_value_ptr(), limbs_read_count());
        if (sgn == -1)
            mpz_neg(temp, temp);

        mpf_init2(d, limbs_read_count() * 64);

        if (exp2 < 0) {
            mpf_set_z(d, temp);
            mpf_div_2exp(d, d, -exp2);
        } else {
            mpf_set_z(d, temp);
            mpf_mul_2exp(d, d, exp2);
        }
        char *str;
        gmp_asprintf(&str, "%.Ff", d);
        std::string result(str);

        // gmp_asprinf uses malloc(), Do not remove this
        free(str);
        mpf_clear(d);
        return result;
    }

    inline size_t fixed_point_decimal::limbs_read_count() const { return int_limbs_to_read + dec_limbs_count; }

    inline size_t fixed_point_decimal::limbs_count() const { return int_limbs_count + dec_limbs_count; }

    inline void fixed_point_decimal::make_operation_compatible(fixed_point_decimal &result, const fixed_point_decimal &v) {
        result.set_int_limbs_to_read(v.int_limbs_to_read);
    }


    inline void fixed_point_decimal::set_int_limbs_to_read(const size_t new_int_limbs_to_read) {
#ifndef NDEBUG
        if (new_int_limbs_to_read > int_limbs_count) throw std::invalid_argument("limbs overflow");
#endif
        if (int_limbs_to_read < new_int_limbs_to_read) {
            mp_limb_t *ptr = get_value_ptr();
            std::fill(ptr + int_limbs_to_read, ptr + new_int_limbs_to_read , 0);
        }
        int_limbs_to_read = new_int_limbs_to_read;
    }


    inline void fixed_point_decimal::export_value(int *exp2, int *shift, uint64_t *mantissa_bit, size_t *cnt,
                                                  int *f_exp2) {
        *exp2 = -dec_limbs_count * 64;
        temp_write_limbs(get_value_ptr(), limbs_read_count());
        const size_t len = mpz_sizeinbase(temp, 2);
        *shift = static_cast<int>(len - 53);
        if (*shift < 0) {
            mpz_mul_2exp(temp, temp, -*shift);
        } else {
            mpz_div_2exp(temp, temp, *shift);
        }
        mpz_export(mantissa_bit, cnt, -1, sizeof(uint64_t), 0, 0, temp);
        *f_exp2 = *exp2 + *shift + 52;
    }
} // namespace merutilm::rff2
