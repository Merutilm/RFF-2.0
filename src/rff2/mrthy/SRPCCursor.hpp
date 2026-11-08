//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../util/ArrayCompressorUtils.hpp"
#include "SRPCData.hpp"
namespace merutilm::rff2 {

    template<typename Index, typename Data>
    struct SRPCCursor {
        const SRPCData<Index, Data> &data;
        Index index = 0;
        size_t refIndex = 0;

        explicit SRPCCursor(const SRPCData<Index, Data> &data) : data(data) {

        }

        void reset() {
            index = 0;
            refIndex = 0;
        }

        // Time complexity O(1)
        const Data &get() const {
            return data.compressedArray[refIndex];
        }

        // Time complexity O(log N loglog N)
        const Data &get(Index newIndex) {
            setIndex(newIndex);
            return get();
        }

        // Time complexity O(log N loglog N)
        void setIndex(Index newIndex) {
            index = newIndex;
            refIndex = data.compressor.empty() ? newIndex : ArrayCompressorUtils::compress(data.compressor, newIndex);
        }

        // Time complexity O(1)
        void next() {
            //TODO
        }

        // Time complexity O(1)
        void previous() {
            //TODO
        }
    };
}
