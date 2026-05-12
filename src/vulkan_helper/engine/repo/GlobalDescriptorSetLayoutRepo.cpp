//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/repo/GlobalDescriptorSetLayoutRepo.hpp>
namespace merutilm::vkh {
    DescriptorSetLayout & GlobalDescriptorSetLayoutRepo::pick(const DescriptorSetLayoutBuilder &layoutBuilder) {
        auto it = repository.find(layoutBuilder);
        if (it == repository.end()) {
            auto[newIt, _] = repository.try_emplace(layoutBuilder, std::make_unique<DescriptorSetLayout>(core, layoutBuilder));
            it = newIt;
        }
        return *it->second;
    }
}