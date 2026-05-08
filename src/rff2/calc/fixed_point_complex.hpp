//
// Created by Merutilm on 2026-04-25.
//

#pragma once
#include <array>

#include "fixed_point_complex.hpp"
#include "fixed_point_decimal.hpp"
#include "spin_thread_pool.hpp"

namespace merutilm::rff2 {


    struct fixed_point_complex;


    using op_thread_pool =
            spin_thread_pool<fixed_point_complex *, const fixed_point_complex *, const fixed_point_complex *>;


    struct fixed_point_complex {
        static constexpr int TEMPS_COUNT = 6;
        fixed_point_decimal real;
        fixed_point_decimal imag;
        std::array<fixed_point_decimal, TEMPS_COUNT> temps;

        explicit fixed_point_complex(const std::string &re_str, const std::string &im_str, int dec_exp10,
                                     int int_exp10);

        explicit fixed_point_complex(double re, double im, int dec_exp10, int int_exp10);

        explicit fixed_point_complex(const dex re, const dex im, int dec_exp10, int int_exp10);

        explicit fixed_point_complex(const fixed_point_decimal &re, const fixed_point_decimal &im, int dec_exp10,
                                     int int_exp10);

        /**
         * Fast-addition. It assumes that the count of limbs of both numbers are the same.
         * in-place operation is supported.
         * @param result the pointer of result
         * @param lhs left operand
         * @param rhs right operand
         */
        static void add(fixed_point_complex &result, const fixed_point_complex &lhs, const fixed_point_complex &rhs);

        /**
         * Fast-subtraction. It assumes that the count of limbs of both numbers are the same.
         * in-place operation is supported.
         * @param result the pointer of result
         * @param lhs left operand
         * @param rhs right operand
         */
        static void sub(fixed_point_complex &result, const fixed_point_complex &lhs, const fixed_point_complex &rhs);

        /**
         * Fast-multiplication. It assumes that the count of limbs of both numbers are the same.
         * in-place operation is supported. (but in-place multiplication of each decimal is not supported)
         * @param result the pointer of result
         * @param lhs left operand
         * @param rhs right operand
         * @param tp multithread pool, default is nullptr
         */
        static void mul(fixed_point_complex &result, const fixed_point_complex &lhs, const fixed_point_complex &rhs,
                        op_thread_pool *tp = nullptr);

        /**
         * Fast-division. It assumes that the count of limbs of both numbers are the same.
         * in-place operation is supported.
         * @param result the pointer of result
         * @param lhs left operand
         * @param rhs right operand
         * @param tp multithread pool, default is nullptr
         */
        static void div(fixed_point_complex &result, const fixed_point_complex &lhs, const fixed_point_complex &rhs,
                        op_thread_pool *tp = nullptr);

        /**
         * Fast-square. It assumes that the count of limbs of both numbers are the same.
         * in-place operation is supported. (but in-place square of each decimal is not supported)
         * @param result the pointer of result
         * @param v operand
         * @param tp multithread pool, default is nullptr
         */
        static void sqr(fixed_point_complex &result, const fixed_point_complex &v, op_thread_pool *tp = nullptr);
        /**
         * Fast-doubling. It assumes that the count of limbs of both numbers are the same.
         * in-place operation is supported.
         * @param result
         * @param v operand
         */
        static void dbl(fixed_point_complex &result, const fixed_point_complex &v);
        /**
         * Fast-halving. It assumes that the count of limbs of both numbers are the same.
         * in-place operation is supported.
         * @param result
         * @param v operand
         */
        static void hlv(fixed_point_complex &result, const fixed_point_complex &v);


        static void neg(fixed_point_complex &v);

        static void make_operation_compatible(fixed_point_complex &result, const fixed_point_complex &v);

        [[nodiscard]] fixed_point_decimal &get_real();

        [[nodiscard]] fixed_point_decimal &get_imag();

        [[nodiscard]] fixed_point_decimal clone_real() const;

