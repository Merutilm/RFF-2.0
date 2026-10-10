//
// Created by Merutilm on 8/27/26.
//

#pragma once
#include <memory>

#include "vulkan_helper/engine/cmd/CommandBuffer.hpp"
#include "vulkan_helper/engine/sync/Fence.hpp"
#include "vulkan_helper/handle/WindowContextHandler.hpp"
namespace merutilm::rff2 {

    struct MultithreadedRenderManager final : vkh::WindowContextHandler{

        std::unique_ptr<vkh::CommandPool> commandPool;
        std::unique_ptr<vkh::CommandBuffer> commandBuffer;
        std::unique_ptr<vkh::Fence> fence;


        explicit MultithreadedRenderManager(vkh::WindowContext & wc) : WindowContextHandler(wc){
            MultithreadedRenderManager::init();
        }

        ~MultithreadedRenderManager() override {
            MultithreadedRenderManager::cleanup();
        }

        MultithreadedRenderManager(MultithreadedRenderManager &) = delete;

        MultithreadedRenderManager &operator=(MultithreadedRenderManager &) = delete;

        MultithreadedRenderManager(MultithreadedRenderManager &&) = delete;

        MultithreadedRenderManager & operator=(MultithreadedRenderManager &&) = delete;




    protected:
        void init() override {
            commandPool = std::make_unique<vkh::CommandPool>(wc.core);
            commandBuffer = std::make_unique<vkh::CommandBuffer>(wc.core, *commandPool);
            fence = std::make_unique<vkh::Fence>(wc.core);
        }

        void cleanup() override {
            commandBuffer = nullptr;
            commandPool = nullptr;
            fence = nullptr;
        }
    };
}