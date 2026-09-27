//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <filesystem>
#include <vector>

#include "RFFBinary.hpp"
#include "RFFMapBinary.hpp"

namespace merutilm::rff2 {
    struct RFFDynamicMapBinary final : RFFMapBinary{

        uint64_t period;
        uint64_t maxIteration;
        std::vector<double> iterations;
        uint16_t width;
        uint16_t height;
        static const RFFDynamicMapBinary DEFAULT;

        RFFDynamicMapBinary(double logZoom, uint64_t period, uint64_t maxIteration, std::vector<double> iterations,
                            uint16_t width, uint16_t height);

        [[nodiscard]] static RFFDynamicMapBinary read(std::ifstream &in);

        [[nodiscard]] static RFFDynamicMapBinary readByID(const std::filesystem::path &dir, uint32_t id);

        void exportAsKeyframe(const std::filesystem::path &dir) const;

        void write(std::ofstream &out) const;
    };

}
