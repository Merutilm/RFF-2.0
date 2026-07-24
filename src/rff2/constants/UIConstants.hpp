//
// Created by Merutilm on 7/14/26.
//

#pragma once
#include "imgui.h"

namespace merutilm::rff2::Constants::UI {
    constexpr int PALETTE_COLORS_PER_PAGE = 100;
    constexpr float UNLIMITED_MAX_DRAG = 1e12f;
    constexpr float UNLIMITED_DRAG_SPEED = 1e8f;
    constexpr float UNLIMITED_MIN_DRAG_ANIM = 0.1f;
    constexpr float UNLIMITED_MIN_DRAG_SCALAR = 0.001f;
    constexpr auto UNLIMITED_FMT_DRAG = "%.3g";

    constexpr int PALETTE_LIST_HEIGHT = 150;
    constexpr auto SELECTED_COLOR_HIGHLIGHT = IM_COL32(255, 255, 0, 255);
    constexpr float SELECTED_COLOR_THICKNESS = 2.0f;

    constexpr float DRAG_SPEED_SLOPE = 10;
    constexpr float MIN_DRAG_SLOPE = 1e-9f;
    constexpr float MAX_DRAG_SLOPE = 10000;
    constexpr float STATUS_HEIGHT = 24;
    constexpr auto FMT_SLOPE = "%.3g";

} // namespace merutilm::rff2::Constants::UI
