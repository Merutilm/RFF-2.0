//
// Created by Merutilm on 2025-05-17.
//

#include "double_exp.h"

#include <cmath>

#include "../ui/RFF.h"

double_exp::double_exp() : double_exp(0, 0) {
}


double_exp::double_exp(const int exp2, const double mantissa) : exp2(exp2), mantissa(mantissa) {
}


void double_exp::dex_add(double_exp *result, const double_exp &a, const double_exp &b) {
    const int d_exp2 = a.exp2 - b.exp2;
    const double mantissa = ldexp(a.mantissa, std::min(0, d_exp2)) + ldexp(b.mantissa, std::min(0, -d_exp2));
    result->exp2 = std::max(a.exp2, b.exp2);
    result->mantissa = mantissa;
    fix_value(result);
}

void double_exp::dex_sub(double_exp *result, const double_exp &a, const double_exp &b) {
    const int d_exp2 = a.exp2 - b.exp2;
    const double mantissa = ldexp(a.mantissa, std::min(0, d_exp2)) - ldexp(b.mantissa, std::min(0, -d_exp2));
    result->exp2 = std::max(a.exp2, b.exp2);
    result->mantissa = mantissa;
    fix_value(result);
}

void double_exp::dex_mul(double_exp *result, const double_exp &a, const double_exp &b) {
    result->exp2 = a.exp2 + b.exp2;
    result->mantissa = a.mantissa * b.mantissa;
    fix_value(result);
}

void double_exp::dex_div(double_exp *result, const double_exp &a, const double_exp &b) {
    result->exp2 = a.exp2 - b.exp2;
    result->mantissa = a.mantissa / b.mantissa;
    fix_value(result);
}

void double_exp::dex_sqr(double_exp *result, const double_exp &a) {
    result->exp2 = a.exp2 << 1;
    result->mantissa = a.mantissa * a.mantissa;
    fix_value(result);
}


void double_exp::dex_sqrt(double_exp *result, const double_exp &a) {
    result->exp2 = a.exp2 >> 1;
    result->mantissa = a.sgn() * sqrt(std::abs(a.mantissa));
    fix_value(result);
}

void double_exp::dex_mul_2exp(double_exp *result, const double_exp &v, const int exp2) {
    result->exp2 = v.exp2 + exp2;
    result->mantissa = v.mantissa;
}

void double_exp::dex_div_2exp(double_exp *result, const double_exp &v, const int exp2) {
    result->exp2 = v.exp2 - exp2;
    result->mantissa = v.mantissa;
}

void double_exp::dex_cpy(double_exp *result, const double v) {
    result->exp2 = 0;
    result->mantissa = v;
    fix_value(result);
}


void double_exp::dex_cpy(double_exp *result, const double_exp &v) {
    result->exp2 = v.exp2;
    result->mantissa = v.mantissa;
}


double_exp operator+(const double_exp &a, const double_exp &b) {
    if (a.isnan() || b.isnan()) {
        return RFF::Precision::DEX_NAN;
    }
    if (a.isinf() && b.isinf()) {
        if (a.sgn() == b.sgn()) {
            return a;
        }
        return RFF::Precision::DEX_NAN;
    }
    if (a.isinf() || b.is_zero()) {
        return a;
    }
    if (b.isinf() || a.is_zero()) {
        return b;
    }
    auto result = double_exp(0, 0);
    double_exp::dex_add(&result, a, b);
    return result;
}

double_exp operator+(const double_exp &a, const double b) {
    return a + double_exp::value(b);
}

double_exp operator+(const double a, const double_exp &b) {
    return double_exp::value(a) + b;
}

double_exp operator-(const double_exp &a, const double_exp &b) {
    if (a.isnan() || b.isnan()) {
        return RFF::Precision::DEX_NAN;
    }
    if (a.isinf() && b.isinf()) {
        if (a.sgn() == b.sgn()) {
            return RFF::Precision::DEX_NAN;
        }
        return a;
    }
    if (a.isinf() || b.is_zero()) {
        return a;
    }
    if (b.isinf() || a.is_zero()) {
        return -b;
    }
    auto result = double_exp(0, 0);
    double_exp::dex_sub(&result, a, b);
    return result;
}

double_exp operator-(const double_exp &a) {
    return double_exp(a.exp2, -a.mantissa);
}

double_exp operator-(const double_exp &a, const double b) {
    return a - double_exp::value(b);
}

double_exp operator-(const double a, const double_exp &b) {
    return double_exp::value(a) - b;
}

