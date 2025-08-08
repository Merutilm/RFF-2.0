//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <string>

#include "../context/RenderContext.hpp"
#include "../handle/EngineHandler.hpp"
#include "../def/Pipeline.hpp"
#include "../def/ShaderModule.hpp"
#include "../repo/SharedDescriptorRepo.hpp"
#include "../repo/Repositories.hpp"


namespace merutilm::mvk {
    class GeneralPipelineConfigurator : public EngineHandler {
        const uint32_t subpassIndex;
        const ShaderModule &vertexShader;
        const ShaderModule &fragmentShader;
        std::unique_ptr<Pipeline> pipeline = nullptr;
        std::unique_ptr<VertexBuffer> vertexBuffer = {};
        std::unique_ptr<IndexBuffer> indexBuffer = nullptr;
        std::vector<std::unique_ptr<Descriptor> > uniqueDescriptors = {};

    public:
        explicit GeneralPipelineConfigurator(const Engine &engine,
                                             uint32_t subpassIndex,
                                             const std::string &vertName,
                                             const std::string &fragName);

        ~GeneralPipelineConfigurator() override;

        GeneralPipelineConfigurator(const GeneralPipelineConfigurator &) = delete;

        GeneralPipelineConfigurator(GeneralPipelineConfigurator &&) = delete;

        GeneralPipelineConfigurator &operator=(const GeneralPipelineConfigurator &) = delete;

        GeneralPipelineConfigurator &operator=(GeneralPipelineConfigurator &&) = delete;

        template<typename ProgramName> requires std::is_base_of_v<GeneralPipelineConfigurator, ProgramName>
        static std::unique_ptr<ProgramName> createShaderProgram(const Engine &engine,
                                                                const uint32_t subpassIndex,
                                                                const std::string &vertName,
                                                                const std::string &fragName) {
            auto shaderProgram = std::make_unique<ProgramName>(engine, subpassIndex, vertName, fragName);
            shaderProgram->configure();
            return shaderProgram;
        }

        template<typename Expected> requires std::is_base_of_v<RenderContextConfigurator, Expected>
        [[nodiscard]] const RenderContextConfigurator & getRenderContextConfigurator() const {
            auto r = dynamic_cast<const Expected * const>(engine.getRenderContext().configurator.get());
            if (r == nullptr) {
                throw exception_invalid_args("Current RenderContext mismatch");
            }
            return *r;
        }

        [[nodiscard]] Pipeline &getPipeline() const { return *pipeline; }

        [[nodiscard]] VertexBuffer &getVertexBuffer() const { return *vertexBuffer; }

        [[nodiscard]] IndexBuffer &getIndexBuffer() const { return *indexBuffer; }

        static void appendDescriptor(const uint32_t setExpected, std::vector<const Descriptor *> &descriptors, DescriptorSetLayoutRepo &layoutRepo, SharedDescriptorRepo &repo, const DescriptorName descriptorName, const VkShaderStageFlags useStage) {
            IndexChecker::checkIndexEqual(setExpected, descriptors.size(), "Unique Descriptor Add");
            descriptors.push_back(&repo.pick(descriptorName, layoutRepo, useStage));
        }

        void appendUniqueDescriptor(const uint32_t setExpected, std::vector<const Descriptor *> &descriptors, DescriptorSetLayoutRepo &layoutRepo, std::unique_ptr<DescriptorManager> &&manager) {
            IndexChecker::checkIndexEqual(setExpected, descriptors.size(), "Unique Descriptor Add");
            auto desc = std::make_unique<Descriptor>(engine.getCore(), layoutRepo.pick(manager->getLayoutBuilder()), std::move(manager));
            uniqueDescriptors.push_back(std::move(desc));
            descriptors.push_back(uniqueDescriptors.back().get());
        }

        virtual void updateQueue(DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex, uint32_t width, uint32_t height) = 0;

        virtual void render(VkCommandBuffer cbh, uint32_t frameIndex, uint32_t width, uint32_t height) = 0;

    protected:
        void draw(VkCommandBuffer cbh, uint32_t frameIndex, uint32_t indexVarBinding) const;

        virtual void configure();

        virtual void configurePushConstant(DescriptorSetLayoutRepo &layoutRepo,
                                           PipelineLayoutManager &pipelineLayoutManager) = 0;

        virtual void configureDescriptors(std::vector<const Descriptor *> &descriptors,
                                          DescriptorSetLayoutRepo &layoutRepo, SharedDescriptorRepo &descRepo) = 0;

        virtual void configureVertexBuffer(ShaderObjectManager &som) = 0;

        virtual void configureIndexBuffer(ShaderObjectManager &som) = 0;

        [[nodiscard]] const Descriptor &getDescriptor(const uint32_t setIndex) const {return getPipeline().getPipelineManager().getDescriptor(setIndex);}

        [[nodiscard]] PushConstantManager &getPushConstantManager(const uint32_t pushIndex) const {return getPipeline().getPipelineManager().getLayout().getPipelineLayoutManager().getPCM(pushIndex);}

        void pushAll(const VkCommandBuffer cbh) const {
            getPipeline().getPipelineManager().getLayout().push(cbh);
        }

    private:
        void init() override;

        void destroy() override;
    };
}
