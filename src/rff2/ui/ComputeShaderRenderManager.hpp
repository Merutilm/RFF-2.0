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

        std::mutex mutex;
        std::unique_ptr<vkh::CommandBuffer> commandBuffer;
        std::unique_ptr<vkh::Fence> fence;

        bool dstBatchBufferExists = false;
        vkh::BufferContext dstBatchBuffer = {};

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


        void tryCreateOrResizeBatchTransferDstBuffer(const vkh::BufferContext &batchCtx) {

            if (!dstBatchBufferExists || batchCtx.bufferSize != dstBatchBuffer.bufferSize) {

                if (dstBatchBufferExists) vkh::BufferContext::destroyContext(wc.core, dstBatchBuffer);

                dstBatchBuffer = vkh::BufferContext::createContext(
                   wc.core, {.size = batchCtx.bufferSize,
                                       .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                       .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT});

                vkh::BufferContext::mapMemory(wc.core, dstBatchBuffer);
                dstBatchBufferExists = true;
            }
        }

    protected:
        void init() override {
            commandBuffer = std::make_unique<vkh::CommandBuffer>(wc.core, wc.getCommandPool());
            fence = std::make_unique<vkh::Fence>(wc.core);
        }

        void cleanup() override {
            if (dstBatchBufferExists) vkh::BufferContext::destroyContext(wc.core, dstBatchBuffer);
            commandBuffer = nullptr;
            fence = nullptr;
        }
    };
}