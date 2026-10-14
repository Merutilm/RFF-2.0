//
// Created by Merutilm on 10/14/26.
//

#pragma once
#include <cstdint>

namespace merutilm::rff2 {

    template <typename T>
    struct SIMDChunkData {
        uint32_t x = 0;
        uint32_t y = 0;
        T *value = nullptr;
    };
}