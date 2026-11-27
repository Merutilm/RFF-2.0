//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../algorithm/ReferenceCheckpoint.hpp"
#include "../algorithm/srpc_module/SRPCDataView.hpp"
#include "../calc/complex.hpp"
#include "../calc/iteration_types.hpp"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtReferenceSettings.hpp"
#include "HNREReferenceData.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    class HNREReferenceView;

    struct ReferenceDataBase {
        FrtGeneralSettings generalSettings;
        FrtReferenceSettings refSettings;
        fixed_point_complex c;

        std::vector<ReferenceCheckpoint> highestHNRECheckpoints;

        dex dcMax;

        ReferenceDataBase() = default;

        explicit ReferenceDataBase(FrtGeneralSettings generalSettings, FrtReferenceSettings refSettings,
                                   fixed_point_complex &&c,
                                   std::vector<ReferenceCheckpoint> &&highestHNRECheckpoints, const dex dcMax) :
            generalSettings(std::move(generalSettings)), refSettings(std::move(refSettings)), c(std::move(c)),
        highestHNRECheckpoints(std::move(highestHNRECheckpoints)), dcMax(dcMax) {}

        virtual ~ReferenceDataBase() = default;

        [[nodiscard]] virtual itt::ref calcPeriod() const = 0;

        [[nodiscard]] virtual const HNREDataBase &highestHNREData() const = 0;

        [[nodiscard]] virtual size_t calcTotalLength() const = 0;

        [[nodiscard]] virtual size_t hnreCount() const = 0;
    };

    template<Number Num>
    struct ReferenceData : ReferenceDataBase {
        std::vector<HNREReferenceData<Num>> hnreList;

        explicit ReferenceData(FrtGeneralSettings generalSettings,
                               FrtReferenceSettings refSettings, fixed_point_complex &&c, std::vector<HNREReferenceData<Num>> &&hnreList,
                               std::vector<ReferenceCheckpoint> &&highestHNRECheckpoints, const dex dcMax) :
            ReferenceDataBase(std::move(generalSettings), std::move(refSettings), std::move(c),
                              std::move(highestHNRECheckpoints), dcMax),
            hnreList(std::move(hnreList)) {}

        [[nodiscard]] size_t calcTotalLength() const override {
            size_t length = 0;
            for (auto &hnreData : hnreList) {
                length += hnreData.getCompressedArray().size();
            }
            return length;
        }

        [[nodiscard]] itt::ref calcPeriod() const override {
            itt::ref period = 1;
            for (auto &hnreData : hnreList) {
                period *= hnreData.generatedPeriods.back();
            }
            return period;
        }

        [[nodiscard]] const HNREDataBase &highestHNREData() const override {
            return hnreList.back();
        }

        [[nodiscard]] size_t hnreCount() const override {
            return hnreList.size();
        }

    };

} // namespace merutilm::rff2
