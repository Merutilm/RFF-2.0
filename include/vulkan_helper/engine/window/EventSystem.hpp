//
// Created by Merutilm on 2026-02-06.
//

#pragma once
#include <string>
#include <vector>

#include "Listener.hpp"
#include "PlatformMenuBase.hpp"

namespace merutilm::vkh {

    struct EventSystem final {

        struct MouseEvents {
            Listener<> onMouseEnter;
            Listener<> onMouseExit;
            Listener<int, int, int> onMouseDown;
            Listener<int, int, int> onMouseUp;
            Listener<int, int> onMouseMove;
        };

        struct MouseWheelEvents {
            Listener<float> onMouseScroll;
        };

        struct MouseDragEvents {
            Listener<int, int, int> onMouseDragStart;
            Listener<int, int, int, int, int> onMouseDrag;
            Listener<int, int, int> onMouseDragEnd;
        };

        struct KeyboardEvents {
            Listener<int> onKeyDown;
            Listener<int> onKeyUp;
        };

        struct FocusEvents {
            Listener<> onFocusGained;
            Listener<> onFocusLost;
        };

        struct ResizeEvents {
            Listener<int, int> onResize;
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
