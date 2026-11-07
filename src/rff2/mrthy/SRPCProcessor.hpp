//
// Created by Merutilm on 11/7/26.
//

#pragma once
#include <vector>


#include "../calc/iteration_types.hpp"
#include "SRPCResult.hpp"
#include "SRPCSegment.hpp"
namespace merutilm::rff2 {

    // Segment-based Recursive Pattern-approximate Compressor (SRPC)
    template<typename T, typename Approx>
    class SRPCProcessor {

        SRPCResult<T> result;
        itt::ref refIndex = 1;
        itt::ref originalIndex = 0;
        itt::ref compressStartIndex = 0;
        std::vector<SRPCSegment> segmentStack{};

        std::vector<T> tempBuffer{};
        bool minCompressSizeReached = false;
        uint32_t minCompressSize = 0; //set 0 to disable compression
        Approx approx;

    public:
        explicit SRPCProcessor(const uint32_t minCompressSize, size_t initialResultCapacity, Approx approx) : minCompressSize(minCompressSize), approx(std::move(approx)) {
            tempBuffer.reserve(minCompressSize);
            result.compressedArray.reserve(initialResultCapacity);
        }


        void put(const T &value) {
            if (minCompressSize > 0 && result.compressedArray.size() > 1 && approx.approximately(value, result.compressedArray[refIndex])) {
                assert(refIndex < result.compressedArray.size());
                processSegment(value);
            } else {
                if (!segmentStack.empty()) {
                    // clear and add compressor unit
                    refIndex = segmentStack[0].referenceIndexToRecover;
                    segmentStack.clear();
                    addCompressorIfEnough(originalIndex - 1); // without current index
                    minCompressSizeReached = false;
                }
                result.compressedArray.push_back(value);
            }
            ++originalIndex;
        }

        SRPCResult<T> finish() {
            if (!segmentStack.empty()) {
                refIndex = segmentStack[0].referenceIndexToRecover;
                segmentStack.clear();
                addCompressorIfEnough(originalIndex - 1);
            }
            return std::move(result);
        }
    private:

        void addCompressorIfEnough(itt::ref compressEndIndex) {
            if (minCompressSizeReached) {
                result.compressor.emplace_back(1, compressStartIndex, compressEndIndex);
            } else {
                for (const auto &v: tempBuffer) {
                    result.compressedArray.push_back(v);
                }
                tempBuffer.clear();
            }
        }

        void processSegment(const T &value) {
            if (segmentStack.empty()) {
                const auto maxCompressionSize = originalIndex - 1;
                // pushing root segment
                segmentStack.push_back({
                    .processingCompressionUnitIndex = result.compressor.size(),
                    .remained = maxCompressionSize,
                    .maxCompressionSize = maxCompressionSize,
                    .referenceIndexToRecover = 1,
                    .nextChildSegmentCompressionUnitIndex = 0,
                });
                compressStartIndex = originalIndex;
                refIndex = 1;
            }

            SRPCSegment *topPtr = &segmentStack.back();

            --topPtr->remained;
            ++refIndex;

            // push_back or clear
            if (!minCompressSizeReached) {
                if (tempBuffer.size() < minCompressSize) {
                    tempBuffer.push_back(value);
                }
                if (tempBuffer.size() == minCompressSize) {
                    minCompressSizeReached = true;
                    tempBuffer.clear();
                }
            }


            if (topPtr->nextChildSegmentCompressionUnitIndex < result.compressor.size()) {
                const auto &unit = result.compressor[topPtr->nextChildSegmentCompressionUnitIndex];
                if (topPtr->remained > 0 && unit.start - unit.rebase == topPtr->maxCompressionSize - topPtr->remained) {
                    const auto maxCompressionSize = std::min(unit.range(), topPtr->remained);
                    // pushing child segment (referencing area may also be deleted by other compressor unit)
                    segmentStack.push_back({
                        .processingCompressionUnitIndex = topPtr->nextChildSegmentCompressionUnitIndex++,
                        .remained = maxCompressionSize,
                        .maxCompressionSize = maxCompressionSize,
                        .referenceIndexToRecover = refIndex,
                        .nextChildSegmentCompressionUnitIndex = 0
                    });
                    topPtr = &segmentStack[segmentStack.size() - 2];
                    // if vector resizes, top pointer may be a dangling pointer
                    refIndex = 1;
                }
            }
            if (topPtr->remained == 0) {
                // if 'remained' is zero, pop and do recover
                SRPCSegment popped = *topPtr;
                assert(popped.nextChildSegmentCompressionUnitIndex <= result.compressor.size());
                while (!segmentStack.empty() && topPtr->remained == 0) {
                    popped = *topPtr;
                    segmentStack.pop_back();
                    if (!segmentStack.empty()) {
                        segmentStack.back().remained -= popped.maxCompressionSize;
                        topPtr = &segmentStack.back();
                    }
                }

                if (segmentStack.empty()) {
                    // if root segment is deleted, add compressor unit
                    addCompressorIfEnough(originalIndex); // containing current index
                    minCompressSizeReached = false;
                }

                // recover reference index
                refIndex = popped.referenceIndexToRecover;
            }
        }
    };
}
