//
// Created by Merutilm on 11/7/26.
//

#pragma once

#include "../calc/iteration_types.hpp"
namespace merutilm::rff2 {
    struct CompressionProcessorSegment {
        size_t processingCompressionUnitIndex;
        itt::ref remained = 0;
        itt::ref maxCompressionSize = 0;
        size_t referenceIndexToRecover = 0;
        size_t nextChildSegmentCompressionUnitIndex = 0;
    };
}
