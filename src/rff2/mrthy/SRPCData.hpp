//
// Created by Merutilm on 11/8/26.
//

#pragma once
#include <vector>

#include "SRPCUnit.hpp"
namespace merutilm::rff2 {
    template<typename Index, typename Data>
    struct SRPCData {
        Index uncompressedSize = 0;
        std::vector<Data> compressedArray{};
        std::vector<SRPCUnit<Index>> compressor{};
    };
}
