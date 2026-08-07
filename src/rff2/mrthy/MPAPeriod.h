//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include <memory>
#include <vector>

#include "../settings/FrtMPASettings.h"

namespace merutilm::rff2 {
    struct MPAPeriod {

        // generated period (it is different with reference period because it is contained artificially-generated periods)
        const std::vector<uint64_t> tablePeriods;

        // artificially-generated period flag
        const std::vector<bool> isArtificial;

        // the total count of skippable iterations count within current period
        const std::vector<uint64_t> skippableIterationCounts;

        // the total count of elements within current and lower level period
        const std::vector<uint64_t> tableElementCounts;

        explicit MPAPeriod(std::vector<uint64_t> &&tablePeriod, std::vector<bool> &&isArtificial, std::vector<uint64_t> &&skippableIterationCounts, std::vector<uint64_t> &&tableElementCounts);

        static void generateCountInfos(const std::vector<uint64_t> &tablePeriod,
                                       std::vector<uint64_t> &skippableIterationCounts,
                                       std::vector<uint64_t> &tableElementCounts);

        static void generateTablePeriod(const std::vector<uint64_t> &referencePeriod, const FrtMPASettings &mpaSettings,
                                        std::vector<uint64_t> &tablePeriods, std::vector<bool> &isArtificial);

        static std::unique_ptr<MPAPeriod> generate(const std::vector<uint64_t> &referencePeriod,
                                                 const FrtMPASettings &mpaSettings);
    };
}