//
// Created by Merutilm on 2025-05-04.
//

#pragma once


namespace merutilm::rff2 {
    struct FrtReferenceCompAttribute final{
        uint32_t compressCriteria;
        uint8_t compressionThresholdPower;
        bool noCompressorNormalization;
    };
}