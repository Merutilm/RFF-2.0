//
// Created by Merutilm on 11/8/26.
//

#pragma once
#include <vector>

#include "SRPCUnit.hpp"
namespace merutilm::rff2 {
    template<typename Index, typename Element>
    struct SRPCData {
        Index uncompressedSize = 0;
        std::vector<Element> compressedArray{};
        std::vector<SRPCUnit<Index>> compressor{};

    };
}
