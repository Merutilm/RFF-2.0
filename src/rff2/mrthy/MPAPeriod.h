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
        const std::vector<uint64_t> tablePeriod;

        // artificially-generated period flag
        const std::vector<bool> isArtificial;

        // the total count of skippable iterations count within current period
        const std::vector<uint64_t> skippableIterationsCount;

        explicit MPAPeriod(std::vector<uint64_t> &&tablePeriod, std::vector<bool> &&isArtificial, std::vector<uint64_t> &&nonEmptyMpaCount);

        struct Temp {
            std::vector<uint64_t> tablePeriod;
            std::vector<bool> isArtificial;
        };

        static std::vector<uint64_t> generateSkippableIterationsCount(const std::vector<uint64_t> &tablePeriod);

        static Temp generateTablePeriod(const std::vector<uint64_t> &referencePeriod, const FrtMPASettings &mpaSettings);

        static std::unique_ptr<MPAPeriod> generate(const std::vector<uint64_t> &referencePeriod,
                                                 const FrtMPASettings &mpaSettings);
    };
}