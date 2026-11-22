//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "SRPCCursor.hpp"
#include "SRPCData.hpp"
namespace merutilm::rff2 {

    template<typename Index, typename Element>
    class SRPCDataView : public SRPCCursor<Index> {
    protected:
        const SRPCData<Index, Element> &data;

    public:
        explicit SRPCDataView(const SRPCData<Index, Element> &data) : SRPCCursor<Index>(data.compressor), data(data) {
        }


        [[nodiscard]] bool hasNext() const {
            return SRPCCursor<Index>::originalIndex < data.uncompressedSize - 1;
        }

        // Time complexity O(1)
        [[nodiscard]] const Element &get() const { return data.compressedArray[SRPCCursor<Index>::refIndex]; }

        // Time complexity O(log N)
        [[nodiscard]] const Element &get(Index i) const {
            return data.compressedArray[SRPCCursor<Index>::calcRefIndexFast(i)];
        }
    };
} // namespace merutilm::rff2
