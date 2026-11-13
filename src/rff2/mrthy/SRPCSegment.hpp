//
// Created by Merutilm on 11/7/26.
//

#pragma once

namespace merutilm::rff2 {
    template<typename Index>
    struct SRPCSegment {
        size_t processingCompressionUnitIndex = 0;
        Index remained = 0;
        Index maxCompressionSize = 0;
        size_t referenceIndexToRecover = 0;
        size_t nextChildSegmentCompressionUnitIndex = 0;
    };
}
