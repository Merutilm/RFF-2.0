//
// Created by Merutilm on 8/27/26.
//

#pragma once
#include <memory>

#include "vulkan_helper/engine/cmd/CommandBuffer.hpp"
#include "vulkan_helper/engine/sync/Fence.hpp"
#include "vulkan_helper/handle/WindowContextHandler.hpp"
namespace merutilm::rff2 {

    struct ComputeShaderRenderManager final : vkh::WindowContextHandler{

        std::unique_ptr<vkh::CommandPool> commandPool;
        std::unique_ptr<vkh::CommandBuffer> commandBuffer;
        std::unique_ptr<vkh::Fence> fence;

        vkh::BufferContext dstBatchBuffer = {};
        vkh::BufferContext dstIterBuffer = {};
        VkExtent2D bufferExtent = {0, 0};

        explicit ComputeShaderRenderManager(vkh::WindowContext & wc) : WindowContextHandler(wc){
            ComputeShaderRenderManager::init();
        }

        ~ComputeShaderRenderManager() override {
            ComputeShaderRenderManager::cleanup();
        }

        ComputeShaderRenderManager(ComputeShaderRenderManager &) = delete;

        ComputeShaderRenderManager &operator=(ComputeShaderRenderManager &) = delete;

        ComputeShaderRenderManager(ComputeShaderRenderManager &&) = delete;

        ComputeShaderRenderManager & operator=(ComputeShaderRenderManager &&) = delete;


        void tryCreateOrResizeTransferDstBuffer(const vkh::BufferContext &batchCtx, const vkh::BufferContext &iterCtx, const VkExtent2D &extent) {

            if (extent.width != bufferExtent.width || extent.height != bufferExtent.height) {
                if (bufferExtent.width > 0 && bufferExtent.height > 0) {
                    vkh::BufferContext::destroyContext(wc.core, dstBatchBuffer);
                    vkh::BufferContext::destroyContext(wc.core, dstIterBuffer);
                }

                bufferExtent = extent;

                dstBatchBuffer = vkh::BufferContext::createContext(
                   wc.core, {.size = batchCtx.bufferSize,
                                       .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                       .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT});
                dstIterBuffer = vkh::BufferContext::createContext(
                   wc.core, {.size = iterCtx.bufferSize,
                                       .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                       .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT});

                vkh::BufferContext::mapMemory(wc.core, dstBatchBuffer);
                vkh::BufferContext::mapMemory(wc.core, dstIterBuffer);

            }
        }



    protected:
        void init() override {
            commandPool = std::make_unique<vkh::CommandPool>(wc.core);
            commandBuffer = std::make_unique<vkh::CommandBuffer>(wc.core, *commandPool);
            fence = std::make_unique<vkh::Fence>(wc.core);
        }

        void cleanup() override {
            if (bufferExtent.width > 0 && bufferExtent.height > 0) {
                vkh::BufferContext::destroyContext(wc.core, dstBatchBuffer);
                vkh::BufferContext::destroyContext(wc.core, dstIterBuffer);
            }
            commandBuffer = nullptr;
            commandPool = nullptr;
            fence = nullptr;
        }
    };
}