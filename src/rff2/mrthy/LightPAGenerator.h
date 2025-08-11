//
// Created by Merutilm on 2025-05-22.
//

#pragma once
#include <vector>

#include "LightPA.h"
#include "PAGenerator.h"
#include "../formula/LightMandelbrotReference.h"

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
        explicit LightPAGenerator(const LightMandelbrotReference &reference, double epsilon, double dcMax, uint64_t start);

        static std::unique_ptr<LightPAGenerator> create(const LightMandelbrotReference &reference, double epsilon,
                                                   double dcMax,
                                                   uint64_t start);


        void merge(const PA &pa) override;

        void step() override;

        LightPA build() const {
            return LightPA(anr, ani, bnr, bni, skip, radius);
        }
    };

    inline std::unique_ptr<LightPAGenerator> LightPAGenerator::create(const LightMandelbrotReference &reference,
                                                                 const double epsilon, const double dcMax,
                                                                 const uint64_t start) {
        return std::make_unique<LightPAGenerator>(reference, epsilon, dcMax, start);
    }
}