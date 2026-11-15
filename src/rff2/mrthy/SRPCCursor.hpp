//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "SRPCSegment.hpp"
#include "SRPCUtils.hpp"
namespace merutilm::rff2 {

    template<typename Index>
    class SRPCCursor {
    protected:
        const std::vector<SRPCUnit<Index>> &compressor;
        std::vector<SRPCSegment<Index> > currentIndexSegmentStack;

        Index originalIndex = 0;
        size_t refIndex = 0;
        size_t behindCompressionUnits = 0;

    public:
        explicit SRPCCursor(const std::vector<SRPCUnit<Index>> &compressor) : compressor(compressor) {
        }

        void reset() {
            originalIndex = 0;
            refIndex = 0;
            behindCompressionUnits = 0;
            currentIndexSegmentStack.clear();
        }

        [[nodiscard]] Index currentOriginalIndex() const{
            return originalIndex;
        }


        [[nodiscard]] Index currentRefIndex() const{
            return refIndex;
        }

        [[nodiscard]] size_t calcRefIndexFast(Index i) const {
            return compressor.empty()
                       ? i
                       : SRPCUtils::compress(compressor, i);
        }

        [[nodiscard]] size_t calcRefIndex(Index i) {
            return compressor.empty()
                       ? i
                       : SRPCUtils::compress(compressor, i, &currentIndexSegmentStack, &behindCompressionUnits);
        }

        // Time complexity O(log N)
        void setIndex(Index newIndex) {
            currentIndexSegmentStack.clear();
            originalIndex = newIndex;
            refIndex = calcRefIndex(newIndex);
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
                    assert(popped.nextChildSegmentCompressionUnitIndex <= compressor.size());
                    while (!currentIndexSegmentStack.empty() && topPtr->remained == 0) {
                        popped = *topPtr;
                        currentIndexSegmentStack.pop_back();
                        if (currentIndexSegmentStack.empty()) {
                            topPtr = nullptr;
                            ++behindCompressionUnits;
                        } else {
                            assert(currentIndexSegmentStack.back().remained >= popped.maxCompressionSize);
                            currentIndexSegmentStack.back().remained -= popped.maxCompressionSize;
                            topPtr = &currentIndexSegmentStack.back();
                        }
                    }
                    // recover reference index
                    refIndex = popped.referenceIndexToRecover;
                }
                if (topPtr && topPtr->nextChildSegmentCompressionUnitIndex < compressor.size()) {
                    const auto &unit = compressor[topPtr->nextChildSegmentCompressionUnitIndex];
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
            if (behindCompressionUnits < compressor.size()) {
                const auto &nextCompressionUnit = compressor[behindCompressionUnits];
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
        }
    };
} // namespace merutilm::rff2
