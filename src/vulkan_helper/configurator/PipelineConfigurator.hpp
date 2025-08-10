//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include "../def/Pipeline.hpp"
#include "../handle/EngineHandler.hpp"
#include "../struct/DescriptorTemplate.hpp"

namespace merutilm::mvk {
    struct PipelineConfigurator : public EngineHandler {
        const uint32_t subpassIndex;
        const ShaderModule &vertexShader;
        const ShaderModule &fragmentShader;
        std::unique_ptr<Pipeline> pipeline = nullptr;
        std::vector<std::unique_ptr<Descriptor> > uniqueDescriptors = {};

        explicit PipelineConfigurator(const Engine &engine, const uint32_t subpassIndex, const std::string &vertName,
                                      const std::string &fragName) : EngineHandler(engine), subpassIndex(subpassIndex),
                                                                     vertexShader(
                                                                         engine.getRepositories().getRepository<
                                                                             ShaderModuleRepo>()->pick(vertName)),
                                                                     fragmentShader(
                                                                         engine.getRepositories().getRepository<
                                                                             ShaderModuleRepo>()->pick(fragName)) {
        }

        ~PipelineConfigurator() override = default;

        PipelineConfigurator(const PipelineConfigurator &) = delete;

        PipelineConfigurator(PipelineConfigurator &&) = delete;

        PipelineConfigurator &operator=(const PipelineConfigurator &) = delete;

        PipelineConfigurator &operator=(PipelineConfigurator &&) = delete;


        virtual void updateQueue(DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex, uint32_t width,
                                 uint32_t height) = 0;

        virtual void render(VkCommandBuffer cbh, uint32_t frameIndex, uint32_t width, uint32_t height) = 0;

        void draw(const VkCommandBuffer cbh, const uint32_t frameIndex, const uint32_t indexVarBinding) const {
            const VkBuffer vertexBufferHandle = getVertexBuffer().getBufferHandle(frameIndex);
            constexpr VkDeviceSize vertexBufferOffset = 0;
            vkCmdBindVertexBuffers(cbh, 0, 1, &vertexBufferHandle, &vertexBufferOffset);
            getIndexBuffer().bind(cbh, frameIndex, indexVarBinding);
            vkCmdDrawIndexed(cbh, getIndexBuffer().getElementCount(indexVarBinding), 1, 0, 0, 0);
        }

        virtual void configure() = 0;

        virtual void configureVertexBuffer(ShaderObjectManager &som) = 0;

        virtual void configureIndexBuffer(ShaderObjectManager &som) = 0;

        virtual void configurePushConstant(DescriptorSetLayoutRepo &layoutRepo,
                                           PipelineLayoutManager &pipelineLayoutManager) = 0;

        virtual void configureDescriptors(std::vector<const Descriptor *> &descriptors,
                                          DescriptorSetLayoutRepo &layoutRepo, SharedDescriptorRepo &descRepo) = 0;


        [[nodiscard]] const Descriptor &getDescriptor(const uint32_t setIndex) const {
            return pipeline->getPipelineManager().getDescriptor(setIndex);
        }

        [[nodiscard]] PushConstantManager &getPushConstantManager(const uint32_t pushIndex) const {
            return pipeline->getPipelineManager().getLayout().getPipelineLayoutManager().getPCM(pushIndex);
        }


        [[nodiscard]] virtual VertexBuffer &getVertexBuffer() const = 0;

        [[nodiscard]] virtual IndexBuffer &getIndexBuffer() const = 0;


        void pushAll(const VkCommandBuffer cbh) const {
            pipeline->getPipelineManager().getLayout().push(cbh);
        }

        template<typename ProgramName> requires std::is_base_of_v<PipelineConfigurator, ProgramName>
        static std::unique_ptr<ProgramName> createShaderProgram(const Engine &engine,
                                                                const uint32_t subpassIndex) {
            auto shaderProgram = std::make_unique<ProgramName>(engine, subpassIndex);
            shaderProgram->configure();
            return shaderProgram;
        }

        template<typename Expected> requires std::is_base_of_v<RenderContextConfigurator, Expected>
        [[nodiscard]] const RenderContextConfigurator &getRenderContextConfigurator() const {
            auto r = dynamic_cast<const Expected * const>(engine.getRenderContext().configurator.get());
            if (r == nullptr) {
                throw exception_invalid_args("Current RenderContext mismatch");
            }
            return *r;
        }

        template<DescTemplateHasID D>
        static void appendDescriptor(const uint32_t setExpected, std::vector<const Descriptor *> &descriptors,
                                     DescriptorSetLayoutRepo &layoutRepo, SharedDescriptorRepo &repo, const VkShaderStageFlags useStage) {
            IndexChecker::checkIndexEqual(setExpected, static_cast<uint32_t>(descriptors.size()),
                                          "Unique Descriptor Add");
            descriptors.push_back(&repo.pick(DescriptorTemplate::from<D>(), layoutRepo, useStage));
        }

        void appendUniqueDescriptor(const uint32_t setExpected, std::vector<const Descriptor *> &descriptors,
                                    DescriptorSetLayoutRepo &layoutRepo, std::unique_ptr<DescriptorManager> &&manager) {
            IndexChecker::checkIndexEqual(setExpected, static_cast<uint32_t>(descriptors.size()),
                                          "Unique Descriptor Add");
            auto desc = std::make_unique<Descriptor>(engine.getCore(), layoutRepo.pick(manager->getLayoutBuilder()),
                                                     std::move(manager));
            uniqueDescriptors.push_back(std::move(desc));
            descriptors.push_back(uniqueDescriptors.back().get());
        }
    };
}
