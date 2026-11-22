//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <cstdint>


namespace merutilm::rff2 {
    /**
     * <b>SRPC Unit</b>
     * <br/>
     * A class for compressing when detecting duplicated elements in an array. <br/>
     * It can be used if there are duplicate values in the array. <br/>
     * Given a range, rebase the first element in that range sequentially starting with index @code 1@endcode.
     * <br/><br/>
     * (e.g., if the given ranges are @code 11-15@endcode, and the rebase index is @code 1@endcode, the first value in the range, @code 11@endcode, will be rebased to index @code 1@endcode when compressed. <br/>
     * at the same time, the other elements will be also rebased @code 12 → 2@endcode, @code 13 → 3@endcode, @code 14 → 4@endcode, and @code 15 → 5@endcode.)

     * @param start the start index of uncompressed array
     * @param end the end index of uncompressed array
     * @param recoverIndex recover index after end of unit
     */
    template <typename Index>
    struct SRPCUnit {
        Index start;
        Index end;
        size_t recoverIndex;

        /**
        * Gets the range of current unit : @code end - start + 1@endcode
        * @return The range of current unit
        */
        [[nodiscard]] Index range() const {
            return end - start + 1;
        }
    };

    using ReferenceCompressionUnit = SRPCUnit<itt::ref>;
    using PulledMPACompressionUnit = SRPCUnit<itt::ref>;

    using ReferenceCompressor = std::vector<ReferenceCompressionUnit>;
    using PulledMPACompressor = std::vector<PulledMPACompressionUnit>;
}