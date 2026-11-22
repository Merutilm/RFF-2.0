//
// Created by Merutilm on 11/15/26.
//
#pragma once
#include "../algorithm/MPAInfo.hpp"
#include "../calc/templates.hpp"
#include "ApproxTableBuffer.h"
namespace merutilm::rff2 {

    template<Number Num>
    class MPAView;

    template<Number Num>
    struct MPAData {
        // buffer
        ApproxTableBuffer *buffer = nullptr;

        // settings
        FrtMPASettings mpaSettings{};

        // important data to generate
        MPAInfo info;

        [[nodiscard]] MPAView<Num> makeMPAView() const;

    };
}