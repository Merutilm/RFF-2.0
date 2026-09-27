//
// Created by Merutilm on 2026-04-25.
//

#pragma once
#include <array>

#include <sstream>
#include <utility>
#include "fixed_point_decimal.hpp"
#include "spin_thread_pool.hpp"

#include "complex.hpp"

namespace merutilm::rff2 {


    struct fixed_point_complex;


    using op_thread_pool =
            spin_thread_pool<fixed_point_complex *, const fixed_point_complex *, const fixed_point_complex *>;


    using op_thread_pool_s = spin_thread_pool<fixed_point_complex *, const fixed_point_complex *, const uint64_t *>;


    struct fixed_point_complex {
        static constexpr uint64_t TEMPS_COUNT = 6;
        fixed_point_decimal real;
        fixed_point_decimal imag;

        fixed_point_complex() : fixed_point_complex(0.0, 0.0, -1) {}

        explicit fixed_point_complex(const std::string &re_str, const std::string &im_str, int64_t exp10);

        explicit fixed_point_complex(double re, double im, int64_t exp10);

        template<Number Exp, Number Mantissa, Number Bit>
        explicit fixed_point_complex(exponent<Exp, Mantissa, Bit> re, exponent<Exp, Mantissa, Bit> im, int64_t exp10);

        template<Number Num>
        explicit fixed_point_complex(complex<Num> c, int64_t exp10);

        explicit fixed_point_complex(fixed_point_decimal re, fixed_point_decimal im, int64_t exp10);

        void add_one();

        /**
         * Fast-addition. It assumes that the count of limbs of both numbers are the same.
         * in-place operation is supported.
         * @param result the pointer of result
         * @param lhs left operand
         * @param rhs right operand
         */
        static void add(fixed_point_complex &result, const fixed_point_complex &lhs, const fixed_point_complex &rhs);

        /**
         * Fast-subtraction. It assumes that the exp2div64 of both numbers are the same.
         * in-place operation is supported.
         * @param result the pointer of result
         * @param lhs left operand
         * @param rhs right operand
         */
        static void sub(fixed_point_complex &result, const fixed_point_complex &lhs, const fixed_point_complex &rhs);

        /**
         * Fast-multiplication. It assumes that the exp2div64 of both numbers are the same.
         * in-place operation is supported. (but in-place multiplication of each decimal is not supported)
         * @param result the pointer of result
         * @param lhs left operand
         * @param rhs right operand
         * @param temps temp
         * @param tp multithread pool, default is nullptr
         */
        static void mul(fixed_point_complex &result, const fixed_point_complex &lhs, const fixed_point_complex &rhs, std::array<fixed_point_decimal, TEMPS_COUNT> &temps,
                        op_thread_pool *tp = nullptr);

        static void mul(fixed_point_complex &result, const fixed_point_complex &lhs, uint64_t rhs,
                        op_thread_pool_s *tp_s = nullptr);

        /**
         * Fast-division. It assumes that the exp2div64 of both numbers are the same.
         * in-place operation is supported.
         * @param result the pointer of result
         * @param lhs left operand
         * @param rhs right operand
         * @param temps temp
         * @param tp multithread pool, default is nullptr
         */
        static void div(fixed_point_complex &result, const fixed_point_complex &lhs, const fixed_point_complex &rhs, std::array<fixed_point_decimal, TEMPS_COUNT> &temps,
                        op_thread_pool *tp = nullptr);

        /**
         * Fast-square. It assumes that the exp2div64 of both numbers are the same.
         * in-place operation is supported. (but in-place square of each decimal is not supported)
         * @param result the pointer of result
         * @param v operand
         * @param temps temp
         * @param tp multithread pool, default is nullptr
         */
        static void sqr(fixed_point_complex &result, const fixed_point_complex &v, std::array<fixed_point_decimal, TEMPS_COUNT> &temps, op_thread_pool *tp = nullptr);
        /**
         * Fast-doubling. It assumes that the exp2div64 of both numbers are the same.
         * in-place operation is supported.
         * @param result
         * @param v operand
         */
        static void dbl(fixed_point_complex &result, const fixed_point_complex &v);
        /**
         * Fast-halving. It assumes that the exp2div64 of both numbers are the same.
         * in-place operation is supported.
         * @param result
         * @param v operand
         */
        static void hlv(fixed_point_complex &result, const fixed_point_complex &v);

