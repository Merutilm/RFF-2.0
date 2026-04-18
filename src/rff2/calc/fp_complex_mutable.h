#pragma once

#include <array>

#include "dex.h"
#include "fp_decimal_mutable.h"

namespace merutilm::rff2 {
    class fp_complex_mutable {
        fp_decimal_mutable real;
        fp_decimal_mutable imag;
        std::array<fp_decimal_mutable, 4> temp;

    public:
        explicit fp_complex_mutable(const std::string &re, const std::string &im, int exp10);

        explicit fp_complex_mutable(const fp_decimal_mutable &re, const fp_decimal_mutable &im, int exp10);

        explicit fp_complex_mutable(double re, double im, int exp10);

        explicit fp_complex_mutable(const dex &re, const dex &im, int exp10);

        ~fp_complex_mutable();

        fp_complex_mutable(const fp_complex_mutable &other);

        fp_complex_mutable &operator=(const fp_complex_mutable &other);

        fp_complex_mutable(fp_complex_mutable &&other) noexcept;

        fp_complex_mutable &operator=(fp_complex_mutable &&other) noexcept;

    private:
        static void fpc_add(fp_complex_mutable &a, const fp_complex_mutable &b);

        static void fpc_sub(fp_complex_mutable &a, const fp_complex_mutable &b);

        static void fpc_mul(fp_complex_mutable &a, const fp_complex_mutable &b);

        static void fpc_div(fp_complex_mutable &a, const fp_complex_mutable &b);

        static void fpc_square(fp_complex_mutable &a);

        static void fpc_doubled(fp_complex_mutable &a);

        static void fpc_halved(fp_complex_mutable &a);

    public:
        static fp_complex_mutable init(const std::string &re, const std::string &im, int precision);


        friend fp_complex_mutable &operator+=(fp_complex_mutable &a, const fp_complex_mutable &b) {
            fpc_add(a, b);
            return a;
        }

        friend fp_complex_mutable &operator*=(fp_complex_mutable &a, const fp_complex_mutable &b) {
            fpc_mul(a, b);
            return a;
        }

        friend fp_complex_mutable &operator-=(fp_complex_mutable &a, const fp_complex_mutable &b) {
            fpc_sub(a, b);
            return a;
        }

        friend fp_complex_mutable &operator/=(fp_complex_mutable &a, const fp_complex_mutable &b) {
            fpc_div(a, b);
            return a;
        }

        fp_complex_mutable &square();

        fp_complex_mutable &doubled();

        fp_complex_mutable &halved();

        fp_complex_mutable &negate();

        fp_decimal_mutable &get_real();

        fp_decimal_mutable &get_imag();

        fp_decimal_mutable clone_real() const;

        fp_decimal_mutable clone_imag() const;

        void set_exp10(int exp10);
    };

    //DEFINITION OF FP_COMPLEX_MUTABLE

    inline fp_complex_mutable::fp_complex_mutable(const std::string &re, const std::string &im, const int exp10) {
        this->real = fp_decimal_mutable(re, exp10);
        this->imag = fp_decimal_mutable(im, exp10);

        for (auto &temp1: temp) {
            temp1 = fp_decimal_mutable("0", exp10);
        }
    }

    inline fp_complex_mutable::fp_complex_mutable(const fp_decimal_mutable &re, const fp_decimal_mutable &im,
                                                  const int exp10) {
        this->real = re;
        this->imag = im;
        set_exp10(exp10);
        for (auto &temp1: temp) {
            temp1 = fp_decimal_mutable("0", exp10);
        }
    }

    inline fp_complex_mutable::fp_complex_mutable(const double re, const double im, const int exp10) {
        this->real = fp_decimal_mutable(re, exp10);
        this->imag = fp_decimal_mutable(im, exp10);
        for (auto &temp1: temp) {
            temp1 = fp_decimal_mutable("0", exp10);
        }
    }

    inline fp_complex_mutable::fp_complex_mutable(const dex &re, const dex &im, const int exp10) {
        this->real = fp_decimal_mutable(re, exp10);
        this->imag = fp_decimal_mutable(im, exp10);
        for (auto &temp1: temp) {
            temp1 = fp_decimal_mutable("0", exp10);
        }
    }


    inline fp_complex_mutable::~fp_complex_mutable() = default;


    inline fp_complex_mutable::fp_complex_mutable(const fp_complex_mutable &other) {
        real = fp_decimal_mutable(other.real);
        imag = fp_decimal_mutable(other.imag);
        for (auto &temp1: temp) {
            temp1 = fp_decimal_mutable(other.temp[0]);
        }
    }

    inline fp_complex_mutable &fp_complex_mutable::operator=(const fp_complex_mutable &other) {
        real = other.real;
        imag = other.imag;
        for (auto &temp1: temp) {
            temp1 = fp_decimal_mutable(other.temp[0]);
        }
        return *this;
    }

    inline fp_complex_mutable::fp_complex_mutable(fp_complex_mutable &&other) noexcept {
        real = std::move(other.real);
        imag = std::move(other.imag);
        temp = std::move(other.temp);
    }

    inline fp_complex_mutable &fp_complex_mutable::operator=(fp_complex_mutable &&other) noexcept {
        if (this != &other) {
            real = std::move(other.real);
            imag = std::move(other.imag);
            temp = std::move(other.temp);
        }
        return *this;
    }


