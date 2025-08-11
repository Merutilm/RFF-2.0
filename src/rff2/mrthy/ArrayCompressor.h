//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <cstdint>
#include <vector>

#include "ArrayCompressionTool.h"


namespace merutilm::rff2 {
    /**
     * <b>Array compressor</b><br/>
     * A class for compressing when detecting duplicated elements in an array. <br/>
     * It can be used if there are duplicate values in the array. <br/>
     * <b> The compression process in this is divided into two steps: </b> <br/>
     * <li> 1. rebases the given index based on its data.</li>
     * <li> 2. pulls the subsequent elements to fill the empty space.</li> <br/>
     *
     * <b>NOTE : It was get 10% ~ 20% performance improvement by moving all the methods to the header file.</b>
     */
    struct ArrayCompressor {
        ArrayCompressor() = delete;

        /**
         * Checks whether the given index is independent. <br/>
         * The time complexity is @code O(log N)@endcode because it uses binary-search.
         *
         * @param tools The Compression tools
         * @param index To checking index
         * @return @code true@endcode when given index is independent.
         */
        static bool isIndependent(const std::vector<ArrayCompressionTool> &tools, uint64_t index);

        /**
         * Rebases the given index. <br/>
         * The rebased index is not the compressed index because it is a middle step that has not proceeded the pulling process. <br/>
         * To get the final index, use the pull() method.
         *
         * @param tools The Compression tools
         * @param index To rebasing index
         * @return The rebased index.
         */
        static uint64_t rebase(const std::vector<ArrayCompressionTool> &tools, uint64_t index);

        /**
         * Completes the compressed array by pulling subsequent elements into the void in the array made by the rebasing process. <br/>
         * You can get the final index of the compressed array. <br/>
         * <b>This method assumes that the given index has been completed the rebase process and the given list is the same as the list used in the rebasing process.</b>
         *
         * @param tools The Compression tools
         * @param index To pulling rebased index
         * @return The finally compressed index.
         */
        static uint64_t pull(const std::vector<ArrayCompressionTool> &tools, uint64_t index);

        /**
         * Gets the contained index of the given list. <br/>
         * The time complexity is @code O(log N)@endcode
         *
         * @param tools The Compression tools
         * @param index To checking index
         * @return The index of the given list. returns @code UINT64_MAX()@endcode if the given index is un-rebase-able.
         * @see ArrayCompressor#binarySearch(long, int, int, int) binarySearch
         */
        static uint64_t containedIndex(const std::vector<ArrayCompressionTool> &tools, uint64_t index);

        /**
         * Gets the compressed index of compressed array.
         *
         * @param tools The Compression tools
         * @param index the index you want
         * @return Finally compressed index
         */
        static uint64_t compress(const std::vector<ArrayCompressionTool> &tools, uint64_t index);

    private:
        /**
        * A single step of recursive binary-search to get the contained index of the list. <br/>
        *
        * @param tools        The Compression tools
        * @param index        To checking index
        * @param compIndex    The index of given list
        * @param indexGap     The index gap
        * @param lastIndexGap The last index Gap
        * @return The index of the given list. returns @code UINT64_MAX@endcode if the given index is un-rebase-able.
        */
        static uint64_t binarySearch(const std::vector<ArrayCompressionTool> &tools, uint64_t index,
                                     uint64_t compIndex, uint64_t indexGap, uint64_t lastIndexGap);
    };

    // DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR
    // DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR
    // DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR
    // DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR
    // DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR  DEFINITION OF ARRAY_COMPRESSOR


    inline bool ArrayCompressor::isIndependent(const std::vector<ArrayCompressionTool> &tools, const uint64_t index) {
        return containedIndex(tools, index) == UINT64_MAX;
    }

    inline uint64_t ArrayCompressor::rebase(const std::vector<ArrayCompressionTool> &tools, const uint64_t index) {
        if (tools.empty() || index < tools.front().start || index > tools.back().end) {
            return index;
        }

        // Since large value are compressed to small, and it compresses again.

        // Example
        // 10000 -> 15000 -> 1 -> 5001
        // 1000 -> 1500 -> 1 -> 501
        // 100 -> 150 = 1 -> 51
        //
        // test input : 11111
        // 11111 -> 11111 - 10000 + 1 = 1112
        // 1112 -> 1112 - 1000 + 1 = 113,
        // 113 -> 113 - 100 + 1 = 14
        //
        // test input : 1400
        // 1400 -> 1400 - 1000 + 1 = 401

        // Use the REVERSED FOR statement because it must be used recursively.
        uint64_t rebased = index;

        for (uint64_t i = tools.size(); i > 0; --i) {
            if (auto &tool = tools[i - 1];
                tool.start <= rebased && rebased <= tool.end) {
                rebased -= tool.start - tool.rebase;
                } else if (rebased > tool.end) {
                    break;
                }
        }
        return rebased;
    }

    inline uint64_t ArrayCompressor::pull(const std::vector<ArrayCompressionTool> &tools, const uint64_t index) {
        // "Rebased" iteration is not a target of compressions.
        // The space created by compression is filled by pushing indices to front. This is the definition of this method.
        // CAUTION : given index must be not rebase-able.
        //
        // Example
        // let's assume we have two tools below.
        // 26 -> 39 = 1 -> 14,
        // 51 -> 65 = 1 -> 15
        //
        // the reference orbit will be shown :
        // 1 2 3 ... 25 26 27 28 ... 36 37 38 39 40 41 42 43 - index
        // 1 2 3 ... 25 40 41 42 ... 50 66 67 68 69 70 71 72 - actual iteration
        //
        // Approach : Subtract the length of tools. It is already compressed iteration, DO NOT consider that includes tools' iteration
        //

        uint64_t compressed = index;

        for (auto &tool: tools) {
            // Since it already processed the cases within the range : startIteration < compressedIteration < endIteration,
            // there is no reason to consider that condition.

            if (index > tool.end) {
                compressed -= tool.range();
            } else {
                break;
            }
        }

        return compressed;
    }

    inline uint64_t ArrayCompressor::containedIndex(const std::vector<ArrayCompressionTool> &tools, const uint64_t index) {
        return binarySearch(tools, index, 0, (tools.size() + 1) / 2, tools.size());
    }

    inline uint64_t ArrayCompressor::compress(const std::vector<ArrayCompressionTool> &tools, const uint64_t index) {
        const uint64_t rebased = rebase(tools, index);
        return pull(tools, rebased);
    }

    inline uint64_t ArrayCompressor::binarySearch(const std::vector<ArrayCompressionTool> &tools, const uint64_t index,
                                                  const uint64_t compIndex, uint64_t const indexGap,
                                                  const uint64_t lastIndexGap) {
        if (compIndex >= tools.size() || tools.front().start > index) {
            return UINT64_MAX;
        }

        const auto &current = tools[compIndex];
        const bool requiredSmallerIndex = current.start > index;
        const bool requiredLargerIndex = current.end < index;

        if (indexGap == lastIndexGap && (requiredLargerIndex || requiredSmallerIndex)) {
            return UINT64_MAX;
        }

        if (requiredSmallerIndex) {
            return binarySearch(tools, index, compIndex - indexGap, (indexGap + 1) / 2, indexGap);
        }
        if (requiredLargerIndex) {
            return binarySearch(tools, index, compIndex + indexGap, (indexGap + 1) / 2, indexGap);
        }

        return compIndex;
    }
}