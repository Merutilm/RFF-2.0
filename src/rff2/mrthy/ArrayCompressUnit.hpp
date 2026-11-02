//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <cstdint>


namespace merutilm::rff2 {
    /**
     * <b>Array Compress Unit</b>
     * <br/>
     * A class for compressing when detecting duplicated elements in an array. <br/>
     * It can be used if there are duplicate values in the array. <br/>
     * Given a range, rebase the first element in that range sequentially starting with index @code 1@endcode.
     * <br/><br/>
     * (e.g., if the given ranges are @code 11-15@endcode, and the rebase index is @code 1@endcode, the first value in the range, @code 11@endcode, will be rebased to index @code 1@endcode when compressed. <br/>
     * at the same time, the other elements will be also rebased @code 12 → 2@endcode, @code 13 → 3@endcode, @code 14 → 4@endcode, and @code 15 → 5@endcode.)

     * @param rebase the index to rebase
     * @param start the start index of uncompressed array
     * @param end the end index of uncompressed array
     */
    template <typename NumType>
    struct ArrayCompressUnit {
        const NumType rebase;
        const NumType start;
        const NumType end;

        explicit ArrayCompressUnit(NumType rebase, NumType start, NumType end);

        /**
        * Gets the range of current tool : @code end - start + 1@endcode
        * @return The range of current tool.
        */
        [[nodiscard]] NumType range() const;
    };


    template <typename NumType>
    ArrayCompressUnit<NumType>::ArrayCompressUnit(const NumType rebase, const NumType start,
                                                      const NumType end) : rebase(rebase), start(start), end(end) {
    }

    template <typename NumType>
    NumType ArrayCompressUnit<NumType>::range() const {
        return end - start + 1;
    }

    using ReferenceCompressUnit = ArrayCompressUnit<itt::ref>;
    using MPACompressUnit = ArrayCompressUnit<itt::ref>;

    using ReferenceCompressor = std::vector<ReferenceCompressUnit>;
    using PulledMPACompressor = std::vector<MPACompressUnit>;
}