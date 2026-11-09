//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../util/ArrayCompressorUtils.hpp"
#include "SRPCData.hpp"
#include "SRPCSegment.hpp"
namespace merutilm::rff2 {

    template<typename Index, typename Data>
    class SRPCCursor {
        const SRPCData<Index, Data> &data;
        std::vector<const ArrayCompressionUnit<Index> *> rawStackTrace;
        std::vector<SRPCSegment<Index>> currentIndexSegmentStack;
        Index originalIndex = 0;
        size_t refIndex = 0;
    public:
        explicit SRPCCursor(const SRPCData<Index, Data> &data) : data(data) {}

        void reset() {
            originalIndex = 0;
            refIndex = 0;
            currentIndexSegmentStack.clear();
        }

        // Time complexity O(1)
        const Data &get() const { return data.compressedArray[refIndex]; }

        // Time complexity O(log N loglog N)
        const Data &get(Index i) {
            return data.compressedArray[calcRefIndex(i, nullptr)];
        }

        [[nodiscard]] size_t calcRefIndex(Index i, std::vector<const ArrayCompressionUnit<Index> *> *stackTrace) const {
            return data.compressor.empty() ? i : ArrayCompressorUtils::compress(data.compressor, i, stackTrace);
        }

        // Time complexity O(log N loglog N)
        void setIndex(Index newIndex) {
            rawStackTrace.clear();
            originalIndex = newIndex;
            refIndex = calcRefIndex(newIndex, &rawStackTrace);
            for (auto unit : rawStackTrace) {
                //TODO
            }

        }

        // Time complexity O(1)
        void next() {
            // TODO
        }
    };
} // namespace merutilm::rff2
