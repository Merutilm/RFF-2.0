//
// Created by Merutilm on 2025-05-05.
//

#pragma once
#include "gmp.h"
#include "fp_decimal_mutable.h"

namespace merutilm::rff2 {
    class fp_decimal {
        int exp2;
        mpz_t value;

    public:

        fp_decimal(const std::string &value, int exp10);

        explicit fp_decimal(const fp_decimal_mutable &calc);

        ~fp_decimal();

        fp_decimal(const fp_decimal& other);

        fp_decimal& operator=(const fp_decimal& other);

        fp_decimal(fp_decimal&& other) noexcept;

        fp_decimal& operator=(fp_decimal&& other) noexcept;

        std::string to_string() const;

        int get_exp2() const;

        bool is_positive() const;

        bool is_zero() const;

        bool is_negative() const;

        fp_decimal_mutable edit() const;

    };

    //DEFINITION OF FP_DECIMAL

    inline fp_decimal::fp_decimal(const std::string& value, const int exp10) : fp_decimal(fp_decimal_mutable(value, exp10)){};


    inline fp_decimal::fp_decimal(const fp_decimal_mutable &calc) {
        this->exp2 = calc.exp2;
        mpz_init(this->value);
        mpz_set(this->value, calc.value);
    }


    inline fp_decimal::~fp_decimal() {

        mpz_clear(this->value);
    }


    inline fp_decimal::fp_decimal(const fp_decimal &other) {
        this->exp2 = other.exp2;
        mpz_init(this->value);
        mpz_set(this->value, other.value);
    }

    inline fp_decimal &fp_decimal::operator=(const fp_decimal &other) {
        this->exp2 = other.exp2;
        mpz_set(this->value, other.value);
        return *this;
    }

    inline fp_decimal::fp_decimal(fp_decimal &&other) noexcept {
        this->exp2 = other.exp2;
        mpz_init(this->value);
        mpz_swap(this->value, other.value);
        mpz_set_ui(other.value, 0);
    }

    inline fp_decimal &fp_decimal::operator=(fp_decimal &&other) noexcept {
        if (this != &other) {
            this->exp2 = other.exp2;
            mpz_swap(this->value, other.value);
            mpz_set_ui(other.value, 0);
        }
        return *this;
    }

    inline std::string fp_decimal::to_string() const{
        mpf_t d;

        if(exp2 < 0){
            mpf_init2(d, -exp2);
            mpf_set_z(d, value);
            mpf_div_2exp(d, d, -exp2);

        }else{

            mpf_init2(d, exp2);
            mpf_set_z(d, value);
            mpf_mul_2exp(d, d, exp2);
        }
        char* str;
        gmp_asprintf(&str, "%.Ff", d);
        std::string result(str);

        //gmp_asprinf uses malloc(), Do not remove this
        free(str);
        mpf_clear(d);
        return result;
    }

    inline int fp_decimal::get_exp2() const {
        return exp2;
    }

    inline bool fp_decimal::is_positive() const {
        return mpz_sgn(value) == 1;
    }

    inline bool fp_decimal::is_zero() const {
        return mpz_sgn(value) == 0;
    }

    inline bool fp_decimal::is_negative() const {
        return mpz_sgn(value) == -1;
    }

    inline fp_decimal_mutable fp_decimal::edit() const{
        return fp_decimal_mutable(value, exp2);
    }
}