        void one();

        void zero();

        void neg();

        template<Number Num>
        explicit operator complex<Num>() const {
            return {static_cast<Num>(real), static_cast<Num>(imag)};
        }

        [[nodiscard]] fixed_point_decimal &get_real();

        [[nodiscard]] fixed_point_decimal &get_imag();

        [[nodiscard]] fixed_point_decimal clone_real() const;

        [[nodiscard]] fixed_point_decimal clone_imag() const;

        [[nodiscard]] fixed_point_complex create_variant(int64_t exp10) const;
        void try_realloc_inc(uint64_t new_limbs_alloc);

        void set_exp10(int64_t exp10);

        [[nodiscard]] bool is_zero() const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] static std::array<fixed_point_decimal, TEMPS_COUNT> create_temps(int64_t exp10);
    };


    inline fixed_point_complex::fixed_point_complex(const std::string &re_str, const std::string &im_str,
                                                    const int64_t exp10) :
        real(re_str, exp10), imag(im_str, exp10) {
    }

    inline fixed_point_complex::fixed_point_complex(const double re, const double im, const int64_t exp10) :
        real(re, exp10), imag(im, exp10) {
    }

    template<Number Exp, Number Mantissa, Number Bit>
    fixed_point_complex::fixed_point_complex(const exponent<Exp, Mantissa, Bit> re,
                                             const exponent<Exp, Mantissa, Bit> im, const int64_t exp10) :
        real(re, exp10), imag(im, exp10) {
    }

    template<Number Num>
    fixed_point_complex::fixed_point_complex(const complex<Num> c, const int64_t exp10) :
        real(c.re, exp10), imag(c.im, exp10) {
    }

    inline fixed_point_complex::fixed_point_complex(fixed_point_decimal re, fixed_point_decimal im,
                                                    const int64_t exp10) : real(std::move(re)), imag(std::move(im)) {
        set_exp10(exp10);
    }

    inline void fixed_point_complex::add_one() {
        real.add_one();
    }

    inline void fixed_point_complex::add(fixed_point_complex &result, const fixed_point_complex &lhs,
                                         const fixed_point_complex &rhs) {
        fixed_point_decimal::add(result.real, lhs.real, rhs.real);
        fixed_point_decimal::add(result.imag, lhs.imag, rhs.imag);
    }


    inline void fixed_point_complex::sub(fixed_point_complex &result, const fixed_point_complex &lhs,
                                         const fixed_point_complex &rhs) {
        fixed_point_decimal::sub(result.real, lhs.real, rhs.real);
        fixed_point_decimal::sub(result.imag, lhs.imag, rhs.imag);
    }

    inline void fixed_point_complex::mul(fixed_point_complex &result, const fixed_point_complex &lhs,
                                         const uint64_t rhs, op_thread_pool_s *tp_s) {


        if (tp_s) {
            if (tp_s->is_empty()) {
                tp_s->add_func([](fixed_point_complex *res, const fixed_point_complex *l, const uint64_t *r) {
                    fixed_point_decimal::mul(res->real, l->real, *r);
                });
                tp_s->add_func([](fixed_point_complex *res, const fixed_point_complex *l, const uint64_t *r) {
                    fixed_point_decimal::mul(res->imag, l->imag, *r);
                });
            }


            tp_s->run_all(&result, &lhs, &rhs);
            tp_s->wait_all();

        } else {
            fixed_point_decimal::mul(result.real, lhs.real, rhs);
            fixed_point_decimal::mul(result.imag, lhs.imag, rhs);
        }
    }

    inline void fixed_point_complex::mul(fixed_point_complex &result, const fixed_point_complex &lhs,
                                         const fixed_point_complex &rhs, std::array<fixed_point_decimal, TEMPS_COUNT> &temps, op_thread_pool *tp) {
        //(a+bi)*(c+di)
        // REAL : ac-bd
        // IMAG : ad+bc

        if (tp) {
            if (tp->is_empty()) {
                tp->add_func([&temps](fixed_point_complex *, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::sub(temps[0], l->real, l->imag);
                    fixed_point_decimal::add(temps[1], r->real, r->imag);
                    fixed_point_decimal::mul(temps[2], temps[0], temps[1]);
                });
                tp->add_func([&temps](fixed_point_complex *,const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::mul(temps[3], l->real, r->imag);
                });
                tp->add_func([&temps](fixed_point_complex *, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::mul(temps[4], l->imag, r->real);
                });
            }


            tp->run_all(&result, &lhs, &rhs);
            tp->wait_all();

        } else {

            fixed_point_decimal::sub(temps[0], lhs.real, lhs.imag);
            fixed_point_decimal::add(temps[1], rhs.real, rhs.imag);
            fixed_point_decimal::mul(temps[2], temps[0], temps[1]);
            fixed_point_decimal::mul(temps[3], lhs.real, rhs.imag);
            fixed_point_decimal::mul(temps[4], lhs.imag, rhs.real);
        }

        fixed_point_decimal::sub(result.real, temps[2], temps[3]);
        fixed_point_decimal::add(result.real, result.real, temps[4]);
        fixed_point_decimal::add(result.imag, temps[3], temps[4]);
    }


    inline void fixed_point_complex::div(fixed_point_complex &result, const fixed_point_complex &lhs,
                                         const fixed_point_complex &rhs, std::array<fixed_point_decimal, TEMPS_COUNT> &temps, op_thread_pool *tp) {
        // (a + bi) / (c + di)
        // REAL : (ac+bd) / (c^2+d^2)
        // IMAG : (bc-ad) / (c^2+d^2)

        //(a + b)(c + d)
        // ac + bd + bc + ad
        //

        if (tp) {
            if (tp->is_empty()) {

                // 1ST
                tp->add_func([&temps](fixed_point_complex *, const fixed_point_complex *, const fixed_point_complex *r) {
                    fixed_point_decimal::sqr(temps[0], r->real);
                });
                tp->add_func([&temps](fixed_point_complex *, const fixed_point_complex *, const fixed_point_complex *r) {
                    fixed_point_decimal::sqr(temps[1], r->imag);
                });

                // 2ND
                tp->add_func([&temps](fixed_point_complex *, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::add(temps[0], temps[0], temps[1]);
                    fixed_point_decimal::add(temps[1], l->real, l->imag);
                    fixed_point_decimal::add(temps[2], r->real, r->imag);
                    fixed_point_decimal::mul(temps[3], temps[1], temps[2]);
                });
                tp->add_func([&temps](fixed_point_complex *, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::mul(temps[4], l->real, r->imag);
                });
                tp->add_func([&temps](fixed_point_complex *, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::mul(temps[5], l->imag, r->real);
                });

                // 3RD
                tp->add_func([&temps](fixed_point_complex *res, const fixed_point_complex *, const fixed_point_complex *) {
                    fixed_point_decimal::sub(temps[3], temps[3], temps[4]);
                    fixed_point_decimal::sub(temps[3], temps[3], temps[5]);
                    fixed_point_decimal::div(res->real, temps[3], temps[0]);
                });
                tp->add_func([&temps](fixed_point_complex *res, const fixed_point_complex *, const fixed_point_complex *) {
                    fixed_point_decimal::sub(temps[1], temps[5], temps[4]);
                    fixed_point_decimal::div(res->imag, temps[1], temps[0]);
                });
            }

            tp->run_ranged(0, 2, &result, &lhs, &rhs);
            tp->wait_all();
            tp->run_ranged(2, 3, &result, &lhs, &rhs);
            tp->wait_all();
            tp->run_ranged(5, 2, &result, &lhs, &rhs);
            tp->wait_all();

        } else {
            fixed_point_decimal::sqr(temps[0], rhs.real);
            fixed_point_decimal::sqr(temps[1], rhs.imag);
            fixed_point_decimal::add(temps[0], temps[0], temps[1]);

            fixed_point_decimal::add(temps[1], lhs.real, lhs.imag);
            fixed_point_decimal::add(temps[2], rhs.real, rhs.imag);
            fixed_point_decimal::mul(temps[3], temps[1], temps[2]);


            fixed_point_decimal::mul(temps[4], lhs.real, rhs.imag);
            fixed_point_decimal::mul(temps[5], lhs.imag, rhs.real);

            fixed_point_decimal::sub(temps[3], temps[3], temps[4]);
            fixed_point_decimal::sub(temps[3], temps[3], temps[5]);

            fixed_point_decimal::sub(temps[4], temps[5], temps[4]);

            fixed_point_decimal::div(result.real, temps[3], temps[0]);
            fixed_point_decimal::div(result.imag, temps[4], temps[0]);
        }
    }


    inline void fixed_point_complex::sqr(fixed_point_complex &result, const fixed_point_complex &v,
                                         std::array<fixed_point_decimal, TEMPS_COUNT> &temps, op_thread_pool *tp) {
        //(a+bi)^2
        // REAL : a^2-b^2 = (a+b)(a-b)
        // IMAG : 2ab

        if (tp) {

            if (tp->is_empty()) {
                tp->add_func([&temps](fixed_point_complex *, const fixed_point_complex *, const fixed_point_complex *) {
                    fixed_point_decimal::mul(temps[3], temps[0], temps[1]);
                });
                tp->add_func([&temps](fixed_point_complex *, const fixed_point_complex *v2, const fixed_point_complex *) {
                    fixed_point_decimal::mul(temps[2], v2->real, v2->imag);
                });
            }

            fixed_point_decimal::add(temps[0], v.real, v.imag);
            fixed_point_decimal::sub(temps[1], v.real, v.imag);
            tp->run_all(&result, &v, nullptr);
            tp->wait_all();
            std::swap(result.real, temps[3]);
            fixed_point_decimal::dbl(result.imag, temps[2]);
        } else {
            fixed_point_decimal::add(temps[0], v.real, v.imag);
            fixed_point_decimal::sub(temps[1], v.real, v.imag);
            fixed_point_decimal::mul(temps[2], v.real, v.imag);
            fixed_point_decimal::mul(result.real, temps[0], temps[1]);
            fixed_point_decimal::dbl(result.imag, temps[2]);
        }
    }

    inline void fixed_point_complex::dbl(fixed_point_complex &result, const fixed_point_complex &v) {
        fixed_point_decimal::dbl(result.real, v.real);
        fixed_point_decimal::dbl(result.imag, v.imag);
    }


    inline void fixed_point_complex::hlv(fixed_point_complex &result, const fixed_point_complex &v) {
        fixed_point_decimal::hlv(result.real, v.real);
        fixed_point_decimal::hlv(result.imag, v.imag);
    }

    inline void fixed_point_complex::one() {
        real.one();
        imag.zero();
    }

    inline void fixed_point_complex::zero() {
        real.zero();
        imag.zero();
    }

    inline void fixed_point_complex::neg() {
        real.neg();
        imag.neg();
    }

    inline fixed_point_decimal &fixed_point_complex::get_real() { return real; }


    inline fixed_point_decimal &fixed_point_complex::get_imag() { return imag; }


    inline fixed_point_decimal fixed_point_complex::clone_real() const { return real; }


    inline fixed_point_decimal fixed_point_complex::clone_imag() const { return imag; }

    inline fixed_point_complex fixed_point_complex::create_variant(const int64_t exp10) const {
        return fixed_point_complex(real, imag, exp10);
    }

    inline void fixed_point_complex::try_realloc_inc(const uint64_t new_limbs_alloc) {
        real.try_realloc_inc(new_limbs_alloc);
        imag.try_realloc_inc(new_limbs_alloc);
    }


    inline void fixed_point_complex::set_exp10(const int64_t exp10) {
        real.set_exp10(exp10);
        imag.set_exp10(exp10);
    }
    inline bool fixed_point_complex::is_zero() const { return real.size == 0 && imag.size == 0; }


    inline std::string fixed_point_complex::to_string() const {
        if (is_zero())
            return "0";

        const std::string re = real.to_string();
        const std::string im = imag.to_string();
        std::ostringstream oss;

        if (real.size != 0) {
            oss << re;
        }
        if (imag.size != 0) {
            if (real.size != 0 && imag.size > 0)
                oss << "+";
            oss << im;
            oss << "i";
        }

        return oss.str();
    }

    inline std::array<fixed_point_decimal, fixed_point_complex::TEMPS_COUNT> fixed_point_complex::create_temps(const int64_t exp10) {
        std::array<fixed_point_decimal, TEMPS_COUNT> result;
        for (fixed_point_decimal &temp : result) {
            temp.set_exp10(exp10);
        }
        return result;
    }

} // namespace merutilm::rff2
