//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "ArrayCompressionTool.h"
#include "PA.h"
#include "../calc/double_exp.h"
#include "../formula/DeepMandelbrotReference.h"


struct DeepPA final : public PA{
    const double_exp anr;
    const double_exp ani;
    const double_exp bnr;
    const double_exp bni;
    const double_exp radius;

    DeepPA(const double_exp &anr, const double_exp &ani, const double_exp &bnr, const double_exp &bni, uint64_t skip, const double_exp &radius);

    bool isValid(double_exp *temp, const double_exp &dzRad) const;

    double_exp getRadius() const;

};

inline DeepPA::DeepPA(const double_exp &anr, const double_exp &ani, const double_exp &bnr, const double_exp &bni,
               const uint64_t skip, const double_exp &radius) : PA(skip), anr(anr), ani(ani), bnr(bnr), bni(bni),
                                                                radius(radius) {
}



inline double_exp DeepPA::getRadius() const {
    return radius;
}


inline bool DeepPA::isValid(double_exp *temp, const double_exp &dzRad) const {
    double_exp::dex_sub(temp, radius, dzRad);
    return temp->sgn() > 0;
}