        [[nodiscard]] fixed_point_decimal clone_imag() const;

        fixed_point_complex create_variant(int dec_exp10, int int_exp10) const;

        void set_exp10(int dec_exp10, int int_exp10);

        std::string to_string();
    };


    inline fixed_point_complex::fixed_point_complex(const std::string &re_str, const std::string &im_str,
                                                    const int dec_exp10, const int int_exp10) :
        real(re_str, dec_exp10, int_exp10), imag(im_str, dec_exp10, int_exp10) {
        for (auto &temp: temps) {
            temp.set_exp10(dec_exp10, int_exp10);
        }
    }

    inline fixed_point_complex::fixed_point_complex(const double re, const double im, const int dec_exp10,
                                                    const int int_exp10) :
        real(re, dec_exp10, int_exp10), imag(im, dec_exp10, int_exp10) {
        for (auto &temp: temps) {
            temp.set_exp10(dec_exp10, int_exp10);
        }
    }
    inline fixed_point_complex::fixed_point_complex(const dex re, const dex im, const int dec_exp10,
                                                    const int int_exp10) :
        real(re, dec_exp10, int_exp10), imag(im, dec_exp10, int_exp10) {
        for (auto &temp: temps) {
            temp.set_exp10(dec_exp10, int_exp10);
        }
    }

