#include "GMPComplexCalculator.h"

#include <iostream>
#include <memory>

#include "Center.h"


GMPComplexCalculator::GMPComplexCalculator(const std::string &re, const std::string &im, const int exp10) {
    this->real = GMPDecimalCalculator(re, exp10);
    this->imag = GMPDecimalCalculator(im, exp10);

    for (auto &temp1: temp) {
        temp1 = GMPDecimalCalculator("0", exp10);
    }
}
GMPComplexCalculator::GMPComplexCalculator(const GMPDecimalCalculator &re, const GMPDecimalCalculator &im, const int exp10) {
    this->real = re;
    this->imag = im;
    setExp10(exp10);
    for (auto &temp1: temp) {
        temp1 = GMPDecimalCalculator("0", exp10);
    }
}

GMPComplexCalculator::GMPComplexCalculator(const double re, const double im, const int exp10) {
    this->real = GMPDecimalCalculator(re, exp10);
    this->imag = GMPDecimalCalculator(im, exp10);
    for (auto &temp1: temp) {
        temp1 = GMPDecimalCalculator("0", exp10);
    }
}


GMPComplexCalculator::~GMPComplexCalculator() = default;


GMPComplexCalculator::GMPComplexCalculator(const GMPComplexCalculator &other) {
    real = GMPDecimalCalculator(other.real);
    imag = GMPDecimalCalculator(other.imag);
    for (auto &temp1: temp) {
        temp1 = GMPDecimalCalculator(other.temp[0]);
    }
}

GMPComplexCalculator &GMPComplexCalculator::operator=(const GMPComplexCalculator &other) {
    real = other.real;
    imag = other.imag;
    for (auto &temp1: temp) {
        temp1 = GMPDecimalCalculator(other.temp[0]);
    }
    return *this;
}

GMPComplexCalculator::GMPComplexCalculator(GMPComplexCalculator &&other) noexcept {
    real = std::move(other.real);
    imag = std::move(other.imag);
    temp = std::move(other.temp);
}

GMPComplexCalculator &GMPComplexCalculator::operator=(GMPComplexCalculator &&other) noexcept {
    if (this != &other) {
        real = std::move(other.real);
        imag = std::move(other.imag);
        temp = std::move(other.temp);
    }
    return *this;
}


GMPComplexCalculator GMPComplexCalculator::init(const std::string &re, const std::string &im, const int precision) {
    return GMPComplexCalculator(re, im, precision);
}

void GMPComplexCalculator::mpc_add(GMPComplexCalculator &a, const GMPComplexCalculator &b) {
    GMPDecimalCalculator::fst_add(a.real, a.real, b.real);
    GMPDecimalCalculator::fst_add(a.imag, a.imag, b.imag);
}


void GMPComplexCalculator::mpc_sub(GMPComplexCalculator &a, const GMPComplexCalculator &b) {
    GMPDecimalCalculator::fst_sub(a.real, a.real, b.real);
    GMPDecimalCalculator::fst_sub(a.imag, a.imag, b.imag);
}


void GMPComplexCalculator::mpc_mul(GMPComplexCalculator &a, const GMPComplexCalculator &b) {
    //(a+bi)*(c+di)
    //REAL : ac-bd
    //IMAG : ad+bc

    GMPDecimalCalculator::fst_mul(a.temp[0], a.real, b.real);
    GMPDecimalCalculator::fst_mul(a.temp[1], a.imag, b.imag);
    GMPDecimalCalculator::fst_sub(a.temp[2], a.temp[0], a.temp[1]);


    GMPDecimalCalculator::fst_mul(a.temp[0], a.real, b.imag);
    GMPDecimalCalculator::fst_mul(a.temp[1], a.imag, b.real);
    GMPDecimalCalculator::fst_add(a.temp[3], a.temp[0], a.temp[1]);

    GMPDecimalCalculator::fst_swap(a.real, a.temp[2]);
    GMPDecimalCalculator::fst_swap(a.imag, a.temp[3]);
}

