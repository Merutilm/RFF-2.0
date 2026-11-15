//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include <vector>

#include "../calc/iteration_types.hpp"
#include "../settings/FrtMPASettings.h"

namespace merutilm::rff2 {
    struct MPAInfo {

        // generated period (it is different with reference period because it is contained artificially-generated periods)
        std::vector<itt::ref> tablePeriods;

        // artificially-generated period flag
        std::vector<bool> isArtificial;

        // the total count of skippable iterations count within current period
        std::vector<itt::ref> skippableIterationCounts;

        // the total count of elements within current and lower level period
        std::vector<itt::ref> tableElementCounts;


        MPAInfo() = default;

        explicit MPAInfo(const std::vector<itt::ref> &referencePeriod, const FrtMPASettings &mpaSettings);

    private:
        void generateCountInfos();

        void generateTablePeriod(const std::vector<itt::ref> &referencePeriod, const FrtMPASettings &mpaSettings);

    };
}