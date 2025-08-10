//
// Created by Merutilm on 2025-07-09.
//

#pragma once

#include <optional>

namespace merutilm::mvk {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }

        [[nodiscard]] bool requiredConcurrent() const { return graphicsFamily != presentFamily; }
    };
}
