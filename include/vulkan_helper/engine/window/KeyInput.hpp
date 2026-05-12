//
// Created by Merutilm on 2026-02-08.
//

#pragma once
#include <stdint.h>

namespace merutilm::vkh::KeyInput {
    enum class MouseButton : uint8_t { LEFT, RIGHT, MIDDLE };

    enum class Modifier : uint8_t {
        KEY_LEFT_SHIFT = 1,
        KEY_RIGHT_SHIFT = 2,
        KEY_LEFT_CONTROL = 4,
        KEY_RIGHT_CONTROL = 8,
        KEY_LEFT_ALT = 16,
        KEY_RIGHT_ALT = 32,
        KEY_LEFT_SUPER = 64,
        KEY_RIGHT_SUPER = 128,

    };

    inline Modifier operator|(Modifier a, Modifier b) {
        return static_cast<Modifier>(
            static_cast<uint8_t>(a) |
            static_cast<uint8_t>(b)
        );
    }

    enum class KeyCategory : uint16_t{

        FUNCTION = 0x0000,
        NUM = 0x0100,
        NUMPAD = 0x0200,
        SYMBOL = 0x0300,
        ARROW = 0x0400,
        MODIFIER = 0x0500,
        CONTROL = 0x0600,
        ALPHABET = 0x0700,
        SYSTEM = 0x0800,
    };

    enum class Key : uint16_t{
        KEY_UNKNOWN = 0xffff,

        KEY_F1 = static_cast<uint16_t>(KeyCategory::FUNCTION),
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,
        KEY_F13,
        KEY_F14,
        KEY_F15,
        KEY_F16,
        KEY_F17,
        KEY_F18,
        KEY_F19,
        KEY_F20,
        KEY_F21,
        KEY_F22,
        KEY_F23,
        KEY_F24,


        KEY_NUM0 = static_cast<uint16_t>(KeyCategory::NUM),
        KEY_NUM1,
        KEY_NUM2,
        KEY_NUM3,
        KEY_NUM4,
        KEY_NUM5,
        KEY_NUM6,
        KEY_NUM7,
        KEY_NUM8,
        KEY_NUM9,

        KEY_NUMPAD0 = static_cast<uint16_t>(KeyCategory::NUMPAD),
        KEY_NUMPAD1,
        KEY_NUMPAD2,
        KEY_NUMPAD3,
        KEY_NUMPAD4,
        KEY_NUMPAD5,
        KEY_NUMPAD6,
        KEY_NUMPAD7,
        KEY_NUMPAD8,
        KEY_NUMPAD9,
        KEY_NUMPAD_MULTIPLY,
        KEY_NUMPAD_ADD,
        KEY_NUMPAD_SEPARATOR,
        KEY_NUMPAD_SUBTRACT,
        KEY_NUMPAD_DECIMAL,
        KEY_NUMPAD_DIVIDE,

        KEY_BACKQUOTE = static_cast<uint16_t>(KeyCategory::SYMBOL),
        KEY_MINUS_UNDERLINE,
        KEY_PLUS_EQUAL,
        KEY_LEFTBRACKET,
        KEY_RIGHTBRACKET,
        KEY_BACKSLASH,
        KEY_SEMICOLON_COLON,
        KEY_QUOTE,
        KEY_COMMA,
        KEY_PERIOD,
        KEY_SLASH,

        KEY_UP = static_cast<uint16_t>(KeyCategory::ARROW),
        KEY_DOWN,
        KEY_LEFT,
        KEY_RIGHT,

        KEY_LEFT_SHIFT = static_cast<uint16_t>(KeyCategory::MODIFIER),
        KEY_RIGHT_SHIFT,
        KEY_LEFT_CONTROL,
        KEY_RIGHT_CONTROL,
        KEY_LEFT_ALT,
        KEY_RIGHT_ALT,
        KEY_LEFT_SUPER,
        KEY_RIGHT_SUPER,

        KEY_TAB = static_cast<uint16_t>(KeyCategory::CONTROL),
        KEY_CAPS_LOCK,
        KEY_SPACE,
        KEY_BACKSPACE,
        KEY_ENTER,
        KEY_ESCAPE,
        KEY_INSERT,
        KEY_DELETE,
        KEY_HOME,
        KEY_END,
        KEY_PAGE_UP,
        KEY_PAGE_DOWN,
        KEY_NUM_LOCK,

        KEY_A = static_cast<uint16_t>(KeyCategory::ALPHABET),
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,

        KEY_PRINT_SCREEN = static_cast<uint16_t>(KeyCategory::SYSTEM),
        KEY_SCROLL_LOCK,
        KEY_PAUSE,
    };
} // namespace merutilm::vkh::KeyInput
