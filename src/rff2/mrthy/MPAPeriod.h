//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include <memory>
#include <vector>

#include "../attr/FrtMPAAttribute.h"

namespace merutilm::rff2 {
    struct MPAPeriod {
        const std::vector<uint64_t> tablePeriod;
        const std::vector<bool> isArtificial;
        const std::vector<uint64_t> tableElements;

        explicit MPAPeriod(std::vector<uint64_t> &&tablePeriod, std::vector<bool> &&isArtificial, std::vector<uint64_t> &&tableElements);

        struct Temp {
            std::vector<uint64_t> tablePeriod;
            std::vector<bool> isArtificial;
        };

        static std::vector<uint64_t> generatePeriodElements(const std::vector<uint64_t> &tablePeriod);

        static Temp generateTablePeriod(const std::vector<uint64_t> &referencePeriod, const FrtMPAAttribute &mpaSettings);

        static std::unique_ptr<MPAPeriod> create(const std::vector<uint64_t> &referencePeriod,
                                                 const FrtMPAAttribute &mpaSettings);
    };
}