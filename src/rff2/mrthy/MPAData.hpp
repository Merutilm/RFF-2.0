//
// Created by Merutilm on 11/15/26.
//
#pragma once
#include "../data/ApproxTableBuffer.h"
#include "MPAInfo.hpp"
namespace merutilm::rff2 {

    struct MPAData {
        // buffer
        ApproxTableBuffer *buffer = nullptr;

        // settings
        FrtMPASettings mpaSettings;

        // important data to generate
        MPAInfo info;

    };
}