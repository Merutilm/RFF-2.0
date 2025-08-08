//
// Created by Merutilm on 2025-05-05.
//

#include "fp_decimal_calculator.h"

#include <cfloat>
#include <cmath>

#include "fp_complex_calculator.h"
#include "../constants/Constants.hpp"

namespace merutilm::rff2 {
    fp_decimal_calculator::fp_decimal_calculator() {
        exp2 = 0;
        mpz_init(value);
        mpz_init(temp);
    }

    fp_decimal_calculator::fp_decimal_calculator(const mpz_srcptr value, const int exp2) {
        mpz_init(this->value);
        mpz_init(temp);
        mpz_set(this->value, value);
        this->exp2 = exp2;
    }

    fp_decimal_calculator::fp_decimal_calculator(const dex &d, const int exp10) {
        mpz_init(value);
        mpz_init(temp);

        mpf_t d1;
        exp2 = exp10ToExp2(exp10);

        mpf_init2(d1, -exp2);
        mpf_set_d(d1, d.get_mantissa());

        if (const int mul_div = exp2 - d.get_exp2();
            mul_div < 0
        ) {
            mpf_mul_2exp(d1, d1, -mul_div);
        } else {
            mpf_div_2exp(d1, d1, mul_div);
        }

        mpz_set_f(value, d1);
        mpf_clear(d1);
    }

    fp_decimal_calculator::fp_decimal_calculator(const double d, const int exp10) {
        mpz_init(value);
        mpz_init(temp);

        mpf_t d1;
        exp2 = exp10ToExp2(exp10);

        mpf_init2(d1, -exp2);
        mpf_set_d(d1, d);

        if (exp2 < 0) {
            mpf_mul_2exp(d1, d1, -exp2);
        } else {
            mpf_div_2exp(d1, d1, exp2);
        }

        mpz_set_f(value, d1);
        mpf_clear(d1);
    }

    fp_decimal_calculator::fp_decimal_calculator(const std::string &str, const int exp10) {
        mpz_init(value);
        mpz_init(temp);

        mpf_t d1;
        exp2 = exp10ToExp2(exp10);


        mpf_init2(d1, -exp2);
        mpf_set_str(d1, str.c_str(), 10);

        if (exp2 < 0) {
            mpf_mul_2exp(d1, d1, -exp2);
        } else {
            mpf_div_2exp(d1, d1, exp2);
        }

        mpz_set_f(value, d1);
        mpf_clear(d1);
    }


    fp_decimal_calculator::~fp_decimal_calculator() {
        mpz_clear(value);
        mpz_clear(temp);
    }


    fp_decimal_calculator::fp_decimal_calculator(const fp_decimal_calculator &other) {
        mpz_init_set(temp, other.temp);
        mpz_init_set(value, other.value);
        exp2 = other.exp2;
    }

    fp_decimal_calculator &fp_decimal_calculator::operator=(const fp_decimal_calculator &other) {
        if (this != &other) {
            mpz_set(value, other.value);
            exp2 = other.exp2;
        }
        return *this;
    }


    fp_decimal_calculator::fp_decimal_calculator(fp_decimal_calculator &&other) noexcept {
        mpz_init(value);
        mpz_init(temp);

        mpz_swap(value, other.value);
        mpz_swap(temp, other.temp);

        exp2 = other.exp2;
    }

    fp_decimal_calculator &fp_decimal_calculator::operator=(fp_decimal_calculator &&other) noexcept {
        exp2 = other.exp2;
        mpz_swap(value, other.value);
        return *this;
    }

    /**
     * <b> A + B</b> <br/><br/>
     * Fast addition. It assumes that the exponents of both numbers are the same.
     * @param out the result
     * @param a input A
     * @param b input B
     */
    void fp_decimal_calculator::fp_add(fp_decimal_calculator &out, const fp_decimal_calculator &a,
                                        const fp_decimal_calculator &b) {
        mpz_add(out.value, a.value, b.value);
    }

    /**
     * <b> A - B </b> <br/><br/>
     * Fast subtraction. It assumes that the exponents of both numbers are the same.
     * @param out the result
     * @param a input A
     * @param b input B
     */
    void fp_decimal_calculator::fp_sub(fp_decimal_calculator &out, const fp_decimal_calculator &a,
                                        const fp_decimal_calculator &b) {
        mpz_sub(out.value, a.value, b.value);
    }

    /**
     * <b> A * B </b> <br/><br/>
     * Fast multiplication. It assumes that the exponents of both numbers are the same.
     * @param out the result
     * @param a input A
     * @param b input B
     */
    void fp_decimal_calculator::fp_mul(fp_decimal_calculator &out, const fp_decimal_calculator &a,
                                        const fp_decimal_calculator &b) {
        mpz_mul(out.temp, a.value, b.value);
        mpz_div_2exp(out.value, out.temp, -a.exp2);
    }

