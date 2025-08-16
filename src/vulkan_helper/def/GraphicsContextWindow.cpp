//
// Created by Merutilm on 2025-07-07.
//

#include "GraphicsContextWindow.hpp"
#include <chrono>
#include <windows.h>

namespace merutilm::vkh {
    GraphicsContextWindow::GraphicsContextWindow(const HWND window, const float framerate) : window(window),
        framerate(framerate) {
    }


    void GraphicsContextWindow::start() const {
        MSG message;
        using namespace std::chrono;
        auto started = high_resolution_clock::now();

        while (true) {
            if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);

                if (message.message == WM_QUIT) {
                    break;
                }
            }

            auto now = high_resolution_clock::now();

            if (auto elapsed = duration_cast<milliseconds>(now - started);
                static_cast<float>(elapsed.count()) > 1000 / framerate) {
                started = now;
                for (const auto &renderer: renderers) {
                    renderer();
                }
            }
        }
    }


    LRESULT GraphicsContextWindow::GraphicsContextWindowProc(const HWND hwnd, const UINT message, const WPARAM wparam,
                                                             const LPARAM lparam) {
        const auto windowPtr = reinterpret_cast<GraphicsContextWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        if (windowPtr == nullptr) {
            return DefWindowProcW(hwnd, message, wparam, lparam);
        }

        auto &window = *windowPtr;

        if (message == WM_DESTROY) {
            PostQuitMessage(0);
        }

        return window.runListeners(hwnd, message, wparam, lparam);
    }

    LRESULT GraphicsContextWindow::runListeners(const HWND hwnd, const UINT message, const WPARAM wparam,
                                                const LPARAM lparam) {
        if (listeners.contains(message)) {
            return listeners[message](*this, hwnd, wparam, lparam);
        }
        return DefWindowProc(hwnd, message, wparam, lparam);
    }
}
