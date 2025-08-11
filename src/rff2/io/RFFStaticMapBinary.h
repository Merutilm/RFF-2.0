//
// Created by Merutilm on 2025-06-23.
//

#pragma once
#include <filesystem>

#include "RFFBinary.h"
#include "opencv2/core/mat.hpp"

namespace merutilm::rff2 {
    class RFFStaticMapBinary final : public RFFBinary {
        uint32_t width;
        uint32_t height;
    public:

        static const RFFStaticMapBinary DEFAULT;

        explicit RFFStaticMapBinary(float logZoom, uint32_t width, uint32_t height);

        [[nodiscard]] bool hasData() const override;

        [[nodiscard]] static RFFStaticMapBinary read(const std::filesystem::path &path);

        [[nodiscard]] static cv::Mat loadImageByID(const std::filesystem::path &dir, uint32_t id);

        [[nodiscard]] static RFFStaticMapBinary readByID(const std::filesystem::path &dir, uint32_t id);

        [[nodiscard]] uint32_t getWidth() const;

        [[nodiscard]] uint32_t getHeight() const;

        void exportAsKeyframe(const std::filesystem::path &dir) const override;

        void exportFile(const std::filesystem::path &path) const override;

    };


}
