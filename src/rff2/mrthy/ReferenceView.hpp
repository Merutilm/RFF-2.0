//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../calc/complex.hpp"
#include "../calc/iteration_types.hpp"
#include "SRPCDataView.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    class ReferenceView : public SRPCDataView<itt::ref, complex<Num>> {
    public:
        using SRPCDataView<itt::ref, complex<Num>>::SRPCDataView;
    };

} // namespace merutilm::rff2