double_exp operator*(const double_exp &a, const double_exp &b) {
    if (a.is_zero() || b.is_zero()) {
        return RFF::Precision::DEX_ZERO;
    }
    if (a.isnan() || b.isnan()) {
        return RFF::Precision::DEX_NAN;
    }
    if (a.isinf() || b.isinf()) {
        return a.sgn() == b.sgn() ? RFF::Precision::DEX_POSITIVE_INFINITY : RFF::Precision::DEX_NEGATIVE_INFINITY;
    }
    auto result = double_exp(0, 0);
    double_exp::dex_mul(&result, a, b);
    return result;
}


double_exp operator*(const double_exp &a, const double b) {
    return a * double_exp::value(b);
}

double_exp operator*(const double a, const double_exp &b) {
    return double_exp::value(a) * b;
}

double_exp operator/(const double_exp &a, const double_exp &b) {
    if (a.is_zero() || b.isinf()) {
        return RFF::Precision::DEX_ZERO;
    }
    if (b.is_zero() || a.isinf()) {
        return a.sgn() == 1 ? RFF::Precision::DEX_POSITIVE_INFINITY : RFF::Precision::DEX_NEGATIVE_INFINITY;
    }
    if (a.is_zero() && b.is_zero()) {
        return RFF::Precision::DEX_NAN;
    }
    auto result = double_exp(0, 0);
    double_exp::dex_div(&result, a, b);
    return result;
}

double_exp operator/(const double_exp &a, const double b) {
    return a / double_exp::value(b);
}

double_exp operator/(const double a, const double_exp &b) {
    return double_exp::value(a) / b;
}

double_exp &double_exp::operator+=(const double_exp &other) {
    dex_add(this, *this, other);
    return *this;
}

double_exp &double_exp::operator+=(const double other) {
    return *this += value(other);
}

double_exp &double_exp::operator-=(const double_exp &other) {
    dex_sub(this, *this, other);
    return *this;
}

double_exp &double_exp::operator-=(const double other) {
    return *this -= value(other);
}

double_exp &double_exp::operator*=(const double_exp &other) {
    dex_mul(this, *this, other);
    return *this;
}

double_exp &double_exp::operator*=(const double other) {
    return *this *= value(other);
}

double_exp &double_exp::operator/=(const double_exp &other) {
    dex_div(this, *this, other);
    return *this;
}

double_exp &double_exp::operator/=(const double other) {
    return *this /= value(other);
}

std::partial_ordering double_exp::operator<=>(const double_exp &other) const {
    const double_exp v = *this - other;
    if (v.isnan()) {
        return std::partial_ordering::unordered;
    }
    return v.sgn() <=> 0;
}

std::partial_ordering double_exp::operator<=>(const double other) const {
    return *this <=> value(other);
}

double_exp double_exp::value(const double value) {
    if (value == 0) {
        return RFF::Precision::DEX_ZERO;
    }
    if (std::isinf(value)) {
        return value > 0 ? RFF::Precision::DEX_POSITIVE_INFINITY : RFF::Precision::DEX_NEGATIVE_INFINITY;
    }
    if (std::isnan(value)) {
        return RFF::Precision::DEX_NAN;
    }

    auto result = double_exp(0, 0);
    dex_cpy(&result, value);
    return result;
}

void double_exp::fix_value(double_exp *result) {
    int dex;
    result->mantissa = std::frexp(result->mantissa, &dex);
    result->exp2 += dex;
}

char double_exp::sgn() const {
    return static_cast<char>(0 < mantissa) - static_cast<char>(mantissa < 0);
}

bool double_exp::isinf() const {
    return RFF::Precision::DEX_POSITIVE_INFINITY == *this || RFF::Precision::DEX_NEGATIVE_INFINITY == *this;
}

bool double_exp::isnan() const {
    return std::isnan(mantissa);
}

bool double_exp::is_zero() const {
    return RFF::Precision::DEX_ZERO == *this;
}


double_exp::operator double() const {
    return ldexp(mantissa, exp2);
}


std::string double_exp::to_string() const {
    // m * 2^n
    // = m * 10^(log10(2) * n)
    // = exp10 = log10(2) * n
    if (isnan()) {
        return "nan";
    }
    if (isinf()) {
        return sgn() > 0 ? "inf" : "-inf";
    }
    const double raw_exp10 = RFF::Precision::LOG10_2 * exp2;
    auto exp10 = static_cast<int>(raw_exp10);
    double mantissa10 = mantissa * std::pow(10, raw_exp10 - exp10);

    while (std::abs(mantissa10) < 1) {
        mantissa10 *= 10;
        exp10 -= 1;
    }
    return std::format("{}e{}", mantissa10, exp10);
}


int double_exp::get_exp2() const {
    return exp2;
}

double double_exp::get_mantissa() const {
    return mantissa;
}
