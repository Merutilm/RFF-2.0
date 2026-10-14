//
// Created by Merutilm on 10/14/26.
//

#pragma once
#include <array>
#include <cstdint>

#include "SIMDChunkData.hpp"

namespace merutilm::rff2 {

    template<typename T>
    struct SIMDChunk {
        uint32_t validLen = 0;
        uint32_t xRes = 1;
        uint32_t yRes = 1;
        std::array<SIMDChunkData<T>, RFF_SIMD_CHUNK_SIZE> data{};
    };
}