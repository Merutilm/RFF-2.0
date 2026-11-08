//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../util/ArrayCompressorUtils.hpp"
#include "SRPCData.hpp"
namespace merutilm::rff2 {
    template<typename Index, typename Data>
    struct SRPCView {
        const SRPCData<Index, Data> *data;

        SRPCView() : data(nullptr) {}
        explicit SRPCView(const SRPCData<Index, Data> &data) : data(&data) {}

        Data get(const Index i) const {
            return data->compressor.empty() ? data->compressedArray[i]
                                           : data->compressedArray[ArrayCompressorUtils::compress(data->compressor, i)];
        }
    };
} // namespace merutilm::rff2
