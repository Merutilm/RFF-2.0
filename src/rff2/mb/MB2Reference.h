//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "../app/FnListeners.hpp"
#include "../calc/fixed_point_complex.hpp"
#include "../mrthy/ArrayCompressUnit.hpp"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtReferenceSettings.hpp"
#include "../util/ArrayCompressorUtils.hpp"
#include "MB2ReferenceGenerationResult.hpp"
#include "MB2ReferenceGenerator.hpp"
#include "Reference.hpp"
#include "ReferenceCheckpoint.hpp"

namespace merutilm::rff2 {



    struct MB2ReferenceBase : Reference{

        const FrtGeneralSettings generalSettings;
        const FrtReferenceSettings refSettings;
        fixed_point_complex center;
        const ReferenceCompressor compressor;
        const std::vector<itt::ref> period;
        std::vector<ReferenceCheckpoint> checkpoints;
        const complex<dex> fpgBn;
        const double logZoom;
        const dex dcMax;

        MB2ReferenceBase(FrtGeneralSettings generalSettings, FrtReferenceSettings refSettings,
                         fixed_point_complex &&center, ReferenceCompressor &&compressor, std::vector<itt::ref> &&period,
                         std::vector<ReferenceCheckpoint> &&checkpoints, complex<dex> fpgBn, const double logZoom,
                         const dex dcMax) :
            generalSettings(std::move(generalSettings)), refSettings(std::move(refSettings)), center(std::move(center)),
            compressor(std::move(compressor)), period(std::move(period)), checkpoints(std::move(checkpoints)),
            fpgBn(std::move(fpgBn)), logZoom(logZoom), dcMax(dcMax) {}

        [[nodiscard]] itt::ref longestPeriod() const { return period.back(); }
    };

    template<Number Num>
    struct MB2Reference final : MB2ReferenceBase {
        std::vector<complex<Num>> refOrbit;

        using MB2ReferenceBase::MB2ReferenceBase;


        template<FnListeners::FnRefCalc FnRefCalc>
        static bool
        generateReference(const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                          const FrtReferenceSettings &refSettings, int32_t exp10, size_t refInitialCapacity, dex dcMax,
                          FnRefCalc &&fnRefCalc, std::unique_ptr<MB2Reference> *result);

        [[nodiscard]] complex<Num> orbit(itt::ref refIteration) const;

        [[nodiscard]] size_t length() const override;
    };


    template<Number Num>
    template<FnListeners::FnRefCalc FnRefCalc>
    bool MB2Reference<Num>::generateReference(const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                                         const FrtReferenceSettings &refSettings, int32_t exp10,
                                         size_t refInitialCapacity, dex dcMax, FnRefCalc &&fnRefCalc,
                                         std::unique_ptr<MB2Reference> *result) {
        MB2ReferenceGenerator<Num, FnRefCalc> generator(state, generalSettings, refSettings, exp10, refInitialCapacity, dcMax, std::move(fnRefCalc));
        MB2ReferenceGenerationResult<Num> generationResult;
        if (!generator.generate(generationResult)) {
            return false;
        }

        *result = std::make_unique<MB2Reference>(generalSettings, refSettings, std::move(generationResult.center),
                                                 std::move(generationResult.srpcResult.compressor), std::move(generationResult.generatedPeriods),
                                                 std::move(generationResult.checkpoints), static_cast<complex<dex>>(generationResult.fpgBn),
                                                 generalSettings.logZoom, dcMax);
        (*result)->refOrbit = std::move(generationResult.srpcResult.compressedArray);

        return true;
    }

    template<Number Num>
    complex<Num> MB2Reference<Num>::orbit(const itt::ref refIteration) const {
        return compressor.empty() ? refOrbit[refIteration]
                                  : refOrbit[ArrayCompressorUtils::compress(compressor, refIteration)];
    }

    template<Number Num>
    size_t MB2Reference<Num>::length() const {
        return refOrbit.size();
    }
} // namespace merutilm::rff2
