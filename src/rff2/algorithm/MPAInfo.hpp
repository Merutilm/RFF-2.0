//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include <vector>

#include "../calc/iteration_types.hpp"
#include "../settings/FrtMPASettings.h"
#include "srpc_module/SRPCUnit.hpp"


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

        // pulled mpa : fill only valid elements from the sparse mpa vector
        // pulled mpa compressor : distinct the elements from "pulled mpa"
        PulledMPACompressor pulledMPACompressor;

        std::vector<itt::ref> compressedSkippableIterationCounts;
        std::vector<size_t> compressionUnitCounts;

        MPAInfo() = default;

        explicit MPAInfo(const std::vector<itt::ref> &referencePeriod, const ReferenceCompressor &referenceCompressor, const FrtMPASettings &mpaSettings);

    private:
        void generateCountInfos();

        void generateTablePeriod(const std::vector<itt::ref> &referencePeriod, const FrtMPASettings &mpaSettings);

        void generatePulledMPACompressor(const ReferenceCompressor &referenceCompressor);

        void generateCompressionInfo();

        void printDebugInfo();
    };
}