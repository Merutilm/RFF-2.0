//
// Created by Merutilm on 2026-02-08.
//

#pragma once

namespace merutilm::vkh {
    struct MouseState {
        bool isPressed;
        bool isDragging;
        int pressedX;
        int pressedY;

        MouseState() { reset(); }

        void reset() {
            isPressed = false;
            isDragging = false;
            pressedX = 0;
            pressedY = 0;
        }
    };
} // namespace merutilm::vkh
