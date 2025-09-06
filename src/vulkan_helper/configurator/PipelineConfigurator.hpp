//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "../handle/EngineHandler.hpp"
#include "../impl/Pipeline.hpp"
#include "../impl/PushConstant.hpp"
#include "../util/DescriptorUpdater.hpp"
#include "../struct/DescriptorTemplate.hpp"

namespace merutilm::vkh {
    struct PipelineConfiguratorAbstract {
        EngineRef engine;
        Pipeline pipeline = nullptr;
        std::vector<Descriptor> uniqueDescriptors = {};

        explicit PipelineConfiguratorAbstract(EngineRef engine) : engine(engine) {
        }

        virtual ~PipelineConfiguratorAbstract() = default;

        PipelineConfiguratorAbstract(const PipelineConfiguratorAbstract &) = delete;

        PipelineConfiguratorAbstract(PipelineConfiguratorAbstract &&) = delete;

        PipelineConfiguratorAbstract &operator=(const PipelineConfiguratorAbstract &) = delete;

        PipelineConfiguratorAbstract &operator=(PipelineConfiguratorAbstract &&) = delete;

        template<typename ProgramName, typename... Args> requires std::is_base_of_v<PipelineConfiguratorAbstract,
            ProgramName>
        static ProgramName *createShaderProgram(
            std::vector<std::unique_ptr<PipelineConfiguratorAbstract> > &shaderPrograms,
            EngineRef engine, Args &&... args) {
            auto shaderProgram = std::make_unique<ProgramName>(engine, args...);
            shaderProgram->configure();
            shaderPrograms.emplace_back(std::move(shaderProgram));
            return dynamic_cast<ProgramName *>(shaderPrograms.back().get());
        }


        [[nodiscard]] DescriptorRef getDescriptor(const uint32_t setIndex) const {
            return pipeline->getDescriptor(setIndex);
        }

        [[nodiscard]] PushConstantRef getPushConstant(const uint32_t pushIndex) const {
            return *pipeline->getLayout().getPushConstant(pushIndex);
        }

        template<typename RepoType, typename Return, typename Key>
        [[nodiscard]] Return pickFromRepository(Key &&key) const {
            return engine.getRepositories().getRepository<RepoType>()->pick(std::forward<Key>(key));
        }


        template<typename F> requires std::is_invocable_r_v<void, F, DescriptorUpdateQueue &>
        void writeDescriptor(F &&func) const {
            auto queue = DescriptorUpdater::createQueue();
            func(queue);
            DescriptorUpdater::write(engine.getCore().getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        template<typename F> requires std::is_invocable_r_v<void, F, DescriptorUpdateQueue &, uint32_t>
        void writeDescriptorForEachFrame(F &&func) const {
            auto queue = DescriptorUpdater::createQueue();
            for (uint32_t i = 0; i < engine.getCore().getPhysicalDevice().getMaxFramesInFlight(); ++i) {
                func(queue, i);
            }
            DescriptorUpdater::write(engine.getCore().getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        template<typename F> requires std::is_invocable_r_v<void, F, uint32_t>
        void updateBufferForEachFrame(F &&func) const {
            for (uint32_t i = 0; i < engine.getCore().getPhysicalDevice().getMaxFramesInFlight(); ++i) {
                func(i);
            }
        }

        template<DescTemplateHasID D>
        void appendDescriptor(const uint32_t setExpected, std::vector<DescriptorPtr> &descriptors) {
            const auto context = engine.getRepositories().getDescriptorRequiresRepositoryContext();
            SharedDescriptorRepo &repo = *engine.getRepositories().getRepository<SharedDescriptorRepo>();

            safe_array::check_index_equal(setExpected, static_cast<uint32_t>(descriptors.size()),
                                          "Unique Descriptor Add");
            descriptors.push_back(&repo.pick(DescriptorTemplate::from<D>(), context));
        }

        void appendUniqueDescriptor(const uint32_t setExpected, std::vector<DescriptorPtr> &descriptors,
                                    DescriptorManager &&manager) {
            std::vector<DescriptorManager> managers(1);
            managers[0] = std::move(manager);
            appendUniqueDescriptor(setExpected, descriptors, std::move(managers));
        }

        void appendUniqueDescriptor(const uint32_t setExpected, std::vector<DescriptorPtr> &descriptors,
                                    std::vector<DescriptorManager> &&manager) {
            if (manager.empty()) {
                throw exception_invalid_args("Descriptor manager is empty");
            }
            DescriptorSetLayoutRepo &layoutRepo = *engine.getRepositories().getRepository<DescriptorSetLayoutRepo>();

            safe_array::check_index_equal(setExpected, static_cast<uint32_t>(descriptors.size()),
                                          "Unique Descriptor Add");
            auto desc = factory::create<Descriptor>(engine.getCore(), layoutRepo.pick(manager[0]->layoutBuilder),
                                                    std::move(manager));
            uniqueDescriptors.push_back(std::move(desc));
            descriptors.push_back(uniqueDescriptors.back().get());
        }


        virtual void cmdRender(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) = 0;

        virtual void pipelineInitialized() = 0;

        virtual void windowResized(uint32_t windowAttachmentIndex) = 0;

        virtual void updateQueue(DescriptorUpdateQueue &queue, uint32_t frameIndex) = 0;

    protected:
        virtual void configure() = 0;

        virtual void configurePushConstant(PipelineLayoutManagerRef pipelineLayoutManager) = 0;

        virtual void configureDescriptors(std::vector<DescriptorPtr> &descriptors) = 0;

        void cmdPushAll(const VkCommandBuffer cbh) const {
            pipeline->getLayout().cmdPush(cbh);
        }
    };

    using PipelineConfigurator = std::unique_ptr<PipelineConfiguratorAbstract>;
    using PipelineConfiguratorPtr = PipelineConfiguratorAbstract *;
    using PipelineConfiguratorRef = PipelineConfiguratorAbstract &;
}
