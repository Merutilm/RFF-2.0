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
        size_t behindCompressionUnits = 0;

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
            ++refIndex;
            ++originalIndex;

            if (!currentIndexSegmentStack.empty()) {
                SRPCSegment<Index> *topPtr = &currentIndexSegmentStack.back();
                assert(topPtr->remained > 0);
                --topPtr->remained;
                if (topPtr->remained == 0) {
                    // if 'remained' is zero, pop and do recover
                    SRPCSegment popped = *topPtr;
                    assert(popped.nextChildSegmentCompressionUnitIndex <= data.compressor.size());
                    while (!currentIndexSegmentStack.empty() && topPtr->remained == 0) {
                        popped = *topPtr;
                        currentIndexSegmentStack.pop_back();
                        if (!currentIndexSegmentStack.empty()) {
                            assert(currentIndexSegmentStack.back().remained >= popped.maxCompressionSize);
                            currentIndexSegmentStack.back().remained -= popped.maxCompressionSize;
                            topPtr = &currentIndexSegmentStack.back();
                        }
                    }

                    if (currentIndexSegmentStack.empty()) {
                        ++behindCompressionUnits;
                    }

                    // recover reference index
                    refIndex = popped.referenceIndexToRecover;
                }
                if (topPtr->nextChildSegmentCompressionUnitIndex < behindCompressionUnits) {
                    const auto &unit = data.compressor[topPtr->nextChildSegmentCompressionUnitIndex];
                    if (topPtr->remained > 0 && unit.start - 1 == topPtr->maxCompressionSize - topPtr->remained) {
                        const auto maxCompressionSize = std::min(unit.range(), topPtr->remained);
                        // pushing child segment (referencing area may also be deleted by other compressor unit)
                        currentIndexSegmentStack.push_back(
                            {
                                .processingCompressionUnitIndex = topPtr->nextChildSegmentCompressionUnitIndex++,
                                .remained = maxCompressionSize,
                                .maxCompressionSize = maxCompressionSize,
                                .referenceIndexToRecover = refIndex,
                                .nextChildSegmentCompressionUnitIndex = 0
                            });
                        topPtr = &currentIndexSegmentStack[currentIndexSegmentStack.size() - 2];
                        // if vector resizes, top pointer may be a dangling pointer
                        refIndex = 1;
                    }
                }
            }
            const auto &nextCompressionUnit = data.compressor[behindCompressionUnits];
            if (nextCompressionUnit.start == originalIndex) {
                const auto maxCompressionSize = nextCompressionUnit.range();
                assert(currentIndexSegmentStack.empty());
                // pushing root segment
                currentIndexSegmentStack.push_back({
                    .processingCompressionUnitIndex = behindCompressionUnits,
                    .remained = maxCompressionSize,
                    .maxCompressionSize = maxCompressionSize,
                    .referenceIndexToRecover = refIndex,
                    .nextChildSegmentCompressionUnitIndex = 0,
                });
                refIndex = 1;
            }
        }
    };
} // namespace merutilm::rff2
