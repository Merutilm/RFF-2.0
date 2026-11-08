//
// Created by Merutilm on 11/8/26.
//

#pragma once
#include <vector>

#include "ArrayCompressUnit.hpp"
namespace merutilm::rff2 {
    template<typename Index, typename Data>
    struct SRPCData {
        std::vector<Data> compressedArray{};
        std::vector<ArrayCompressUnit<Index>> compressor{};
    };
}