    inline fixed_point_complex::fixed_point_complex(const fixed_point_decimal &re, const fixed_point_decimal &im,
                                                    const int dec_exp10, const int int_exp10) : real(re), imag(im) {
        set_exp10(dec_exp10, int_exp10);
        for (auto &temp: temps) {
            temp.set_exp10(dec_exp10, int_exp10);
        }
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
                                         const fixed_point_complex &rhs, op_thread_pool *tp) {
        //(a+bi)*(c+di)
        // REAL : ac-bd
        // IMAG : ad+bc

        fixed_point_decimal::make_operation_compatible(result.temps[0], lhs.real);
        fixed_point_decimal::make_operation_compatible(result.temps[1], rhs.real);
        fixed_point_decimal::make_operation_compatible(result.temps[2], lhs.real);
        fixed_point_decimal::make_operation_compatible(result.temps[3], lhs.real);
        fixed_point_decimal::make_operation_compatible(result.temps[4], lhs.real);

        if (tp) {
            if (tp->is_empty()) {
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::sub(res->temps[0], l->real, l->imag);
                    fixed_point_decimal::add(res->temps[1], r->real, r->imag);
                    fixed_point_decimal::mul(res->temps[2], res->temps[0], res->temps[1]);
                });
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::mul(res->temps[3], l->real, r->imag);
                });
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::mul(res->temps[4], l->imag, r->real);
                });
            }


            tp->run_all(&result, &lhs, &rhs);
            tp->wait_all();

        } else {

            fixed_point_decimal::sub(result.temps[0], lhs.real, lhs.imag);
            fixed_point_decimal::add(result.temps[1], rhs.real, rhs.imag);
            fixed_point_decimal::mul(result.temps[2], result.temps[0], result.temps[1]);
            fixed_point_decimal::mul(result.temps[3], lhs.real, rhs.imag);
            fixed_point_decimal::mul(result.temps[4], lhs.imag, rhs.real);
        }

        fixed_point_decimal::sub(result.real, result.temps[2], result.temps[3]);
        fixed_point_decimal::add(result.real, result.real, result.temps[4]);
        fixed_point_decimal::add(result.imag, result.temps[3], result.temps[4]);
    }


    inline void fixed_point_complex::div(fixed_point_complex &result, const fixed_point_complex &lhs,
                                         const fixed_point_complex &rhs, op_thread_pool *tp) {
        // (a + bi) / (c + di)
        // REAL : (ac+bd) / (c^2+d^2)
        // IMAG : (bc-ad) / (c^2+d^2)

        //(a + b)(c + d)
        // ac + bd + bc + ad
        //

        if (tp) {
            if (tp->is_empty()) {

                // 1ST
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *, const fixed_point_complex *r) {
                    fixed_point_decimal::sqr(res->temps[0], r->real);
                });
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *, const fixed_point_complex *r) {
                    fixed_point_decimal::sqr(res->temps[1], r->imag);
                });

                // 2ND
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::add(res->temps[0], res->temps[0], res->temps[1]);
                    fixed_point_decimal::add(res->temps[1], l->real, l->imag);
                    fixed_point_decimal::add(res->temps[2], r->real, r->imag);
                    fixed_point_decimal::mul(res->temps[3], res->temps[1], res->temps[2]);
                });
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::mul(res->temps[4], l->real, r->imag);
                });
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *l, const fixed_point_complex *r) {
                    fixed_point_decimal::mul(res->temps[5], l->imag, r->real);
                });

                // 3RD
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *, const fixed_point_complex *) {
                    fixed_point_decimal::sub(res->temps[3], res->temps[3], res->temps[4]);
                    fixed_point_decimal::sub(res->temps[3], res->temps[3], res->temps[5]);
                    fixed_point_decimal::div(res->real, res->temps[3], res->temps[0]);
                });
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *, const fixed_point_complex *) {
                    fixed_point_decimal::sub(res->temps[1], res->temps[5], res->temps[4]);
                    fixed_point_decimal::div(res->imag, res->temps[1], res->temps[0]);
                });
            }

            tp->run_ranged(0, 2, &result, &lhs, &rhs);
            tp->wait_all();
            tp->run_ranged(2, 3, &result, &lhs, &rhs);
            tp->wait_all();
            tp->run_ranged(5, 2, &result, &lhs, &rhs);
            tp->wait_all();

        } else {
            fixed_point_decimal::sqr(result.temps[0], rhs.real);
            fixed_point_decimal::sqr(result.temps[1], rhs.imag);
            fixed_point_decimal::add(result.temps[0], result.temps[0], result.temps[1]);

            fixed_point_decimal::add(result.temps[1], lhs.real, lhs.imag);
            fixed_point_decimal::add(result.temps[2], rhs.real, rhs.imag);
            fixed_point_decimal::mul(result.temps[3], result.temps[1], result.temps[2]);


            fixed_point_decimal::mul(result.temps[4], lhs.real, rhs.imag);
            fixed_point_decimal::mul(result.temps[5], lhs.imag, rhs.real);

            fixed_point_decimal::sub(result.temps[3], result.temps[3], result.temps[4]);
            fixed_point_decimal::sub(result.temps[3], result.temps[3], result.temps[5]);

            fixed_point_decimal::sub(result.temps[4], result.temps[5], result.temps[4]);

            fixed_point_decimal::div(result.real, result.temps[3], result.temps[0]);
            fixed_point_decimal::div(result.imag, result.temps[4], result.temps[0]);
        }
    }


    inline void fixed_point_complex::sqr(fixed_point_complex &result, const fixed_point_complex &v,
                                         op_thread_pool *tp) {
        //(a+bi)^2
        // REAL : a^2-b^2 = (a+b)(a-b)
        // IMAG : 2ab
        fixed_point_decimal::make_operation_compatible(result.temps[0], v.real);
        fixed_point_decimal::make_operation_compatible(result.temps[1], v.real);
        fixed_point_decimal::make_operation_compatible(result.temps[2], v.real);

        if (tp) {

            if (tp->is_empty()) {
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *, const fixed_point_complex *) {
                    fixed_point_decimal::mul(res->temps[3], res->temps[0], res->temps[1]);
                });
                tp->add_func([](fixed_point_complex *res, const fixed_point_complex *v2, const fixed_point_complex *) {
                    fixed_point_decimal::mul(res->temps[2], v2->real, v2->imag);
                });
            }

            fixed_point_decimal::make_operation_compatible(result.temps[3], v.real);
            fixed_point_decimal::add(result.temps[0], v.real, v.imag);
            fixed_point_decimal::sub(result.temps[1], v.real, v.imag);
            tp->run_all(&result, &v, nullptr);
            tp->wait_all();
            std::swap(result.real, result.temps[3]);
            fixed_point_decimal::dbl(result.imag, result.temps[2]);
        } else {
            fixed_point_decimal::add(result.temps[0], v.real, v.imag);
            fixed_point_decimal::sub(result.temps[1], v.real, v.imag);
            fixed_point_decimal::mul(result.temps[2], v.real, v.imag);
            fixed_point_decimal::mul(result.real, result.temps[0], result.temps[1]);
            fixed_point_decimal::dbl(result.imag, result.temps[2]);
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

    inline void fixed_point_complex::neg(fixed_point_complex &v) {
        fixed_point_decimal::neg(v.real);
        fixed_point_decimal::neg(v.imag);
    }

    inline void fixed_point_complex::make_operation_compatible(fixed_point_complex &result, const fixed_point_complex &v) {
        fixed_point_decimal::make_operation_compatible(result.real, v.real);
        fixed_point_decimal::make_operation_compatible(result.imag, v.imag);
    }

    inline fixed_point_decimal &fixed_point_complex::get_real() { return real; }


    inline fixed_point_decimal &fixed_point_complex::get_imag() { return imag; }


    inline fixed_point_decimal fixed_point_complex::clone_real() const { return real; }


    inline fixed_point_decimal fixed_point_complex::clone_imag() const { return imag; }

    inline fixed_point_complex fixed_point_complex::create_variant(const int dec_exp10, const int int_exp10) const {
        return fixed_point_complex(real, imag, dec_exp10, int_exp10);
    }


    inline void fixed_point_complex::set_exp10(const int dec_exp10, const int int_exp10) {
        real.set_exp10(dec_exp10, int_exp10);
        imag.set_exp10(dec_exp10, int_exp10);

        for (auto &temp: temps) {
            temp.set_exp10(dec_exp10, int_exp10);
        }
    }


    inline std::string fixed_point_complex::to_string() {
        if (real.sgn == 0 && imag.sgn == 0)
            return "0";

        const std::string re = real.to_string();
        const std::string im = imag.to_string();
        std::ostringstream oss;

        if (real.sgn != 0) {
            oss << re;
        }
        if (imag.sgn != 0) {
            if (real.sgn != 0 && imag.sgn == 1)
                oss << "+";
            oss << im;
            oss << "i";
        }

        return oss.str();
    }

    template<int int_exp10>
    struct fixed_point_complex_i_locked : fixed_point_complex {
        explicit fixed_point_complex_i_locked(const std::string &re_str, const std::string &im_str,
                                              const int dec_exp10) :
            fixed_point_complex(re_str, im_str, dec_exp10, int_exp10) {}
        explicit fixed_point_complex_i_locked(const double re, const double im, const int dec_exp10) :
            fixed_point_complex(re, im, dec_exp10, int_exp10) {}

        explicit fixed_point_complex_i_locked(const dex re, const dex im,
                                                      const int dec_exp10) :
                    fixed_point_complex(re, im, dec_exp10, int_exp10) {}


        explicit fixed_point_complex_i_locked(const fixed_point_decimal &re, const fixed_point_decimal &im,
                                              const int dec_exp10) :
            fixed_point_complex(re, im, dec_exp10, int_exp10) {}

        using fixed_point_complex::create_variant;
        fixed_point_complex_i_locked create_variant(const int dec_exp10) const {
            return fixed_point_complex_i_locked(real, imag, dec_exp10);
        }

        using fixed_point_complex::set_exp10;
        void set_exp10(const int dec_exp10) {
            fixed_point_complex::set_exp10(dec_exp10, int_exp10);
        }
    };

    using fixed_point_complex_i1 = fixed_point_complex_i_locked<1>;
} // namespace merutilm::rff2
