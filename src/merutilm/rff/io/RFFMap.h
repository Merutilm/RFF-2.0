//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <filesystem>
#include "../data/Matrix.h"

struct RFFMap {
    const float logZoom;
    const uint64_t period;
    const uint64_t maxIteration;
    const Matrix<double> iterations;


    RFFMap(float logZoom, uint64_t period, uint64_t maxIteration, Matrix<double> iterations);

    static RFFMap defaultRFFMap();

    bool hasData() const;

    static RFFMap read(const std::filesystem::path &path);

    void exportAsKeyframe(const std::filesystem::path &dir) const;

    void exportRFM(const std::filesystem::path &path) const;
};
