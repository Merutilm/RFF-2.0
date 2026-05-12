//
// Created by Merutilm on 2026-05-10.
//

#pragma once

namespace merutilm::vkh::NativeMenuID {

    inline constexpr int ID_MENUS = 0x2000;
    inline constexpr int ID_OPTIONS = 0x1000;
    inline constexpr int ID_OPTIONS_RADIO = 0x0100;
    inline constexpr int ID_OPTIONS_CHECKBOX_FLAG = 0x0080;
    inline int count = 0;
    inline int getIndex(const int menuID) { return menuID - ID_MENUS; }

} // namespace merutilm::vkh::NativeMenuID
