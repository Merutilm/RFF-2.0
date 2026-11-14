//
// Created by Merutilm on 11/15/26.
//

#pragma once
#include "../calc/templates.hpp"
#include "SRPCUnit.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    class MPACursor {

        PulledMPACompressor &pulledMPACompressor;

    public:
        explicit MPACursor(const PulledMPACompressor &pulledMPACompressor) : pulledMPACompressor(pulledMPACompressor) {

        }
    };

}