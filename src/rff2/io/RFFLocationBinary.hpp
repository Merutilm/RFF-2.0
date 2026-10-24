//
// Created by Merutilm on 2025-06-25.
//

#pragma once

#include <string>

#include "../calc/iteration_types.hpp"
#include "RFFBinary.hpp"

namespace merutilm::rff2 {

    struct RFFLocationBinary final : RFFBinary{
        double logZoom;
        std::string real;
        std::string imag;
        itt::ptb maxIteration;

        static const RFFLocationBinary DEFAULT;

        explicit RFFLocationBinary(double logZoom, std::string real, std::string imag, itt::ptb maxIteration);

        [[nodiscard]] static RFFLocationBinary read(std::ifstream &in);

        void exportAsKeyframe(const std::filesystem::path &dir) const;

        void write(std::ofstream &out) const;
    };
}
