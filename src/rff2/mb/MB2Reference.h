//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "../app/FnListeners.hpp"
#include "../calc/fixed_point_complex.hpp"
#include "../mrthy/SRPCCursor.hpp"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtReferenceSettings.hpp"
#include "MB2ReferenceGenerationResult.hpp"
#include "MB2ReferenceGenerator.hpp"
#include "RFFResult.hpp"
#include "Reference.hpp"
#include "ReferenceCheckpoint.hpp"

namespace merutilm::rff2 {


    struct MB2ReferenceBase : Reference {

        const FrtGeneralSettings generalSettings;
        const FrtReferenceSettings refSettings;
        fixed_point_complex center;
        const std::vector<itt::ref> period;
        std::vector<ReferenceCheckpoint> checkpoints;
        const complex<dex> fpgBn;
        const double logZoom;
        const dex dcMax;

        MB2ReferenceBase(FrtGeneralSettings generalSettings, FrtReferenceSettings refSettings,
                         fixed_point_complex &&center, std::vector<itt::ref> &&period,
                         std::vector<ReferenceCheckpoint> &&checkpoints, complex<dex> fpgBn, const double logZoom,
                         const dex dcMax) :
            generalSettings(std::move(generalSettings)), refSettings(std::move(refSettings)), center(std::move(center)),
            period(std::move(period)), checkpoints(std::move(checkpoints)), fpgBn(std::move(fpgBn)), logZoom(logZoom),
            dcMax(dcMax) {}

        [[nodiscard]] itt::ref longestPeriod() const { return period.back(); }
    };

    template<Number Num>
    class MB2Reference final : public MB2ReferenceBase {

        SRPCData<itt::ref, complex<Num>> data;

    public:
        using MB2ReferenceBase::MB2ReferenceBase;


        template<FnListeners::FnRefCalc FnRefCalc>
        static RFFResult generateReference(const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                                           const FrtReferenceSettings &refSettings, int32_t exp10,
                                           size_t refInitialCapacity, dex dcMax, FnRefCalc &&fnRefCalc,
                                           std::unique_ptr<MB2Reference> &result);

        [[nodiscard]] SRPCCursor<itt::ref, complex<Num>> makeCursor() const {
            return SRPCCursor<itt::ref, complex<Num>>(data);
        }

        [[nodiscard]] size_t length() const override;

        [[nodiscard]] const std::vector<complex<Num>> &getCompressedArray() const {
            return data.compressedArray;
        }
        [[nodiscard]] const ReferenceCompressor &getCompressor() const {
            return data.compressor;
        }
    };


    template<Number Num>
    template<FnListeners::FnRefCalc FnRefCalc>
    RFFResult MB2Reference<Num>::generateReference(const ParallelRenderState &state,
                                                   const FrtGeneralSettings &generalSettings,
                                                   const FrtReferenceSettings &refSettings, int32_t exp10,
                                                   size_t refInitialCapacity, dex dcMax, FnRefCalc &&fnRefCalc,
                                                   std::unique_ptr<MB2Reference> &result) {
        MB2ReferenceGenerator<Num, FnRefCalc> generator(state, generalSettings, refSettings, exp10, refInitialCapacity,
                                                        dcMax, std::move(fnRefCalc));
        MB2ReferenceGenerationResult<Num> generationResult;
        if (const RFFResult r = generator.generate(generationResult); r != RFFResult::SUCCESS) {
            return r;
        }

        result = std::make_unique<MB2Reference>(
                generalSettings, refSettings, std::move(generationResult.center),
                std::move(generationResult.generatedPeriods), std::move(generationResult.checkpoints),
                static_cast<complex<dex>>(generationResult.fpgBn), generalSettings.logZoom, dcMax);
        result->data = std::move(generationResult.srpcData);

        return RFFResult::SUCCESS;
    }

    template<Number Num>
    size_t MB2Reference<Num>::length() const {
        return data.compressedArray.size();
    }
} // namespace merutilm::rff2
