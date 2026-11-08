//
// Created by Merutilm on 11/8/26.
//

#pragma once
#include "../calc/templates.hpp"
#include "../mrthy/SRPCResult.hpp"
#include "ReferenceCheckpoint.hpp"
namespace merutilm::rff2 {

    enum class ReferenceCreationStatus : uint8_t { SUCCESS = 0, FAILED = 1, TERMINATED = 2, UNDEFINED = 3 };

    template <Number Num>
    struct MB2ReferenceGenerationResult {
        fixed_point_complex center;
        std::vector<itt::ref> generatedPeriods;
        complex<Num> fpgBn;
        std::vector<ReferenceCheckpoint> checkpoints;
        SRPCResult<complex<Num>> srpcResult;
        ReferenceCreationStatus creationStatus;

        static MB2ReferenceGenerationResult invalidValueSentinel(ReferenceCreationStatus status) {
            assert(status != ReferenceCreationStatus::SUCCESS);
            MB2ReferenceGenerationResult result{};
            result.creationStatus = status;
            return result;
        };
    };
}
