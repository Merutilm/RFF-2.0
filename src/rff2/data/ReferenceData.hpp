//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../algorithm/HNREInfo.hpp"
#include "../algorithm/ReferenceCheckpoint.hpp"
#include "../algorithm/srpc_module/SRPCData.hpp"
#include "../algorithm/srpc_module/SRPCDataView.hpp"
#include "../calc/complex.hpp"
#include "../calc/iteration_types.hpp"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtReferenceSettings.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    class ReferenceView;

    struct ReferenceDataBase {
        FrtGeneralSettings generalSettings;
        FrtReferenceSettings refSettings;
        fixed_point_complex c;
        HNREInfo hnreInfo;
        std::vector<itt::ref> generatedPeriods;
        std::vector<ReferenceCheckpoint> checkpoints;
        complex<dex> fpgBn;
        dex dcMax;

        ReferenceDataBase() = default;

        explicit ReferenceDataBase(FrtGeneralSettings generalSettings, FrtReferenceSettings refSettings,
                                   fixed_point_complex &&c, HNREInfo &&hnreInfo,
                                   std::vector<itt::ref> &&generatedPeriods,
                                   std::vector<ReferenceCheckpoint> &&checkpoints, complex<dex> &&fpgBn,
                                   const dex dcMax) :
            generalSettings(std::move(generalSettings)), refSettings(std::move(refSettings)), c(std::move(c)),
            hnreInfo(std::move(hnreInfo)), generatedPeriods(std::move(generatedPeriods)),
            checkpoints(std::move(checkpoints)), fpgBn(std::move(fpgBn)), dcMax(dcMax) {}

        virtual ~ReferenceDataBase() = default;

        [[nodiscard]] itt::ref longestPeriod() const { return generatedPeriods.back(); }
        [[nodiscard]] virtual size_t length() const = 0;
    };

    template<Number Num>
    struct ReferenceData : SRPCData<itt::ref, complex<Num>>, ReferenceDataBase {

        explicit ReferenceData(SRPCData<itt::ref, complex<Num>> &&data, FrtGeneralSettings generalSettings,
                               FrtReferenceSettings refSettings, fixed_point_complex &&c,
                               HNREInfo &&hnreInfo, std::vector<itt::ref> &&generatedPeriods,
                               std::vector<ReferenceCheckpoint> &&checkpoints, complex<dex> &&fpgBn, const dex dcMax) :
            SRPCData<itt::ref, complex<Num>>(std::move(data)),
            ReferenceDataBase(std::move(generalSettings), std::move(refSettings), std::move(c),
                              std::move(hnreInfo), std::move(generatedPeriods), std::move(checkpoints),
                              std::move(fpgBn), dcMax) {}

        [[nodiscard]] ReferenceView<Num> makeReferenceView() const;

        [[nodiscard]] size_t length() const override {
            return SRPCData<itt::ref, complex<Num>>::compressedArray.size();
        }

        [[nodiscard]] const std::vector<complex<Num>> &getCompressedArray() const {
            return SRPCData<itt::ref, complex<Num>>::compressedArray;
        }
        [[nodiscard]] const ReferenceCompressor &getCompressor() const {
            return SRPCData<itt::ref, complex<Num>>::compressor;
        }
    };

} // namespace merutilm::rff2
