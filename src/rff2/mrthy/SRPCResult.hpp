//
// Created by Merutilm on 11/8/26.
//

#pragma once
#include <vector>

#include "ArrayCompressUnit.hpp"
namespace merutilm::rff2 {
    template<typename T>
    struct SRPCResult {
        std::vector<T> compressedArray{};
        ReferenceCompressor compressor{};
    };
}
