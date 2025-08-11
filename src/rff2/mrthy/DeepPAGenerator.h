//
// Created by Merutilm on 2025-05-22.
//

#pragma once
#include "DeepPA.h"
#include "PAGenerator.h"
#include "../calc/dex.h"
#include "../formula/DeepMandelbrotReference.h"

namespace merutilm::rff2 {
    class DeepPAGenerator final : public PAGenerator{
        dex anr;
        dex ani;
        dex bnr;
        dex bni;
        dex radius;
        std::array<dex, 8> &temps;
        const std::vector<dex> &refReal;
        const std::vector<dex> &refImag;
        dex dcMax;

    public:
        explicit DeepPAGenerator(const DeepMandelbrotReference &reference, double epsilon, const dex &dcMax, uint64_t start, std::array<dex, 8> &temps);

        static std::unique_ptr<DeepPAGenerator> create(const DeepMandelbrotReference &reference, double epsilon,
                                                 const dex &dcMax,
                                                 uint64_t start, std::array<dex, 8> &temps);


        void merge(const PA &pa) override;

        void step() override;

        DeepPA build() const{
            return DeepPA(anr, ani, bnr, bni, skip, radius);
        }
    };

    inline std::unique_ptr<DeepPAGenerator> DeepPAGenerator::create(const DeepMandelbrotReference &reference,
                                                                          const double epsilon, const dex &dcMax,
                                                                          const uint64_t start, std::array<dex, 8> &temps) {
        return std::make_unique<DeepPAGenerator>(reference, epsilon, dcMax, start, temps);
    }
}