//
// Created by Merutilm on 2026-02-06.
//

#pragma once
#include <string>
#include <vector>

#include "KeyInput.hpp"
#include "Listener.hpp"
#include "PlatformMenuBase.hpp"

namespace merutilm::vkh {

    struct EventSystem final {

        struct MouseEvents {
            Listener<> onMouseEnter;
            Listener<> onMouseExit;
            Listener<KeyInput::MouseButton, int, int> onMouseDown;
            Listener<KeyInput::MouseButton, int, int> onMouseUp;
            Listener<int, int> onMouseMove;
        };

        struct MouseWheelEvents {
            Listener<float> onMouseScroll;
        };

        struct MouseDragEvents {
            Listener<KeyInput::MouseButton, int, int> onMouseDragStart;
            Listener<KeyInput::MouseButton, int, int, int, int> onMouseDrag;
            Listener<KeyInput::MouseButton, int, int> onMouseDragEnd;
        };

        struct KeyboardEvents {
            Listener<KeyInput::Key> onKeyDown;
            Listener<KeyInput::Key> onKeyUp;
        };

        struct FocusEvents {
            Listener<> onFocusGained;
            Listener<> onFocusLost;
        };

        struct ResizeEvents {
            Listener<> onResizeBegin;
            Listener<uint32_t, uint32_t> onResizing;
            Listener<uint32_t, uint32_t> onResizeEnd;
        };

        struct WindowEvents {
            Listener<> onMinimize;
            Listener<> onMaximize;
            Listener<> onRestore;
        };

        struct ApplicationLifecycleEvents {
            Listener<> onStart;
            Listener<> onUpdate;
            Listener<> onQuit;
        };

        struct DnDEvents {
            Listener<const std::vector<std::string> &> onFileDrop;
        };

        struct MenuEvents {
            Listener<PlatformMenuBase *> onMenuOpen;
        };

        MouseEvents mouse;
        MouseWheelEvents mouseWheel;
        MouseDragEvents mouseDrag;
        KeyboardEvents keyboard;
        FocusEvents focus;
        ResizeEvents resize;
        WindowEvents window;
        ApplicationLifecycleEvents applicationLifecycle;
        DnDEvents dnd;


    };
} // namespace merutilm::vkh
