//
// Created by Merutilm on 11/15/26.
//
#pragma once
#include "../settings/FrtMPASettings.h"
#include "../calc/templates.hpp"
#include "HNREMPAData.hpp"
namespace merutilm::rff2 {


    template<Number Num>
    struct MPAData {
        // settings
        FrtMPASettings mpaSettings{};

        // important data to generate
        std::vector<HNREMPAData<Num>> hnreList;
    };
} // namespace merutilm::rff2
