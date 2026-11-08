//
// Created by Merutilm on 11/8/26.
//

#pragma once
#include "../calc/templates.hpp"
#include "../mrthy/SRPCData.hpp"
#include "ReferenceCheckpoint.hpp"
namespace merutilm::rff2 {

    template <Number Num>
    struct MB2ReferenceGenerationResult {
        fixed_point_complex center;
        std::vector<itt::ref> generatedPeriods;
        complex<Num> fpgBn;
        std::vector<ReferenceCheckpoint> checkpoints;
        SRPCData<itt::ref, complex<Num>> srpcData;

    };
}
