//
// Created by Merutilm on 2025-05-05.
//

#include "GMPDecimal.h"

#include <iostream>
#include <string>
#include <bits/ostream.tcc>

#include "GMPDecimalCalculator.h"



GMPDecimal::GMPDecimal(const std::string& value, const int exp10) : GMPDecimal(GMPDecimalCalculator(value, exp10)){};


GMPDecimal::GMPDecimal(const GMPDecimalCalculator &calc) {
    this->exp2 = calc.exp2;
    mpz_init(this->value);
    mpz_set(this->value, calc.value);
}


GMPDecimal::~GMPDecimal() {

    mpz_clear(this->value);
}


GMPDecimal::GMPDecimal(const GMPDecimal &other) {
    this->exp2 = other.exp2;
    mpz_init(this->value);
    mpz_set(this->value, other.value);
}

GMPDecimal &GMPDecimal::operator=(const GMPDecimal &other) {
    this->exp2 = other.exp2;
    mpz_set(this->value, other.value);
    return *this;
}

GMPDecimal::GMPDecimal(GMPDecimal &&other) noexcept {
    this->exp2 = other.exp2;
    mpz_init(this->value);
    mpz_swap(this->value, other.value);
    mpz_set_ui(other.value, 0);
}

GMPDecimal &GMPDecimal::operator=(GMPDecimal &&other) noexcept {
    if (this != &other) {
        this->exp2 = other.exp2;
        mpz_swap(this->value, other.value);
        mpz_set_ui(other.value, 0);
    }
    return *this;
}

std::string GMPDecimal::toString() const{
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

int GMPDecimal::getExp2() const {
    return exp2;
}

bool GMPDecimal::isPositive() const {
    return mpz_sgn(value) == 1;
}

bool GMPDecimal::isZero() const {
    return mpz_sgn(value) == 0;
}

bool GMPDecimal::isNegative() const {
    return mpz_sgn(value) == -1;
}

GMPDecimalCalculator GMPDecimal::edit() const{
    return GMPDecimalCalculator(value, exp2);
}
