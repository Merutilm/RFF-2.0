//
// Created by Merutilm on 2025-06-23.
//

#pragma once
#include <filesystem>

#include "RFFMap.h"
#include "opencv2/core/mat.hpp"

namespace merutilm::rff {
    class RFFStaticMap final : public RFFMap {
        uint32_t width;
        uint32_t height;
    public:

        static const RFFStaticMap DEFAULT_MAP;

        explicit RFFStaticMap(float logZoom, uint32_t width, uint32_t height);

        [[nodiscard]] bool hasData() const override;

        [[nodiscard]] static RFFStaticMap read(const std::filesystem::path &path);

        [[nodiscard]] static cv::Mat loadImageByID(const std::filesystem::path &dir, uint32_t id);

        [[nodiscard]] static RFFStaticMap readByID(const std::filesystem::path &dir, uint32_t id);

        [[nodiscard]] uint32_t getWidth() const;

        [[nodiscard]] uint32_t getHeight() const;

        void exportAsKeyframe(const std::filesystem::path &dir) const override;

        void exportFile(const std::filesystem::path &path) const override;

    };


}
