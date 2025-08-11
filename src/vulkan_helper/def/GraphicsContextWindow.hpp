//
// Created by Merutilm on 2025-07-07.
//

#pragma once
#include <cstdint>
#include <functional>
#include <windows.h>

namespace merutilm::vkh {
    class GraphicsContextWindow final {
        HWND window = nullptr;
        const float framerate;
        std::function<void(UINT, WPARAM)> headListener = {};
        std::unordered_map<UINT, std::function<LRESULT(const GraphicsContextWindow &, HWND, WPARAM, LPARAM)> > listeners
                = {};
        std::vector<std::function<void()> > renderers = {};

    public:
        explicit GraphicsContextWindow(HWND window, float framerate);

        ~GraphicsContextWindow() = default;

        GraphicsContextWindow(GraphicsContextWindow &) = delete;

        GraphicsContextWindow(GraphicsContextWindow &&) = delete;

        GraphicsContextWindow &operator=(const GraphicsContextWindow &) = delete;

        GraphicsContextWindow &operator=(GraphicsContextWindow &&) = delete;

        HWND getWindowHandle() const { return window; }

        [[nodiscard]] bool isUnrenderable() const {
            RECT rect;
            GetClientRect(window, &rect);
            return !IsWindow(window) || !IsWindowVisible(window) || IsIconic(window) || rect.bottom - rect.top <= 0 ||
                   rect.right - rect.left <= 0;
        }

        template<typename F> requires std::is_invocable_r_v<void, F, UINT, WPARAM>
        void setListener(F &&func);

        template<typename F> requires std::is_invocable_r_v<LRESULT, F, const GraphicsContextWindow &, HWND, WPARAM,
            LPARAM>
        void setListener(UINT message, F &&func);

        template<typename F> requires std::is_invocable_r_v<void, F>
        void appendRenderer(F &&func);

        void start() const;

        static LRESULT CALLBACK GraphicsContextWindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    private:
        void clear() {
            listeners.clear();
            renderers.clear();
        }

        LRESULT runListeners(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
    };

    template<typename F> requires std::is_invocable_r_v<void, F, UINT, WPARAM>
    void GraphicsContextWindow::setListener(F&& func) {
        headListener = std::forward<F>(func);
    }

    template<typename F> requires std::is_invocable_r_v<LRESULT, F, const GraphicsContextWindow &, HWND, WPARAM, LPARAM>
    void GraphicsContextWindow::setListener(const UINT message, F &&func) {
        listeners[message] = std::forward<F>(func);
    }

    template<typename F> requires std::is_invocable_r_v<void, F>
    void GraphicsContextWindow::appendRenderer(F &&func) {
        renderers.emplace_back(std::forward<F>(func));
    }
}
