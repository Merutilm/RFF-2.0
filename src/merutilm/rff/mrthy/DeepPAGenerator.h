//
// Created by Merutilm on 2025-05-22.
//

#pragma once
#include "DeepPA.h"
#include "PAGenerator.h"
#include "../calc/double_exp.h"
#include "../formula/DeepMandelbrotReference.h"

class DeepPAGenerator final : public PAGenerator{
    double_exp anr;
    double_exp ani;
    double_exp bnr;
    double_exp bni;
    double_exp radius;
    std::array<double_exp, 8> &temps;
    const std::vector<double_exp> &refReal;
    const std::vector<double_exp> &refImag;
    double_exp dcMax;

public:
    explicit DeepPAGenerator(const DeepMandelbrotReference &reference, double epsilon, const double_exp &dcMax, uint64_t start, std::array<double_exp, 8> &temps);

    static std::unique_ptr<DeepPAGenerator> create(const DeepMandelbrotReference &reference, double epsilon,
                                             const double_exp &dcMax,
                                             uint64_t start, std::array<double_exp, 8> &temps);


    void merge(const PA &pa) override;

    void step() override;

    DeepPA build() const{
        return DeepPA(anr, ani, bnr, bni, skip, radius);
    }
};

inline std::unique_ptr<DeepPAGenerator> DeepPAGenerator::create(const DeepMandelbrotReference &reference,
                                                                      const double epsilon, const double_exp &dcMax,
                                                                      const uint64_t start, std::array<double_exp, 8> &temps) {
    return std::make_unique<DeepPAGenerator>(reference, epsilon, dcMax, start, temps);
}
