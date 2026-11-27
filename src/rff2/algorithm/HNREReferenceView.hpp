//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../calc/complex.hpp"
#include "../calc/iteration_types.hpp"
#include "../data/HNREReferenceData.hpp"
#include "MPAUtils.hpp"
#include "srpc_module/SRPCDataView.hpp"

namespace merutilm::rff2 {

    template<Number Num>
    class HNREReferenceView : public SRPCDataView<itt::ref, complex<Num>> {
    public:
        using SRPCDataView<itt::ref, complex<Num>>::SRPCDataView;


        void endpoint() {
            using View = SRPCDataView<itt::ref, complex<Num>>;
            using Cursor = SRPCCursor<itt::ref>;

            Cursor::originalIndex = View::data.uncompressedSize - MPAUtils::PERTURBATION_REQ;
            Cursor::refIndex = View::data.compressedArray.size() - MPAUtils::PERTURBATION_REQ;
            Cursor::currentIndexSegmentStack.clear();
            Cursor::startedUnitCount = View::data.compressor.size();
            if (!Cursor::compressor.empty()) {
                const auto &back = Cursor::compressor.back();
                if (back.end >= Cursor::originalIndex) {
                    // This operation is performed only when the compressor has been deleted from the end of the
                    // reference array; compressors processed after rebasing are part of the entire array, so they are
                    // inexpensive.
                    View::setIndex(Cursor::originalIndex);
                }
            }
        }
    };

    template<Number Num>
    HNREReferenceView<Num> HNREReferenceData<Num>::makeReferenceView() const {
        return HNREReferenceView<Num>(static_cast<const SRPCData<itt::ref, complex<Num>> &>(*this));
    }

} // namespace merutilm::rff2
