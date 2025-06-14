//
// Created by Merutilm on 2025-05-05.
//

#include "fp_decimal.h"

#include <iostream>
#include <string>
#include "fp_decimal_calculator.h"



merutilm::rff::fp_decimal::fp_decimal(const std::string& value, const int exp10) : fp_decimal(fp_decimal_calculator(value, exp10)){};


merutilm::rff::fp_decimal::fp_decimal(const fp_decimal_calculator &calc) {
    this->exp2 = calc.exp2;
    mpz_init(this->value);
    mpz_set(this->value, calc.value);
}


merutilm::rff::fp_decimal::~fp_decimal() {

    mpz_clear(this->value);
}


merutilm::rff::fp_decimal::fp_decimal(const fp_decimal &other) {
    this->exp2 = other.exp2;
    mpz_init(this->value);
    mpz_set(this->value, other.value);
}

merutilm::rff::fp_decimal &merutilm::rff::fp_decimal::operator=(const fp_decimal &other) {
    this->exp2 = other.exp2;
    mpz_set(this->value, other.value);
    return *this;
}

merutilm::rff::fp_decimal::fp_decimal(fp_decimal &&other) noexcept {
    this->exp2 = other.exp2;
    mpz_init(this->value);
    mpz_swap(this->value, other.value);
    mpz_set_ui(other.value, 0);
}

merutilm::rff::fp_decimal &merutilm::rff::fp_decimal::operator=(fp_decimal &&other) noexcept {
    if (this != &other) {
        this->exp2 = other.exp2;
        mpz_swap(this->value, other.value);
        mpz_set_ui(other.value, 0);
    }
    return *this;
}

std::string merutilm::rff::fp_decimal::to_string() const{
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

int merutilm::rff::fp_decimal::get_exp2() const {
    return exp2;
}

bool merutilm::rff::fp_decimal::is_positive() const {
    return mpz_sgn(value) == 1;
}

bool merutilm::rff::fp_decimal::is_zero() const {
    return mpz_sgn(value) == 0;
}

bool merutilm::rff::fp_decimal::is_negative() const {
    return mpz_sgn(value) == -1;
}

merutilm::rff::fp_decimal_calculator merutilm::rff::fp_decimal::edit() const{
    return fp_decimal_calculator(value, exp2);
}

