//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../calc/complex.hpp"
#include "../calc/iteration_types.hpp"
#include "SRPCData.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    using ReferenceData = SRPCData<itt::ref, complex<Num>>;

} // namespace merutilm::rff2
