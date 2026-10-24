//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include <memory>
#include <vector>

#include "../calc/iteration_types.hpp"
#include "../settings/FrtMPASettings.h"

namespace merutilm::rff2 {
    struct MPAPeriod {

        // generated period (it is different with reference period because it is contained artificially-generated periods)
        const std::vector<itt::ref> tablePeriods;

        // artificially-generated period flag
        const std::vector<bool> isArtificial;

        // the total count of skippable iterations count within current period
        const std::vector<itt::ref> skippableIterationCounts;

        // the total count of elements within current and lower level period
        const std::vector<itt::ref> tableElementCounts;

        explicit MPAPeriod(std::vector<itt::ref> &&tablePeriod, std::vector<bool> &&isArtificial, std::vector<itt::ref> &&skippableIterationCounts, std::vector<itt::ref> &&tableElementCounts);

        static void generateCountInfos(const std::vector<itt::ref> &tablePeriod,
                                       std::vector<itt::ref> &skippableIterationCounts,
                                       std::vector<itt::ref> &tableElementCounts);

        static void generateTablePeriod(const std::vector<itt::ref> &referencePeriod, const FrtMPASettings &mpaSettings,
                                        std::vector<itt::ref> &tablePeriods, std::vector<bool> &isArtificial);

        static std::unique_ptr<MPAPeriod> generate(const std::vector<itt::ref> &referencePeriod,
                                                 const FrtMPASettings &mpaSettings);
    };
}