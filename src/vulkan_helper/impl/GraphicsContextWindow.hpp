//
// Created by Merutilm on 2025-07-07.
//

#pragma once
#include <functional>
#include <memory>
#include <windows.h>

namespace merutilm::vkh {
    class GraphicsContextWindowImpl final {
        using Listeners = std::unordered_map<UINT, std::function<LRESULT(GraphicsContextWindowImpl &, HWND, WPARAM, LPARAM)> > ;
        HWND window = nullptr;
        const float framerate;
        Listeners listeners = {};
        std::vector<std::function<void()> > renderers = {};

    public:
        explicit GraphicsContextWindowImpl(HWND window, float framerate);

        ~GraphicsContextWindowImpl() = default;

        GraphicsContextWindowImpl(GraphicsContextWindowImpl &) = delete;

        GraphicsContextWindowImpl(GraphicsContextWindowImpl &&) = delete;

        GraphicsContextWindowImpl &operator=(const GraphicsContextWindowImpl &) = delete;

        GraphicsContextWindowImpl &operator=(GraphicsContextWindowImpl &&) = delete;

        HWND getWindowHandle() const { return window; }

        [[nodiscard]] bool isUnrenderable() const {
            RECT rect;
            GetClientRect(window, &rect);
            return !IsWindow(window) || !IsWindowVisible(window) || IsIconic(window) || rect.bottom - rect.top <= 0 ||
                   rect.right - rect.left <= 0;
        }

        template<typename F> requires std::is_invocable_r_v<LRESULT, F, GraphicsContextWindowImpl &,  HWND, WPARAM,LPARAM>
        void setListener(UINT message, F &&func);

        template<typename F> requires std::is_invocable_r_v<void, F>
        void appendRenderer(F &&func);

        void start() const;

        const Listeners &getListeners() const {return listeners;};

    private:
        void clear() {
            listeners.clear();
            renderers.clear();
        }

    };
    using GraphicsContextWindow = std::unique_ptr<GraphicsContextWindowImpl>;
    using GraphicsContextWindowPtr = GraphicsContextWindowImpl *;
    using GraphicsContextWindowRef = GraphicsContextWindowImpl &;

    template<typename F> requires std::is_invocable_r_v<LRESULT, F, GraphicsContextWindowRef, HWND, WPARAM, LPARAM>
    void GraphicsContextWindowImpl::setListener(const UINT message, F &&func) {
        listeners[message] = std::forward<F>(func);
    }

    template<typename F> requires std::is_invocable_r_v<void, F>
    void GraphicsContextWindowImpl::appendRenderer(F &&func) {
        renderers.emplace_back(std::forward<F>(func));
    }


}
