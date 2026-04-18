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

        // the count of elements of each level period
        const std::vector<uint64_t> tableElements;

        explicit MPAPeriod(std::vector<uint64_t> &&tablePeriod, std::vector<bool> &&isArtificial, std::vector<uint64_t> &&tableElements);

        struct Temp {
            std::vector<uint64_t> tablePeriod;
            std::vector<bool> isArtificial;
        };

        static std::vector<uint64_t> generatePeriodElements(const std::vector<uint64_t> &tablePeriod);

        static Temp generateTablePeriod(const std::vector<uint64_t> &referencePeriod, const FrtMPASettings &mpaSettings);

        static std::unique_ptr<MPAPeriod> generate(const std::vector<uint64_t> &referencePeriod,
                                                 const FrtMPASettings &mpaSettings);
    };
}