//
// Created by Merutilm on 11/15/26.
//
#pragma once
#include "../data/ApproxTableBuffer.h"
#include "MPAInfo.hpp"
#include "SRPCUnit.hpp"
namespace merutilm::rff2 {

    struct MPAData {
        // buffer
        ApproxTableBuffer *buffer = nullptr;

        // settings
        FrtMPASettings mpaSettings;

        // important data to generate
        MPAInfo info;

        // pulled mpa : fill only valid elements from the sparse mpa vector
        // pulled mpa compressor : distinct the elements from "pulled mpa"
        PulledMPACompressor compressor;
    };
}