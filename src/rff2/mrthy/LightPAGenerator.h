//
// Created by Merutilm on 2025-05-22.
//

#pragma once
#include <vector>

#include "LightPA.h"
#include "PAGenerator.h"
#include "../formula/LightMB2Reference.h"

namespace merutilm::rff2 {
    class LightPAGenerator final : public PAGenerator{
        double anr;
        double ani;
        double bnr;
        double bni;
        double radius;
        const std::vector<double> &refReal;
        const std::vector<double> &refImag;
        double dcMax;

    public:
        explicit LightPAGenerator(const LightMB2Reference &reference, double epsilon, double dcMax, uint64_t start);


        void merge(const PA &pa) override;

        void step() override;

        [[nodiscard]] LightPA build() const {
            return LightPA{anr, ani, bnr, bni, skip, radius};
        }
    };

}