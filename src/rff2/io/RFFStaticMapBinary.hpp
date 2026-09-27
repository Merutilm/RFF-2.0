//
// Created by Merutilm on 2025-06-23.
//

#pragma once
#include <filesystem>

#include "RFFMapBinary.hpp"
#include "opencv2/core/mat.hpp"

namespace merutilm::rff2 {
    struct RFFStaticMapBinary final : RFFMapBinary {

        uint32_t width;
        uint32_t height;

        static const RFFStaticMapBinary DEFAULT;

        explicit RFFStaticMapBinary(double logZoom, uint32_t width, uint32_t height);

        [[nodiscard]] static RFFStaticMapBinary read(std::ifstream &in);

        [[nodiscard]] static cv::Mat loadImageByID(const std::filesystem::path &dir, uint32_t id);

        [[nodiscard]] static RFFStaticMapBinary readByID(const std::filesystem::path &dir, uint32_t id);

        void exportAsKeyframe(const std::filesystem::path &dir) const;

        void write(std::ofstream &out) const;

    };
}
