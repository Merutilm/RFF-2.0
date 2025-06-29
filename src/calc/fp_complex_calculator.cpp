#include "fp_complex_calculator.h"

#include <memory>

#include "fp_complex.h"

namespace merutilm::rff2 {
    fp_complex_calculator::fp_complex_calculator(const std::string &re, const std::string &im, const int exp10) {
        this->real = fp_decimal_calculator(re, exp10);
        this->imag = fp_decimal_calculator(im, exp10);

        for (auto &temp1: temp) {
            temp1 = fp_decimal_calculator("0", exp10);
        }
    }
    fp_complex_calculator::fp_complex_calculator(const fp_decimal_calculator &re, const fp_decimal_calculator &im, const int exp10) {
        this->real = re;
        this->imag = im;
        setExp10(exp10);
        for (auto &temp1: temp) {
            temp1 = fp_decimal_calculator("0", exp10);
        }
    }

    fp_complex_calculator::fp_complex_calculator(const double re, const double im, const int exp10) {
        this->real = fp_decimal_calculator(re, exp10);
        this->imag = fp_decimal_calculator(im, exp10);
        for (auto &temp1: temp) {
            temp1 = fp_decimal_calculator("0", exp10);
        }
    }

    fp_complex_calculator::fp_complex_calculator(const dex &re, const dex &im, const int exp10) {
        this->real = fp_decimal_calculator(re, exp10);
        this->imag = fp_decimal_calculator(im, exp10);
        for (auto &temp1: temp) {
            temp1 = fp_decimal_calculator("0", exp10);
        }
    }


    fp_complex_calculator::~fp_complex_calculator() = default;


    fp_complex_calculator::fp_complex_calculator(const fp_complex_calculator &other) {
        real = fp_decimal_calculator(other.real);
        imag = fp_decimal_calculator(other.imag);
        for (auto &temp1: temp) {
            temp1 = fp_decimal_calculator(other.temp[0]);
        }
    }

    fp_complex_calculator &fp_complex_calculator::operator=(const fp_complex_calculator &other) {
        real = other.real;
        imag = other.imag;
        for (auto &temp1: temp) {
            temp1 = fp_decimal_calculator(other.temp[0]);
        }
        return *this;
    }

    fp_complex_calculator::fp_complex_calculator(fp_complex_calculator &&other) noexcept {
        real = std::move(other.real);
        imag = std::move(other.imag);
        temp = std::move(other.temp);
    }

    fp_complex_calculator &fp_complex_calculator::operator=(fp_complex_calculator &&other) noexcept {
        if (this != &other) {
            real = std::move(other.real);
            imag = std::move(other.imag);
            temp = std::move(other.temp);
        }
        return *this;
    }


    fp_complex_calculator fp_complex_calculator::init(const std::string &re, const std::string &im, const int precision) {
        return fp_complex_calculator(re, im, precision);
    }

    void fp_complex_calculator::fpc_add(fp_complex_calculator &a, const fp_complex_calculator &b) {
        fp_decimal_calculator::fp_add(a.real, a.real, b.real);
        fp_decimal_calculator::fp_add(a.imag, a.imag, b.imag);
    }


    void fp_complex_calculator::fpc_sub(fp_complex_calculator &a, const fp_complex_calculator &b) {
        fp_decimal_calculator::fp_sub(a.real, a.real, b.real);
        fp_decimal_calculator::fp_sub(a.imag, a.imag, b.imag);
    }


    void fp_complex_calculator::fpc_mul(fp_complex_calculator &a, const fp_complex_calculator &b) {
        //(a+bi)*(c+di)
        //REAL : ac-bd
        //IMAG : ad+bc

        fp_decimal_calculator::fp_sub(a.temp[0], a.real, a.imag);
        fp_decimal_calculator::fp_add(a.temp[1], b.real, b.imag);
        fp_decimal_calculator::fp_mul(a.temp[0], a.temp[0], a.temp[1]);
        fp_decimal_calculator::fp_mul(a.temp[1], a.real, b.imag);
        fp_decimal_calculator::fp_mul(a.temp[2], a.imag, b.real);
        fp_decimal_calculator::fp_sub(a.real, a.temp[0], a.temp[1]);
        fp_decimal_calculator::fp_add(a.real, a.real, a.temp[2]);
        fp_decimal_calculator::fp_add(a.imag, a.temp[1], a.temp[2]);

    }