    inline fp_complex_mutable fp_complex_mutable::init(const std::string &re, const std::string &im,
                                                       const int precision) {
        return fp_complex_mutable(re, im, precision);
    }

    inline void fp_complex_mutable::fpc_add(fp_complex_mutable &a, const fp_complex_mutable &b) {
        fp_decimal_mutable::fp_add(a.real, a.real, b.real);
        fp_decimal_mutable::fp_add(a.imag, a.imag, b.imag);
    }


    inline void fp_complex_mutable::fpc_sub(fp_complex_mutable &a, const fp_complex_mutable &b) {
        fp_decimal_mutable::fp_sub(a.real, a.real, b.real);
        fp_decimal_mutable::fp_sub(a.imag, a.imag, b.imag);
    }


    inline void fp_complex_mutable::fpc_mul(fp_complex_mutable &a, const fp_complex_mutable &b) {
        //(a+bi)*(c+di)
        //REAL : ac-bd
        //IMAG : ad+bc

        fp_decimal_mutable::fp_sub(a.temp[0], a.real, a.imag);
        fp_decimal_mutable::fp_add(a.temp[1], b.real, b.imag);
        fp_decimal_mutable::fp_mul(a.temp[0], a.temp[0], a.temp[1]);
        fp_decimal_mutable::fp_mul(a.temp[1], a.real, b.imag);
        fp_decimal_mutable::fp_mul(a.temp[2], a.imag, b.real);
        fp_decimal_mutable::fp_sub(a.real, a.temp[0], a.temp[1]);
        fp_decimal_mutable::fp_add(a.real, a.real, a.temp[2]);
        fp_decimal_mutable::fp_add(a.imag, a.temp[1], a.temp[2]);
    }

    inline void fp_complex_mutable::fpc_div(fp_complex_mutable &a, const fp_complex_mutable &b) {
        // (a + bi) / (c + di)
        //REAL : (ac+bd) / (c^2+d^2)
        //IMAG : (bc-ad) / (c^2+d^2)

        fp_decimal_mutable::fp_mul(a.temp[0], b.real, b.real);
        fp_decimal_mutable::fp_mul(a.temp[1], b.imag, b.imag);
        fp_decimal_mutable::fp_add(a.temp[0], a.temp[0], a.temp[1]);

        fp_decimal_mutable::fp_mul(a.temp[1], a.real, b.real);
        fp_decimal_mutable::fp_mul(a.temp[2], a.imag, b.imag);
        fp_decimal_mutable::fp_add(a.temp[1], a.temp[1], a.temp[2]);

        fp_decimal_mutable::fp_mul(a.temp[2], a.imag, b.real);
        fp_decimal_mutable::fp_mul(a.temp[3], a.real, b.imag);
        fp_decimal_mutable::fp_sub(a.temp[2], a.temp[2], a.temp[3]);

        fp_decimal_mutable::fp_div(a.real, a.temp[1], a.temp[0]);
        fp_decimal_mutable::fp_div(a.imag, a.temp[2], a.temp[0]);

    }

    inline void fp_complex_mutable::fpc_square(fp_complex_mutable &a) {
        //(a+bi)^2
        //REAL : a^2-b^2 = (a+b)(a-b)
        //IMAG : 2ab
        fp_decimal_mutable::fp_add(a.temp[0], a.real, a.imag);
        fp_decimal_mutable::fp_sub(a.temp[1], a.real, a.imag);
        fp_decimal_mutable::fp_mul(a.temp[2], a.real, a.imag);
        fp_decimal_mutable::fp_mul(a.real, a.temp[0], a.temp[1]);
        fp_decimal_mutable::fp_dbl(a.imag, a.temp[2]);
    }

    inline void fp_complex_mutable::fpc_doubled(fp_complex_mutable &a) {
        fp_decimal_mutable::fp_dbl(a.real, a.real);
        fp_decimal_mutable::fp_dbl(a.imag, a.imag);
    }

    inline void fp_complex_mutable::fpc_halved(fp_complex_mutable &a) {
        fp_decimal_mutable::fp_hlv(a.real, a.real);
        fp_decimal_mutable::fp_hlv(a.imag, a.imag);
    }


    inline fp_complex_mutable &fp_complex_mutable::square() {
        fpc_square(*this);
        return *this;
    }

    inline fp_complex_mutable &fp_complex_mutable::doubled() {
        fpc_doubled(*this);
        return *this;
    }

    inline fp_complex_mutable &fp_complex_mutable::halved() {
        fpc_halved(*this);
        return *this;
    }

    inline fp_complex_mutable &fp_complex_mutable::negate() {
        fp_decimal_mutable::negate(real);
        fp_decimal_mutable::negate(imag);
        return *this;
    }

    inline fp_decimal_mutable &fp_complex_mutable::get_real() {
        return real;
    }

    inline fp_decimal_mutable &fp_complex_mutable::get_imag() {
        return imag;
    }

    inline fp_decimal_mutable fp_complex_mutable::clone_real() const {
        return real;
    }

    inline fp_decimal_mutable fp_complex_mutable::clone_imag() const {
        return imag;
    }


    inline void fp_complex_mutable::set_exp10(const int exp10) {
        real.setExp10(exp10);
        imag.setExp10(exp10);
        for (auto &temp1: temp) {
            temp1.setExp10(exp10);
        }
    }
}
