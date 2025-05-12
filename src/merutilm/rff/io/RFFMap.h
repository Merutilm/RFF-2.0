//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <cstdint>

#include "../data/Matrix.h"

struct RFFMap {
    const double zoom;
    const uint64_t period;
    const uint64_t maxIteration;
    const Matrix<double> iterations;


    RFFMap(double zoom, uint64_t period, uint64_t maxIteration, const Matrix<double> &iterations);

    static RFFMap nullRFFMap();
};
