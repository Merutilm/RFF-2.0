//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include <algorithm>
#include <cassert>
#include "SRPCSegment.hpp"
#include "SRPCUtils.hpp"


namespace merutilm::rff2 {
    template<typename Index>
    class SRPCCursor {
    protected:
        const std::vector<SRPCUnit<Index>> &compressor;
        std::vector<SRPCSegment<Index>> currentIndexSegmentStack;

        Index originalIndex = 0;
        size_t refIndex = 0;
        size_t startedUnitCount = 0;

    public:
        explicit SRPCCursor(const std::vector<SRPCUnit<Index>> &compressor);

        void reset();

        [[nodiscard]] Index currentOriginalIndex() const;


        [[nodiscard]] Index currentRefIndex() const;

        [[nodiscard]] size_t calcRefIndexFast(Index i) const;

        [[nodiscard]] size_t calcRefIndex(Index i);

        // Time complexity O(log N)
        void setIndex(Index newIndex);


        void jump(Index increment);

        // Time complexity O(1)
        void next();

    private:
        // decrement cannot be zero
        [[nodiscard]] Index calculateDecrement(Index increment);


        Index consume(Index increment);

        void popEndedSegments(SRPCSegment<Index> *&topPtr);
    };

    template<typename Index>
    SRPCCursor<Index>::SRPCCursor(const std::vector<SRPCUnit<Index>> &compressor) : compressor(compressor) {}

    template<typename Index>
    void SRPCCursor<Index>::reset() {
        originalIndex = 0;
        refIndex = 0;
        startedUnitCount = 0;
        currentIndexSegmentStack.clear();
    }

    template<typename Index>
    Index SRPCCursor<Index>::currentOriginalIndex() const {
        return originalIndex;
    }

    template<typename Index>
    Index SRPCCursor<Index>::currentRefIndex() const {
        return refIndex;
    }

    template<typename Index>
    size_t SRPCCursor<Index>::calcRefIndexFast(Index i) const {
        return compressor.empty() ? i : SRPCUtils::compress(compressor, i);
    }

    template<typename Index>
    size_t SRPCCursor<Index>::calcRefIndex(Index i) {
        return compressor.empty() ? i
                                  : SRPCUtils::compress(compressor, i, &currentIndexSegmentStack, &startedUnitCount);
    }

    template<typename Index>
    void SRPCCursor<Index>::setIndex(Index newIndex) {
        currentIndexSegmentStack.clear();
        originalIndex = newIndex;
        refIndex = calcRefIndex(newIndex);
    }

    template<typename Index>
    void SRPCCursor<Index>::jump(Index increment) {
        while (increment > 0) {
            increment = consume(increment);
        }
    }

    template<typename Index>
    void SRPCCursor<Index>::next() {
        consume(1);
    }


    template<typename Index>
    Index SRPCCursor<Index>::calculateDecrement(Index increment) {
        assert(increment > 0);
        Index decrement = increment;

        if (!currentIndexSegmentStack.empty()) {
            SRPCSegment<Index> *topPtr = &currentIndexSegmentStack.back();
            assert(topPtr->remained > 0);
            decrement = std::min(topPtr->remained, decrement);

            if (decrement < topPtr->remained && topPtr->nextChildUnitIndex < compressor.size()) {
                Index nextChildPushUntil = compressor[topPtr->nextChildUnitIndex].start - topPtr->maxCompressionSize +
                                           topPtr->remained - 1;
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

    template<typename Index>
    Index SRPCCursor<Index>::consume(Index increment) {
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
                    currentIndexSegmentStack.push_back({.remained = maxCompressionSize,
                                                        .maxCompressionSize = maxCompressionSize,
                                                        .referenceIndexToRecover = refIndex,
                                                        .nextChildUnitIndex = 0});
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

    template<typename Index>
    void SRPCCursor<Index>::popEndedSegments(SRPCSegment<Index> *&topPtr) {
        SRPCUtils::popEndedSegments(topPtr, currentIndexSegmentStack, refIndex);
    }
} // namespace merutilm::rff2
