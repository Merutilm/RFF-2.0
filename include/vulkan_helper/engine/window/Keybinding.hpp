//
// Created by Merutilm on 2026-05-10.
//

#pragma once
#include "KeyInput.hpp"
namespace merutilm::vkh {
    struct Keybinding {
        KeyInput::Key key;
        KeyInput::Modifier mod;
    };
}