//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <filesystem>
#include <vector>

#include "../calc/iteration_types.hpp"
#include "RFFBinary.hpp"
#include "RFFMapBinary.hpp"

namespace merutilm::rff2 {
    struct RFFDynamicMapBinary final : RFFMapBinary{

        itt::ptb period;
        itt::ptb maxIteration;
        std::vector<double> iterations;
        uint16_t width;
        uint16_t height;
        static const RFFDynamicMapBinary DEFAULT;

        RFFDynamicMapBinary(double logZoom, itt::ptb period, itt::ptb maxIteration, std::vector<double> iterations,
                            uint16_t width, uint16_t height);

        [[nodiscard]] static RFFDynamicMapBinary read(std::ifstream &in);

        [[nodiscard]] static RFFDynamicMapBinary readByID(const std::filesystem::path &dir, uint32_t id);

        void exportAsKeyframe(const std::filesystem::path &dir) const;

        void write(std::ofstream &out) const;
    };

}
