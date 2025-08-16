//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vector>
#include "PushConstantManager.hpp"
#include "../def/DescriptorSetLayout.hpp"
#include "../struct/PushConstantReserve.hpp"

namespace merutilm::vkh {
    class PipelineLayoutManager {

        std::vector<const DescriptorSetLayout *> descriptorSetLayouts = {};
        std::vector<std::unique_ptr<PushConstantManager> > pushConstantManagers = {};

    public:
        PipelineLayoutManager() = default;

        ~PipelineLayoutManager() = default;

        PipelineLayoutManager(const PipelineLayoutManager &) = delete;

        PipelineLayoutManager &operator=(const PipelineLayoutManager &) = delete;

        PipelineLayoutManager(PipelineLayoutManager &&) = delete;

        PipelineLayoutManager &operator=(PipelineLayoutManager &&) = delete;

        bool operator==(const PipelineLayoutManager &) const = default;

        void appendDescriptorSetLayout(const DescriptorSetLayout *descriptorSetLayout) {
            descriptorSetLayouts.emplace_back(descriptorSetLayout);
        }

        template<typename... T>
        void appendPushConstantManager(const uint32_t pushIndexExpected, VkShaderStageFlags useStage, PushConstantReserve<T>&&...pushConstantReservesExpected) {


            SafeArrayChecker::checkIndexEqual(pushIndexExpected, pushConstantManagers.size(), "Push Index");
            auto som = std::make_unique<HostBufferObjectManager>();
            (som->reserve<T>(pushConstantReservesExpected.binding), ...);
            auto pcm = std::make_unique<PushConstantManager>(useStage, std::move(som));
            pushConstantManagers.emplace_back(std::move(pcm));
        }

        std::span<const DescriptorSetLayout * const> getDescriptorSetLayouts() const {
            return descriptorSetLayouts;
        }

        std::span<const std::unique_ptr<PushConstantManager>> getPushConstantManagers() const {
            return pushConstantManagers;
        }

        PushConstantManager &getPCM(const uint32_t pushIndex) const {
            return *pushConstantManagers[pushIndex];
        }
    };
}
