//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "SRPCSegment.hpp"
#include "SRPCUtils.hpp"
#include <cassert>
#include <algorithm>


namespace merutilm::rff2 {
    template<typename Index>
    class SRPCCursor {
    protected:
        const std::vector<SRPCUnit<Index> > &compressor;
        std::vector<SRPCSegment<Index> > currentIndexSegmentStack;

        Index originalIndex = 0;
        size_t refIndex = 0;
        size_t startedUnitCount = 0;

    public:
        explicit SRPCCursor(const std::vector<SRPCUnit<Index> > &compressor) : compressor(compressor) {
        }

        void reset() {
            originalIndex = 0;
            refIndex = 0;
            startedUnitCount = 0;
            currentIndexSegmentStack.clear();
        }

        [[nodiscard]] Index currentOriginalIndex() const { return originalIndex; }


        [[nodiscard]] Index currentRefIndex() const { return refIndex; }

        [[nodiscard]] size_t calcRefIndexFast(Index i) const {
            return compressor.empty() ? i : SRPCUtils::compress(compressor, i);
        }

        [[nodiscard]] size_t calcRefIndex(Index i) {
            return compressor.empty()
                       ? i
                       : SRPCUtils::compress(compressor, i, &currentIndexSegmentStack, &startedUnitCount);
        }

        // Time complexity O(log N)
        void setIndex(Index newIndex) {
            currentIndexSegmentStack.clear();
            originalIndex = newIndex;
            refIndex = calcRefIndex(newIndex);
        }

        // decrement cannot be zero
        [[nodiscard]] Index calculateDecrement(Index increment) {
            assert(increment > 0);
            Index decrement = increment;

            if (!currentIndexSegmentStack.empty()) {
                SRPCSegment<Index> *topPtr = &currentIndexSegmentStack.back();
                assert(topPtr->remained > 0);
                decrement = std::min(topPtr->remained, decrement);

                if (decrement < topPtr->remained && topPtr->nextChildUnitIndex < compressor.size()) {
                    Index nextChildPushUntil = compressor[topPtr->nextChildUnitIndex].start - topPtr->maxCompressionSize + topPtr->
                    remained - 1;
                    decrement = std::min(decrement, nextChildPushUntil);
                }
            }
            if (startedUnitCount < compressor.size()) {
                const auto &nextCompressionUnit = compressor[startedUnitCount];
                Index rootPushUntil = nextCompressionUnit.start - originalIndex;
                decrement = std::min(rootPushUntil, decrement);
            }

            assert(decrement > 0);
            return decrement;
        }


        Index consume(Index increment) {
            Index decrement = calculateDecrement(increment);
            originalIndex += decrement;
            refIndex += decrement;
            increment -= decrement;

            if (!currentIndexSegmentStack.empty()) {
                SRPCSegment<Index> *topPtr = &currentIndexSegmentStack.back();
                assert(topPtr->remained > 0);
                topPtr->remained -= decrement;
                popEndedSegments(topPtr);

                if (topPtr && topPtr->nextChildUnitIndex < compressor.size()) {
                    const SRPCUnit<Index> &unit = compressor[topPtr->nextChildUnitIndex];
                    if (topPtr->remained > 0 && unit.start - 1 == topPtr->maxCompressionSize - topPtr->remained) {
                        const auto maxCompressionSize = std::min(unit.range(), topPtr->remained);
                        // pushing child segment (referencing area may also be deleted by other compressor unit)
                        ++topPtr->nextChildUnitIndex;
                        currentIndexSegmentStack.push_back({
                            .remained = maxCompressionSize,
                            .maxCompressionSize = maxCompressionSize,
                            .referenceIndexToRecover = refIndex,
                            .nextChildUnitIndex = 0
                        });
                        topPtr = &currentIndexSegmentStack[currentIndexSegmentStack.size() - 2];
                        // if vector resizes, top pointer may be a dangling pointer
                        refIndex = 1;
                    }
                }
            }

            if (startedUnitCount < compressor.size()) {
                const auto &nextCompressionUnit = compressor[startedUnitCount];
                if (nextCompressionUnit.start == originalIndex) {
                    const auto maxCompressionSize = nextCompressionUnit.range();
                    assert(currentIndexSegmentStack.empty());
                    // pushing root segment
                    ++startedUnitCount;
                    currentIndexSegmentStack.push_back({
                        .remained = maxCompressionSize,
                        .maxCompressionSize = maxCompressionSize,
                        .referenceIndexToRecover = refIndex,
                        .nextChildUnitIndex = 0,
                    });
                    refIndex = 1;
                }
            }
            return increment;
        }

        void jump(Index increment) {
            while (increment > 0) {
                increment = consume(increment);
            }
        }

        // Time complexity O(1)
        void next() {
            consume(1);
        }

        void popEndedSegments(SRPCSegment<Index> *&topPtr) {
            popEndedSegments(topPtr, currentIndexSegmentStack, compressor, refIndex);
        }

        static void popEndedSegments(SRPCSegment<Index> *&topPtr,
                                     std::vector<SRPCSegment<Index> > &currentIndexSegmentStack,
                                     const std::vector<SRPCUnit<Index> > &compressor, Index &refIndex) {
            if (topPtr->remained > 0)
                return;

            // if 'remained' is zero, pop and do recover

            assert(topPtr->nextChildUnitIndex <= compressor.size());
            while (!currentIndexSegmentStack.empty() && topPtr->remained == 0) {
                refIndex = topPtr->referenceIndexToRecover;
                Index maxCompressionSize = topPtr->maxCompressionSize;
                currentIndexSegmentStack.pop_back();
                if (!currentIndexSegmentStack.empty()) {
                    assert(currentIndexSegmentStack.back().remained >= maxCompressionSize);
                    currentIndexSegmentStack.back().remained -= maxCompressionSize;
                    topPtr = &currentIndexSegmentStack.back();
                }
            }

            if (currentIndexSegmentStack.empty()) {
                topPtr = nullptr;
            }
            // recover reference index
        }
    };
} // namespace merutilm::rff2
