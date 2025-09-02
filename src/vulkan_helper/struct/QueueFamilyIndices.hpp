//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include "../core/vkh_base.hpp"

namespace merutilm::vkh {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsAndComputeFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const { return graphicsAndComputeFamily.has_value() && presentFamily.has_value(); }

        [[nodiscard]] bool requiredConcurrent() const { return graphicsAndComputeFamily != presentFamily; }
    };
}
