//
// Created by Merutilm on 2026-02-08.
//

#pragma once

namespace merutilm::vkh {
    struct MouseState {
        bool isDragging{};
        int lastX{};
        int lastY{};
        int pressedX{};
        int pressedY{};

        explicit MouseState(const int x, const int y) {
            lastX = x;
            lastY = y;
            pressedX = x;
            pressedY = y;
            isDragging = false;
        }
    };
} // namespace merutilm::vkh
