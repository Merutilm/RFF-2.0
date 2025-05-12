//
// Created by Merutilm on 2025-05-05.
//

#include "GMPDecimalCalculator.h"

#include <float.h>
#include <iostream>
#include <math.h>

#include "GMPComplexCalculator.h"
#include "../ui/RFFConstants.h"


GMPDecimalCalculator::GMPDecimalCalculator() {
    exp2 = 0;
    mpz_init(value);
    mpz_init(temp);
}

GMPDecimalCalculator::GMPDecimalCalculator(const mpz_srcptr value, const int exp2) {
    mpz_init(this->value);
    mpz_init(temp);
    mpz_set(this->value, value);
    this->exp2 = exp2;
}



GMPDecimalCalculator::GMPDecimalCalculator(const double d, const int exp10) {
    mpz_init(value);
    mpz_init(temp);

    mpf_t d1;
    exp2 = exp10ToExp2(exp10);

    mpf_init2(d1, -exp2);
    mpf_set_d(d1, d);

    if(exp2 < 0){
        mpf_mul_2exp(d1, d1, -exp2);
    }else{
        mpf_div_2exp(d1, d1, exp2);
    }

    mpz_set_f(value, d1);
    mpf_clear(d1);
}

GMPDecimalCalculator::GMPDecimalCalculator(const std::string& str, const int exp10) {

    mpz_init(value);
    mpz_init(temp);

    mpf_t d1;
    exp2 = exp10ToExp2(exp10);


    mpf_init2(d1, -exp2);
    mpf_set_str(d1, str.c_str(), 10);

    if(exp2 < 0){
        mpf_mul_2exp(d1, d1, -exp2);
    }else{
        mpf_div_2exp(d1, d1, exp2);
    }

    mpz_set_f(value, d1);
    mpf_clear(d1);
}




GMPDecimalCalculator::~GMPDecimalCalculator() {
    mpz_clear(value);
    mpz_clear(temp);
}


GMPDecimalCalculator::GMPDecimalCalculator(const GMPDecimalCalculator &other) {
    mpz_init_set(temp, other.temp);
    mpz_init_set(value, other.value);
    exp2 = other.exp2;
}

GMPDecimalCalculator &GMPDecimalCalculator::operator=(const GMPDecimalCalculator &other) {
    if (this != &other) {
        mpz_set(value, other.value);
        exp2 = other.exp2;
    }
    return *this;
}


GMPDecimalCalculator::GMPDecimalCalculator(GMPDecimalCalculator &&other) noexcept {
    mpz_init(value);
    mpz_init(temp);

    mpz_swap(value, other.value);
    mpz_swap(temp, other.temp);

    exp2 = other.exp2;
}

GMPDecimalCalculator &GMPDecimalCalculator::operator=(GMPDecimalCalculator &&other) noexcept {
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
void GMPDecimalCalculator::fst_add(GMPDecimalCalculator &out, const GMPDecimalCalculator &a, const GMPDecimalCalculator &b) {
    mpz_add(out.value, a.value, b.value);
}

/**
 * <b> A - B </b> <br/><br/>
 * Fast subtraction. It assumes that the exponents of both numbers are the same.
 * @param out the result
 * @param a input A
 * @param b input B
 */
void GMPDecimalCalculator::fst_sub(GMPDecimalCalculator &out, const GMPDecimalCalculator &a, const GMPDecimalCalculator &b) {
    mpz_sub(out.value, a.value, b.value);
}

/**
 * <b> A * B </b> <br/><br/>
 * Fast multiplication. It assumes that the exponents of both numbers are the same.
 * @param out the result
 * @param a input A
 * @param b input B
 */
void GMPDecimalCalculator::fst_mul(GMPDecimalCalculator &out, const GMPDecimalCalculator &a, const GMPDecimalCalculator &b) {
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
void GMPDecimalCalculator::fst_div(GMPDecimalCalculator &out, const GMPDecimalCalculator &a, const GMPDecimalCalculator &b) {
    const auto vbl = static_cast<int>(mpz_sizeinbase(b.value, 2));
    const int e = b.exp2 + vbl;
    mpz_mul_2exp(out.temp, a.value, vbl);
    mpz_div(out.temp, out.temp, b.value);
    if (e < 0) {
        mpz_mul_2exp(out.value, out.temp, e);
    }else {
        mpz_div_2exp(out.value, out.temp, -e);
    }
}
/**
 * Doubles value. It assumes that the exponents of both numbers are the same.
 * @param out the result
 * @param target input B
 */
void GMPDecimalCalculator::fst_dbl(GMPDecimalCalculator &out, const GMPDecimalCalculator &target) {
    mpz_mul_2exp(out.value, target.value, 1);
}

/**
 * Do swap. It assumes that the exponents of both numbers are the same.
 * @param a input A
 * @param b input B
 */
void GMPDecimalCalculator::fst_swap(GMPDecimalCalculator &a, GMPDecimalCalculator &b) {
    mpz_swap(a.value, b.value);
}


int GMPDecimalCalculator::exp2ToExp10(const int exp2) {
    return static_cast<int>(static_cast<float>(exp2) * RFFConstants::Precision::LOG10_2);
}

int GMPDecimalCalculator::exp10ToExp2(const int precision){
    return static_cast<int>(static_cast<float>(precision) / RFFConstants::Precision::LOG10_2);
}



double GMPDecimalCalculator::doubleValue() {
    const int signum = mpz_sgn(value);
    if(signum == 0){
        return 0;
    }

    mpz_abs(temp, value);
    const size_t len = mpz_sizeinbase(temp, 2);
    const auto shift = static_cast<int>(len - RFFConstants::Precision::DOUBLE_PRECISION - 1);
    if(shift < 0) {
        mpz_mul_2exp(temp, temp, -shift);
    }else {
        mpz_div_2exp(temp, temp, shift);
    }
    uint64_t mantissa;
    size_t cnt;
    mpz_export(&mantissa, &cnt, -1, sizeof(mantissa), 0, 0, temp);


    const int fExp2 = exp2 + shift + RFFConstants::Precision::DOUBLE_PRECISION;
    //0100 0000 0000 : 2^1
    //0000 0000 0000 : 2^-1023
    //0111 1111 1111 : 2^1024
    if(fExp2 > 0x03ff){
        return signum == 1 ? INFINITY : -INFINITY;
    }
    const int mantissaShift = fExp2 <= -0x03ff ? -0x03ff - fExp2 + 1 : 0;
    mantissa = (mantissa >> mantissaShift) & RFFConstants::Precision::DECIMAL_SIGNUM_BITS;

    const uint64_t exponent =  fExp2 <= -0x03ff ? 0 : RFFConstants::Precision::EXP0_BITS + (static_cast<uint64_t>(fExp2) << RFFConstants::Precision::DOUBLE_PRECISION);
    const uint64_t sig = signum == 1 ? 0 : RFFConstants::Precision::SIGNUM_BIT;
    return std::bit_cast<double>(sig | exponent | mantissa);
}

void GMPDecimalCalculator::setExp10(const int exp10) {
    const int exp2 = exp10ToExp2(exp10);
    const int dExp2 = this->exp2 - exp2;
    if (dExp2 < 0) {
        mpz_div_2exp(value, value, -dExp2);
    }
    if (dExp2 > 0){
        mpz_mul_2exp(value, value, dExp2);
    }
    this->exp2 = exp2;
}