    void fp_complex_calculator::fpc_div(fp_complex_calculator &a, const fp_complex_calculator &b) {
        // (a + bi) / (c + di)
        //REAL : (ac+bd) / (c^2+d^2)
        //IMAG : (bc-ad) / (c^2+d^2)

        fp_decimal_calculator::fp_mul(a.temp[0], b.real, b.real);
        fp_decimal_calculator::fp_mul(a.temp[1], b.imag, b.imag);
        fp_decimal_calculator::fp_add(a.temp[0], a.temp[0], a.temp[1]);

        fp_decimal_calculator::fp_mul(a.temp[1], a.real, b.real);
        fp_decimal_calculator::fp_mul(a.temp[2], a.imag, b.imag);
        fp_decimal_calculator::fp_add(a.temp[1], a.temp[1], a.temp[2]);
        fp_decimal_calculator::fp_div(a.temp[1], a.temp[1], a.temp[0]);

        fp_decimal_calculator::fp_mul(a.temp[2], a.imag, b.real);
        fp_decimal_calculator::fp_mul(a.temp[3], a.real, b.imag);
        fp_decimal_calculator::fp_sub(a.temp[2], a.temp[2], a.temp[3]);
        fp_decimal_calculator::fp_div(a.temp[2], a.temp[2], a.temp[0]);

        fp_decimal_calculator::fp_swap(a.real, a.temp[1]);
        fp_decimal_calculator::fp_swap(a.imag, a.temp[2]);
    }

    void fp_complex_calculator::fpc_square(fp_complex_calculator &a) {
        //(a+bi)^2
        //REAL : a^2-b^2 = (a+b)(a-b)
        //IMAG : 2ab
        fp_decimal_calculator::fp_add(a.temp[0], a.real, a.imag);
        fp_decimal_calculator::fp_sub(a.temp[1], a.real, a.imag);
        fp_decimal_calculator::fp_mul(a.temp[2], a.temp[0], a.temp[1]);
        fp_decimal_calculator::fp_mul(a.temp[3], a.real, a.imag);
        fp_decimal_calculator::fp_swap(a.real, a.temp[2]);
        fp_decimal_calculator::fp_dbl(a.imag, a.temp[3]);
    }

    void fp_complex_calculator::fpc_doubled(fp_complex_calculator &a) {
        fp_decimal_calculator::fp_dbl(a.real, a.real);
        fp_decimal_calculator::fp_dbl(a.imag, a.imag);
    }

    void fp_complex_calculator::fpc_halved(fp_complex_calculator &a) {
        fp_decimal_calculator::fp_hlv(a.real, a.real);
        fp_decimal_calculator::fp_hlv(a.imag, a.imag);
    }



    fp_complex_calculator &fp_complex_calculator::square() {
        fpc_square(*this);
        return *this;
    }

    fp_complex_calculator &fp_complex_calculator::doubled() {
        fpc_doubled(*this);
        return *this;
    }

    fp_complex_calculator &fp_complex_calculator::halved() {
        fpc_halved(*this);
        return *this;
    }

    fp_complex_calculator &fp_complex_calculator::negate() {
        fp_decimal_calculator::negate(real);
        fp_decimal_calculator::negate(imag);
        return *this;
    }

    fp_decimal_calculator &fp_complex_calculator::getReal() {
        return real;
    }

    fp_decimal_calculator &fp_complex_calculator::getImag() {
        return imag;
    }

    fp_decimal_calculator fp_complex_calculator::getRealClone() const {
        return real;
    }

    fp_decimal_calculator fp_complex_calculator::getImagClone() const {
        return imag;
    }


    void fp_complex_calculator::setExp10(const int exp10) {
        real.setExp10(exp10);
        imag.setExp10(exp10);
        for (auto &temp1: temp) {
            temp1.setExp10(exp10);
        }
    }
}