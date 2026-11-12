//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <vector>

#include "../mrthy/ArrayCompressionUnit.hpp"

/**
 * <b>Array compressor Utilities</b><br/>
 * A class for compressing when detecting duplicated elements in an array. <br/>
 * It can be used if there are duplicate values in the array. <br/>
 * <b> The compression process in this is divided into two steps: </b> <br/>
 * <li> 1. rebases the given index based on its data.</li>
 * <li> 2. pulls the subsequent elements to fill the empty space.</li> <br/>
 *
 * <b>NOTE : It was get 10% ~ 20% performance improvement by moving all the methods to the header file.</b>
 */
namespace merutilm::rff2::ArrayCompressorUtils {

    /**
     * Checks whether the given index is independent. <br/>
     * The time complexity is @code O(log N)@endcode because it uses binary-search.
     *
     * @param units The Compression units
     * @param index To checking index
     * @return @code true@endcode when given index is independent.
     */
    template<typename Index>
    static bool isIndependent(const std::vector<ArrayCompressionUnit<Index>> &units, Index index);

    /**
     * Rebases the given index. <br/>
     * The rebased index is not the compressed index because it is a middle step that has not proceeded the pulling
     * process. <br/> To get the final index, use the pull() method.
     *
     * @param units The Compression units
     * @param index To rebasing index
     * @param stackTrace compressor units' stack trace
     * @return The rebased index.
     */
    template<typename Index>
    static Index rebase(const std::vector<ArrayCompressionUnit<Index>> &units, Index index,
                        std::vector<const ArrayCompressionUnit<Index> *> *stackTrace = nullptr);

    /**
     * Completes the compressed array by pulling subsequent elements into the void in the array made by the rebasing
     * process. <br/> You can get the final index of the compressed array. <br/> <b>This method assumes that the given
     * index has been completed the rebase process and the given list is the same as the list used in the rebasing
     * process.</b>
     *
     * @param units The Compression units
     * @param rebased To pulling rebased index
     * @return The finally compressed index.
     */
    template<typename Index>
    static size_t pull(const std::vector<ArrayCompressionUnit<Index>> &units, Index rebased);

    /**
     * Gets the contained index of the given list. <br/>
     * The time complexity is @code O(log N)@endcode
     *
     * @param units The Compression units
     * @param index To checking index
     * @return The index of the given list. returns @code UINT64_MAX()@endcode if the given index is un-rebase-able.
     */
    template<typename Index>
    static const ArrayCompressionUnit<Index> *find(const std::vector<ArrayCompressionUnit<Index>> &units, Index index);

    /**
     * Gets the compressed index of compressed array.
     *
     * @param units The Compression units
     * @param index the index you want
     * @param stackTrace compressor units' stack trace
     * @return Finally compressed index
     */
    template<typename Index>
    static size_t compress(const std::vector<ArrayCompressionUnit<Index>> &units, Index index, std::vector<const ArrayCompressionUnit<Index> *> *stackTrace = nullptr);

    // DEFINITION OF ArrayCompressorUtils
    // DEFINITION OF ArrayCompressorUtils
    // DEFINITION OF ArrayCompressorUtils

    template<typename Index>
    bool isIndependent(const std::vector<ArrayCompressionUnit<Index>> &units, const Index index) {
        return find(units, index) == nullptr;
    }

    template<typename Index>
    Index rebase(const std::vector<ArrayCompressionUnit<Index>> &units, const Index index,
                 std::vector<const ArrayCompressionUnit<Index> *> *stackTrace) {
        if (units.empty() || index < units.front().start || index > units.back().end) {
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
        Index rebased = index;

        for (size_t i = units.size(); i > 0; --i) {
            if (auto &unit = units[i - 1]; unit.start <= rebased && rebased <= unit.end) {
                if (stackTrace)
                    stackTrace->push_back(&unit);
                rebased -= unit.start - 1;
            } else if (rebased > unit.end) {
                break;
            }
        }
        return rebased;
    }

    template<typename Index>
    size_t pull(const std::vector<ArrayCompressionUnit<Index>> &units, const Index rebased) {
        // "Rebased" iteration is not a target of compressions.
        // The space created by compression is filled by pushing indices to front. This is the definition of this
        // method. CAUTION : given index must be not rebase-able.
        //
        // Example
        // let's assume we have two units below.
        // 26 -> 39 = 1 -> 14,
        // 51 -> 65 = 1 -> 15
        //
        // the reference orbit will be shown :
        // 1 2 3 ... 25 26 27 28 ... 36 37 38 39 40 41 42 43 - index
        // 1 2 3 ... 25 40 41 42 ... 50 66 67 68 69 70 71 72 - actual iteration
        //
        // Approach : Subtract the length of units. It is already compressed iteration, DO NOT consider that includes
        // units' iteration
        //

        Index compressed = rebased;

        for (auto &unit: units) {
            // Since it already processed the cases within the range : startIteration < compressedIteration <
            // endIteration, there is no reason to consider that condition.

            if (rebased > unit.end) {
                compressed -= unit.range();
            } else {
                break;
            }
        }

        return compressed;
    }

    template<typename Index>
    const ArrayCompressionUnit<Index> *find(const std::vector<ArrayCompressionUnit<Index>> &units, const Index index) {
        if (units.empty() || units.front().start > index || units.back().end < index) {
            return nullptr;
        }

        size_t min = 0;
        size_t max = units.size() - 1;
        const ArrayCompressionUnit<Index> *result = nullptr;

        while (min <= max) {
            const size_t mid = (min + max) / 2;
            const auto &current = units[mid];

            if (current.start > index) {
                if (mid == 0)
                    break;
                max = mid - 1;
            } else if (current.end < index) {
                min = mid + 1;
            } else {
                result = &current;
                break;
            }
        }

        return result;
    }

    template<typename Index>
    size_t compress(const std::vector<ArrayCompressionUnit<Index>> &units, const Index index, std::vector<const ArrayCompressionUnit<Index> *> *stackTrace) {
        const Index rebased = rebase(units, index, stackTrace);
        return pull(units, rebased);
    }
} // namespace merutilm::rff2::ArrayCompressorUtils