void GMPComplexCalculator::mpc_div(GMPComplexCalculator &a, const GMPComplexCalculator &b) {
    // (a + bi) / (c + di)
    //REAL : (ac+bd) / (c^2+d^2)
    //IMAG : (bc-ad) / (c^2+d^2)

    GMPDecimalCalculator::fst_mul(a.temp[0], b.real, b.real);
    GMPDecimalCalculator::fst_mul(a.temp[1], b.imag, b.imag);
    GMPDecimalCalculator::fst_add(a.temp[0], a.temp[0], a.temp[1]);

    GMPDecimalCalculator::fst_mul(a.temp[1], a.real, b.real);
    GMPDecimalCalculator::fst_mul(a.temp[2], a.imag, b.imag);
    GMPDecimalCalculator::fst_add(a.temp[1], a.temp[1], a.temp[2]);
    GMPDecimalCalculator::fst_div(a.temp[1], a.temp[1], a.temp[0]);

    GMPDecimalCalculator::fst_mul(a.temp[2], a.imag, b.real);
    GMPDecimalCalculator::fst_mul(a.temp[3], a.real, b.imag);
    GMPDecimalCalculator::fst_sub(a.temp[2], a.temp[2], a.temp[3]);
    GMPDecimalCalculator::fst_div(a.temp[2], a.temp[2], a.temp[0]);

    GMPDecimalCalculator::fst_swap(a.real, a.temp[1]);
    GMPDecimalCalculator::fst_swap(a.imag, a.temp[2]);
}

void GMPComplexCalculator::mpc_square(GMPComplexCalculator &a) {
    //(a+bi)^2
    //REAL : a^2-b^2 = (a+b)(a-b)
    //IMAG : 2ab
    GMPDecimalCalculator::fst_add(a.temp[0], a.real, a.imag);
    GMPDecimalCalculator::fst_sub(a.temp[1], a.real, a.imag);
    GMPDecimalCalculator::fst_mul(a.temp[2], a.temp[0], a.temp[1]);
    GMPDecimalCalculator::fst_mul(a.temp[3], a.real, a.imag);
    GMPDecimalCalculator::fst_swap(a.real, a.temp[2]);
    GMPDecimalCalculator::fst_dbl(a.imag, a.temp[3]);
}

void GMPComplexCalculator::mpc_doubled(GMPComplexCalculator &a) {
    GMPDecimalCalculator::fst_dbl(a.real, a.real);
    GMPDecimalCalculator::fst_dbl(a.imag, a.imag);
}


GMPComplexCalculator &GMPComplexCalculator::operator+=(const GMPComplexCalculator &b) {
    mpc_add(*this, b);
    return *this;
}

GMPComplexCalculator &GMPComplexCalculator::operator*=(const GMPComplexCalculator &b) {
    mpc_mul(*this, b);
    return *this;
}

GMPComplexCalculator &GMPComplexCalculator::operator-=(const GMPComplexCalculator &b) {
    mpc_sub(*this, b);
    return *this;
}

GMPComplexCalculator &GMPComplexCalculator::operator/=(const GMPComplexCalculator &b) {
    mpc_div(*this, b);
    return *this;
}

GMPComplexCalculator &GMPComplexCalculator::square() {
    mpc_square(*this);
    return *this;
}

GMPComplexCalculator &GMPComplexCalculator::doubled() {
    mpc_doubled(*this);
    return *this;
}

GMPDecimalCalculator &GMPComplexCalculator::getReal() {
    return real;
}

GMPDecimalCalculator &GMPComplexCalculator::getImag() {
    return imag;
}

GMPDecimalCalculator GMPComplexCalculator::getRealClone() const {
    return real;
}

GMPDecimalCalculator GMPComplexCalculator::getImagClone() const {
    return imag;
}

void GMPComplexCalculator::setExp10(const int exp10) {
    real.setExp10(exp10);
    imag.setExp10(exp10);
    for (auto &temp1: temp) {
        temp1.setExp10(exp10);
    }
}
