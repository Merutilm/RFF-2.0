//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <filesystem>
#include "../data/Matrix.h"

namespace merutilm::rff {
    class RFFMap {
        float logZoom;
        uint64_t period;
        uint64_t maxIteration;
        Matrix<double> iterations;

    public:
        static const RFFMap DEFAULT_MAP;

        RFFMap(float logZoom, uint64_t period, uint64_t maxIteration, Matrix<double> iterations);

        bool hasData() const;

        static RFFMap readByID(const std::filesystem::path &open, uint32_t id);

        static RFFMap read(const std::filesystem::path &path);

        void exportAsKeyframe(const std::filesystem::path &dir) const;

        void exportRFM(const std::filesystem::path &path) const;

        float getLogZoom() const;

        uint64_t getPeriod() const;

        uint64_t getMaxIteration() const;

        const Matrix<double> &getMatrix() const;
    };

    inline const RFFMap RFFMap::DEFAULT_MAP = RFFMap(0, 0, 0, Matrix<double>(0, 0));
}