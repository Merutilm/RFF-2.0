//
// Created by Merutilm on 11/7/26.
//

#pragma once
#include <vector>

#include <cassert>
#include "SRPCData.hpp"
#include "SRPCSegment.hpp"
namespace merutilm::rff2 {

    // Segment-based Recursive Pattern-approximate Compression (SRPC)
    template<typename Index, typename Data, typename Approx>
    class SRPCProcessor {

        SRPCData<Index, Data> result;
        size_t refIndex = 1;
        Index compressStartIndex = 0;
        std::vector<SRPCSegment<Index>> segmentStack{};

        std::vector<Data> tempBuffer{};
        bool minCompressSizeReached = false;
        uint32_t minCompressSize = 0; // set 0 to disable compression
        Approx approx;

    public:
        explicit SRPCProcessor(const uint32_t minCompressSize, size_t initialResultCapacity, Approx approx) :
            minCompressSize(minCompressSize), approx(std::move(approx)) {
            tempBuffer.reserve(minCompressSize);
            result.compressedArray.reserve(initialResultCapacity);
        }


        void put(const Data &value) {
            if (minCompressSize > 0 && refIndex < result.compressedArray.size() &&
                approx.approximately(value, result.compressedArray[refIndex])) {
                processSegment(value);
            } else if (segmentStack.empty()) {
                result.compressedArray.push_back(value);
            } else {
                // clear and add compressor unit
                refIndex = segmentStack[0].referenceIndexToRecover; // 1
                segmentStack.clear();
                addCompressorIfEnough(result.uncompressedSize - 1); // without current index
                minCompressSizeReached = false;
                if (minCompressSize > 0 && refIndex < result.compressedArray.size() &&
                    approx.approximately(value, result.compressedArray[refIndex])) {
                    processSegment(value);
                } else {
                    result.compressedArray.push_back(value);
                }
            }
            ++result.uncompressedSize;
        }

        SRPCData<Index, Data> finish() && {
            if (!segmentStack.empty()) {
                refIndex = segmentStack[0].referenceIndexToRecover;
                segmentStack.clear();
                addCompressorIfEnough(result.uncompressedSize - 1);
            }
            return std::move(result);
        }

    private:
        void addCompressorIfEnough(Index compressEndIndex) {
            if (minCompressSizeReached) {
                result.compressor.emplace_back(compressStartIndex, compressEndIndex);
            } else {
                for (const auto &v: tempBuffer) {
                    result.compressedArray.push_back(v);
                }
                tempBuffer.clear();
            }
        }

        void processSegment(const Data &value) {
            if (segmentStack.empty()) {
                const auto maxCompressionSize = result.uncompressedSize - 1;
                // pushing root segment
                segmentStack.push_back({
                        .processingCompressionUnitIndex = result.compressor.size(),
                        .remained = maxCompressionSize,
                        .maxCompressionSize = maxCompressionSize,
                        .referenceIndexToRecover = 1,
                        .nextChildSegmentCompressionUnitIndex = 0,
                });
                compressStartIndex = result.uncompressedSize;
                refIndex = 1;
            }

            SRPCSegment<Index> *topPtr = &segmentStack.back();
            assert(topPtr->remained > 0);
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

            if (topPtr->remained == 0) {
                // if 'remained' is zero, pop and do recover
                SRPCSegment popped = *topPtr;
                assert(popped.nextChildSegmentCompressionUnitIndex <= result.compressor.size());
                while (!segmentStack.empty() && topPtr->remained == 0) {
                    popped = *topPtr;
                    segmentStack.pop_back();
                    if (!segmentStack.empty()) {
                        assert(segmentStack.back().remained >= popped.maxCompressionSize);
                        segmentStack.back().remained -= popped.maxCompressionSize;
                        topPtr = &segmentStack.back();
                    }
                }

                if (segmentStack.empty()) {
                    // if root segment is deleted, add compressor unit
                    addCompressorIfEnough(result.uncompressedSize); // containing current index
                    minCompressSizeReached = false;
                }

                // recover reference index.
                refIndex = popped.referenceIndexToRecover;
            }
            if (topPtr->nextChildSegmentCompressionUnitIndex < result.compressor.size()) {
                const auto &unit = result.compressor[topPtr->nextChildSegmentCompressionUnitIndex];
                if (topPtr->remained > 0 && unit.start - 1 == topPtr->maxCompressionSize - topPtr->remained) {
                    const auto maxCompressionSize = std::min(unit.range(), topPtr->remained);
                    // pushing child segment (referencing area may also be deleted by other compressor unit)
                    segmentStack.push_back(
                            {.processingCompressionUnitIndex = topPtr->nextChildSegmentCompressionUnitIndex++,
                             .remained = maxCompressionSize,
                             .maxCompressionSize = maxCompressionSize,
                             .referenceIndexToRecover = refIndex,
                             .nextChildSegmentCompressionUnitIndex = 0});
                    topPtr = &segmentStack[segmentStack.size() - 2];
                    // if vector resizes, top pointer may be a dangling pointer
                    refIndex = 1;
                }
            }
        }
    };
} // namespace merutilm::rff2
