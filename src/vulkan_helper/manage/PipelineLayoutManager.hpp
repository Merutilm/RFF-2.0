//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vector>
#include "PushConstantManager.hpp"
#include "../def/Factory.hpp"
#include "../impl/DescriptorSetLayout.hpp"
#include "../struct/PushConstantReserve.hpp"

namespace merutilm::vkh {
    class PipelineLayoutManagerImpl {

        std::vector<const DescriptorSetLayout *> descriptorSetLayouts = {};
        std::vector<PushConstantManager > pushConstantManagers = {};

    public:
        PipelineLayoutManagerImpl() = default;

        ~PipelineLayoutManagerImpl() = default;

        PipelineLayoutManagerImpl(const PipelineLayoutManagerImpl &) = delete;

        PipelineLayoutManagerImpl &operator=(const PipelineLayoutManagerImpl &) = delete;

        PipelineLayoutManagerImpl(PipelineLayoutManagerImpl &&) = delete;

        PipelineLayoutManagerImpl &operator=(PipelineLayoutManagerImpl &&) = delete;

        bool operator==(const PipelineLayoutManagerImpl &) const = default;

        void appendDescriptorSetLayout(const DescriptorSetLayout *descriptorSetLayout) {
            descriptorSetLayouts.emplace_back(descriptorSetLayout);
        }

        template<typename... T>
        void appendPushConstantManager(const uint32_t pushIndexExpected, VkShaderStageFlags useStage, PushConstantReserve<T>&&...pushConstantReservesExpected) {


            SafeArrayChecker::checkIndexEqual(pushIndexExpected, pushConstantManagers.size(), "Push Index");
            auto som = Factory::create<HostBufferObjectManager>();
            (som->reserve<T>(pushConstantReservesExpected.binding), ...);
            auto pcm = Factory::create<PushConstantManager>(useStage, std::move(som));
            pushConstantManagers.emplace_back(std::move(pcm));
        }

        std::span<const DescriptorSetLayout * const> getDescriptorSetLayouts() const {
            return descriptorSetLayouts;
        }

        std::span<const PushConstantManager> getPushConstantManagers() const {
            return pushConstantManagers;
        }

        const PushConstantManager &getPCM(const uint32_t pushIndex) const {
            return pushConstantManagers[pushIndex];
        }
    };

    using PipelineLayoutManager = std::unique_ptr<PipelineLayoutManagerImpl>;
    using PipelineLayoutManagerPtr = PipelineLayoutManagerImpl *;
    using PipelineLayoutManagerRef = PipelineLayoutManagerImpl &;

}
