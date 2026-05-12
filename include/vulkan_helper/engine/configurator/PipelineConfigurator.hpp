//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include <vulkan_helper/engine/context/WindowContext.hpp>
#include <vulkan_helper/engine/pipeline/Pipeline.hpp>
#include <vulkan_helper/engine/repo/GlobalDescriptorSetLayoutRepo.hpp>
#include <vulkan_helper/engine/repo/WindowLocalDescriptorRepo.hpp>
#include <vulkan_helper/engine/wrapped/DescriptorTemplate.hpp>
#include <vulkan_helper/util/DescriptorUpdater.hpp>

namespace merutilm::vkh {

    struct PipelineConfigurator;

    template<typename Tp, typename... Args>
    concept PipelineCreatable = std::is_base_of_v<PipelineConfigurator, Tp> && std::is_constructible_v<Tp, Engine &, uint32_t, Args...>;


    struct PipelineConfigurator {
        Engine &engine;
        WindowContext &wc;
        std::unique_ptr<Pipeline> pipeline = nullptr;
        std::vector<std::unique_ptr<Descriptor>> uniqueDescriptors = {};

        explicit PipelineConfigurator(Engine &engine, const uint32_t windowContextIndex) :
            engine(engine), wc(engine.getWindowContext(windowContextIndex)) {}

        virtual ~PipelineConfigurator() = default;

        PipelineConfigurator(const PipelineConfigurator &) = delete;

        PipelineConfigurator(PipelineConfigurator &&) = delete;

        PipelineConfigurator &operator=(const PipelineConfigurator &) = delete;

        PipelineConfigurator &operator=(PipelineConfigurator &&) = delete;


        template<typename Tp, typename... Args> requires PipelineCreatable<Tp, Args...>
        static Tp *create(std::vector<std::unique_ptr<PipelineConfigurator>> &currentPipelineConfigurators, Engine &engine,
                          const uint32_t windowContextIndex, Args &&...args) {
            auto pipeline = std::make_unique<Tp>(engine, windowContextIndex, std::forward<Args>(args)...);
            pipeline->configure();
            currentPipelineConfigurators.emplace_back(std::move(pipeline));
            return dynamic_cast<Tp *>(currentPipelineConfigurators.back().get());
        }


        [[nodiscard]] Descriptor &getDescriptor(const uint32_t setIndex) const {
            return pipeline->getDescriptor(setIndex);
        }

        [[nodiscard]] PushConstant &getPushConstant(const uint32_t pushIndex) const {
            return *pipeline->getLayout().getPushConstant(pushIndex);
        }

        template<typename RepoType, typename Return, typename Key>
        [[nodiscard]] Return pickFromGlobalRepository(Key &&key) const {
            return engine.getGlobalRepositories().getRepository<RepoType>()->pick(std::forward<Key>(key));
        }


        template<typename F>
            requires std::is_invocable_r_v<void, F, DescriptorUpdateQueue &>
        void writeDescriptor(F &&func) const {
            auto queue = DescriptorUpdater::createQueue();
            func(queue);
            DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        template<typename F>
            requires std::is_invocable_r_v<void, F, DescriptorUpdateQueue &, uint32_t>
        void writeDescriptorMF(F &&func) const {
            auto queue = DescriptorUpdater::createQueue();
            for (uint32_t i = 0; i < wc.core.getPhysicalDevice().getMaxFramesInFlight(); ++i) {
                func(queue, i);
            }
            DescriptorUpdater::write(wc.core.getLogicalDevice().getLogicalDeviceHandle(), queue);
        }

        template<typename F>
            requires std::is_invocable_r_v<void, F, uint32_t>
        void updateBufferMF(F &&func) const {
            for (uint32_t i = 0; i < wc.core.getPhysicalDevice().getMaxFramesInFlight(); ++i) {
                func(i);
            }
        }

        template<DescTemplateHasID D>
        void appendDescriptor(const uint32_t setExpected, std::vector<Descriptor *> &descriptors) {
            auto &layoutRepo = *engine.getGlobalRepositories().getRepository<GlobalDescriptorSetLayoutRepo>();
            WindowLocalDescriptorRepo &repo =
                    *wc.getWindowLocalRepositories().getRepository<WindowLocalDescriptorRepo>();

            safe_array::check_index_equal(setExpected, static_cast<uint32_t>(descriptors.size()),
                                          "Unique Descriptor Add");
            descriptors.push_back(&repo.pick(DescriptorTemplate::from<D>(), layoutRepo));
        }

        void appendUniqueDescriptor(const uint32_t setExpected, std::vector<Descriptor *> &descriptors,
                                    DescriptorManager &&manager) {
            std::vector<DescriptorManager> managers;
            managers.reserve(1);
            managers.emplace_back(std::move(manager));
            appendUniqueDescriptor(setExpected, descriptors, std::move(managers));
        }

        void appendUniqueDescriptor(const uint32_t setExpected, std::vector<Descriptor *> &descriptors,
                                    std::vector<DescriptorManager> &&manager) {
            if (manager.empty()) {
                throw exception_invalid_args("Descriptor manager is empty");
            }

            safe_array::check_index_equal(setExpected, static_cast<uint32_t>(descriptors.size()),
                                          "Unique Descriptor Add");
            auto &layoutRepo = *engine.getGlobalRepositories().getRepository<GlobalDescriptorSetLayoutRepo>();
            auto desc = std::make_unique<Descriptor>(wc.core, layoutRepo.pick(manager[0].layoutBuilder), std::move(manager));
            uniqueDescriptors.push_back(std::move(desc));
            descriptors.push_back(uniqueDescriptors.back().get());
        }

        virtual void updateQueue(DescriptorUpdateQueue &queue, uint32_t frameIndex) = 0;

        virtual void cmdRender(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) = 0;

        virtual void pipelineInitialized() = 0;

        virtual void renderContextRefreshed() = 0;

    protected:
        virtual void configure() = 0;

        virtual void configurePushConstant(PipelineLayoutManager &pipelineLayoutManager) = 0;

        virtual void configureDescriptors(std::vector<Descriptor *> &descriptors) = 0;

        void cmdPushAll(const VkCommandBuffer cbh) const { pipeline->getLayout().cmdPush(cbh); }
    };


} // namespace merutilm::vkh
