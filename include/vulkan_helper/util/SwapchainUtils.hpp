//
// Created by Merutilm on 2025-07-26.
//

#pragma once
#include <vulkan_helper/engine/context/WindowContext.hpp>


namespace merutilm::vkh {
    struct SwapchainUtils {
        SwapchainUtils() = delete;


        template<typename F>
            requires std::is_invocable_r_v<void, F, uint32_t>
        static void tryRenderFrame(WindowContext &wc, uint32_t *frameIndex, F &&renderer);

        static void changeFrameIndex(Core &core, uint32_t *frameIndex);

        static uint32_t begin(WindowContext &wc, uint32_t frameIndex);

        static void end(WindowContext &wc, uint32_t frameIndex, uint32_t swapchainImageIndex);
    };

    template<typename F>
        requires std::is_invocable_r_v<void, F, unsigned>
    void SwapchainUtils::tryRenderFrame(WindowContext &wc, uint32_t *frameIndex, F &&renderer) {
        if (!wc.getWindow()->canRenderable()) {
            return;
        }
        changeFrameIndex(wc.core, frameIndex);
        auto swapchainImageIndex = begin(wc, *frameIndex);
        renderer(swapchainImageIndex);
        end(wc, *frameIndex, swapchainImageIndex);
    }

} // namespace merutilm::vkh
