//
// Created by Merutilm on 2025-05-07.
//

#pragma once
#include <windows.h>

namespace merutilm::rff {
    struct Win32GLContextLoader {
        static void initGL();

        static void createContext(HDC hdc, HGLRC *target);
    };
}
