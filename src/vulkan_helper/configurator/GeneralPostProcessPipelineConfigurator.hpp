//
// Created by Merutilm on 2025-08-05.
//

#pragma once
#include "GeneralPipelineConfigurator.hpp"
#include "PipelineConfigurator.hpp"

namespace merutilm::vkh {
    class GeneralPostProcessPipelineConfigurator : public PipelineConfigurator {
        inline static bool initializedVertexIndex = false;
        inline static std::unique_ptr<VertexBuffer> vertexBufferPP = nullptr;
        inline static std::unique_ptr<IndexBuffer> indexBufferPP = nullptr;
        static constexpr auto VERTEX_MODULE_PATH = "vk_post_process.vert";

    public:

        explicit GeneralPostProcessPipelineConfigurator(const Engine &engine,
                                                        const uint32_t subpassIndex,
                                                        const std::string &fragName) : PipelineConfigurator(
            engine, subpassIndex, VERTEX_MODULE_PATH, fragName) {
        }

        ~GeneralPostProcessPipelineConfigurator() override = default;

        GeneralPostProcessPipelineConfigurator(const GeneralPostProcessPipelineConfigurator &) = delete;

        GeneralPostProcessPipelineConfigurator &operator=(const GeneralPostProcessPipelineConfigurator &) = delete;

        GeneralPostProcessPipelineConfigurator(GeneralPostProcessPipelineConfigurator &&) = delete;

        GeneralPostProcessPipelineConfigurator &operator=(GeneralPostProcessPipelineConfigurator &&) = delete;

        void configure() override;

        void render(VkCommandBuffer cbh, uint32_t frameIndex, uint32_t width, uint32_t height) override;

        static void cleanup() {
            vertexBufferPP = nullptr;
            indexBufferPP = nullptr;
            initializedVertexIndex = false;
        }

    protected:
        [[nodiscard]] VertexBuffer &getVertexBuffer() const override { return *vertexBufferPP; }

        [[nodiscard]] IndexBuffer &getIndexBuffer() const override { return *indexBufferPP; }

        void configureVertexBuffer(BufferObjectManager &som) override;

        void configureIndexBuffer(BufferObjectManager &som) override;

    private:
        void init() override;

        void destroy() override;
    };
}
