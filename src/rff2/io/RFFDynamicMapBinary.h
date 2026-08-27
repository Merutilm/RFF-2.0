//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <filesystem>
#include <vector>

#include "RFFBinary.h"

namespace merutilm::rff2 {
    struct RFFDynamicMapBinary final : RFFBinary{
        uint64_t period;
        uint64_t maxIteration;
        std::vector<double> iterations;
        uint16_t width;
        uint16_t height;
        static const RFFDynamicMapBinary DEFAULT;

        RFFDynamicMapBinary(float logZoom, uint64_t period, uint64_t maxIteration, std::vector<double> iterations,
                            uint16_t width, uint16_t height);
        [[nodiscard]] bool hasData() const override;

        [[nodiscard]] static RFFDynamicMapBinary read(const std::filesystem::path &path);

        [[nodiscard]] static RFFDynamicMapBinary readByID(const std::filesystem::path &dir, uint32_t id);

        void exportAsKeyframe(const std::filesystem::path &dir) const override;

        void exportFile(const std::filesystem::path &path) const override;
    };

}