    /**
     * <b> A / B </b> <br/><br/>
     * Fast division. It assumes that the exponents of both numbers are the same.
     * @param out the result
     * @param a input A
     * @param b input B
     */
    void fp_decimal_calculator::fp_div(fp_decimal_calculator &out, const fp_decimal_calculator &a,
                                        const fp_decimal_calculator &b) {
        const auto vbl = static_cast<int>(mpz_sizeinbase(b.value, 2));

        const int e = b.exp2 + vbl;
        mpz_mul_2exp(out.temp, a.value, vbl);
        mpz_div(out.temp, out.temp, b.value);
        if (e < 0) {
            mpz_mul_2exp(out.value, out.temp, -e);
        } else {
            mpz_div_2exp(out.value, out.temp, e);
        }
    }

    /**
     * Doubles value. It assumes that the exponents of both numbers are the same.
     * @param out the result
     * @param target input B
     */
    void fp_decimal_calculator::fp_dbl(fp_decimal_calculator &out, const fp_decimal_calculator &target) {
        mpz_mul_2exp(out.value, target.value, 1);
    }

    /**
     * Halves value. It assumes that the exponents of both numbers are the same.
     * @param out the result
     * @param target input B
     */
    void fp_decimal_calculator::fp_hlv(fp_decimal_calculator &out, const fp_decimal_calculator &target) {
        mpz_div_2exp(out.value, target.value, 1);
    }

    /**
     * Do swap. It assumes that the exponents of both numbers are the same.
     * @param a input A
     * @param b input B
     */
    void fp_decimal_calculator::fp_swap(fp_decimal_calculator &a, fp_decimal_calculator &b) {
        mpz_swap(a.value, b.value);
    }

    void fp_decimal_calculator::negate(fp_decimal_calculator &target) {
        mpz_neg(target.value, target.value);
    }

    int fp_decimal_calculator::exp2ToExp10(const int exp2) {
        return static_cast<int>(static_cast<double>(exp2) * Constants::Num::LOG10_2);
    }

    int fp_decimal_calculator::exp10ToExp2(const int precision) {
        return static_cast<int>(static_cast<double>(precision) / Constants::Num::LOG10_2);
    }


    double fp_decimal_calculator::double_value() {
        const int sgn = mpz_sgn(value);
        if (sgn == 0) {
            return 0;
        }

        mpz_abs(temp, value);
        const size_t len = mpz_sizeinbase(temp, 2);
        const auto shift = static_cast<int>(len - 53);
        if (shift < 0) {
            mpz_mul_2exp(temp, temp, -shift);
        } else {
            mpz_div_2exp(temp, temp, shift);
        }
        uint64_t mantissa;
        size_t cnt;
        mpz_export(&mantissa, &cnt, -1, sizeof(mantissa), 0, 0, temp);


        const int fExp2 = exp2 + shift + 52;
        //0100 0000 0000 : 2^1
        //0000 0000 0000 : 2^-1023
        //0111 1111 1111 : 2^1024
        if (fExp2 > 0x03ff) {
            return sgn == 1 ? INFINITY : -INFINITY;
        }
        const int mantissa_shift = fExp2 <= -0x03ff ? -0x03ff - fExp2 + 1 : 0;
        mantissa = (mantissa >> mantissa_shift) & 0x800fffffffffffffULL;

        const uint64_t exponent = fExp2 <= -0x03ff
                                      ? 0
                                      : 0x3ff0000000000000ULL + (
                                            static_cast<uint64_t>(fExp2) << 52);
        const uint64_t sig = sgn == 1 ? 0 : 0x8000000000000000ULL;
        return std::bit_cast<double>(sig | exponent | mantissa);
    }

    void fp_decimal_calculator::double_exp_value(dex *result) {
        const int sgn = mpz_sgn(value);
        if (sgn == 0) {
            dex::cpy(result, dex::ZERO);
            return;
        }

        mpz_abs(temp, value);
        const size_t len = mpz_sizeinbase(temp, 2);
        const auto shift = static_cast<int>(len - 53);
        if (shift < 0) {
            mpz_mul_2exp(temp, temp, -shift);
        } else {
            mpz_div_2exp(temp, temp, shift);
        }
        uint64_t mantissa_bit;
        size_t cnt;
        mpz_export(&mantissa_bit, &cnt, -1, sizeof(mantissa_bit), 0, 0, temp);
        const int fExp2 = exp2 + shift + 52;
        const double mantissa = std::bit_cast<double>(0x3ff0000000000000ULL | mantissa_bit);

        dex::cpy(result, mantissa);
        dex::mul_2exp(result, *result, fExp2);
        if (sgn == -1) dex::neg(result);
    }


    void fp_decimal_calculator::setExp10(const int exp10) {
        const int exp2 = exp10ToExp2(exp10);
        const int d_exp2 = this->exp2 - exp2;
        if (d_exp2 < 0) {
            mpz_div_2exp(value, value, -d_exp2);
        }
        if (d_exp2 > 0) {
            mpz_mul_2exp(value, value, d_exp2);
        }
        this->exp2 = exp2;
    }
}