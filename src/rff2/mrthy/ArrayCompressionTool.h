//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <cstdint>


namespace merutilm::rff2 {
    /**
     * <b>Array Compression Tool</b>
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
    struct ArrayCompressionTool {
        const uint64_t rebase;
        const uint64_t start;
        const uint64_t end;

        explicit ArrayCompressionTool(uint64_t rebase, uint64_t start, uint64_t end);

        /**
        * Gets the range of current tool : @code end - start + 1@endcode
        * @return The range of current tool.
        */
        uint64_t range() const;
    };


    inline ArrayCompressionTool::ArrayCompressionTool(const uint64_t rebase, const uint64_t start,
                                                      const uint64_t end) : rebase(rebase), start(start), end(end) {
    }

    inline uint64_t ArrayCompressionTool::range() const {
        return end - start + 1;
    }
}