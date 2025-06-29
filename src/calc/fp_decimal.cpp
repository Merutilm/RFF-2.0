//
// Created by Merutilm on 2025-05-05.
//

#include "fp_decimal.h"

#include <string>
#include "fp_decimal_calculator.h"


namespace merutilm::rff2 {
    fp_decimal::fp_decimal(const std::string& value, const int exp10) : fp_decimal(fp_decimal_calculator(value, exp10)){};


    fp_decimal::fp_decimal(const fp_decimal_calculator &calc) {
        this->exp2 = calc.exp2;
        mpz_init(this->value);
        mpz_set(this->value, calc.value);
    }


    fp_decimal::~fp_decimal() {

        mpz_clear(this->value);
    }


    fp_decimal::fp_decimal(const fp_decimal &other) {
        this->exp2 = other.exp2;
        mpz_init(this->value);
        mpz_set(this->value, other.value);
    }

    fp_decimal &fp_decimal::operator=(const fp_decimal &other) {
        this->exp2 = other.exp2;
        mpz_set(this->value, other.value);
        return *this;
    }

    fp_decimal::fp_decimal(fp_decimal &&other) noexcept {
        this->exp2 = other.exp2;
        mpz_init(this->value);
        mpz_swap(this->value, other.value);
        mpz_set_ui(other.value, 0);
    }

    fp_decimal &fp_decimal::operator=(fp_decimal &&other) noexcept {
        if (this != &other) {
            this->exp2 = other.exp2;
            mpz_swap(this->value, other.value);
            mpz_set_ui(other.value, 0);
        }
        return *this;
    }

    std::string fp_decimal::to_string() const{
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

    int fp_decimal::get_exp2() const {
        return exp2;
    }

    bool fp_decimal::is_positive() const {
        return mpz_sgn(value) == 1;
    }

    bool fp_decimal::is_zero() const {
        return mpz_sgn(value) == 0;
    }

    bool fp_decimal::is_negative() const {
        return mpz_sgn(value) == -1;
    }

    fp_decimal_calculator fp_decimal::edit() const{
        return fp_decimal_calculator(value, exp2);
    }
}