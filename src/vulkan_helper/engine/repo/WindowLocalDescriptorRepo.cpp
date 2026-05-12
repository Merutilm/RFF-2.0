//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/repo/WindowLocalDescriptorRepo.hpp>

namespace merutilm::vkh {
    Descriptor & WindowLocalDescriptorRepo::pick(const DescriptorTemplateInfo &descTemplateInfo,
        GlobalDescriptorSetLayoutRepo &layoutRepo) {
        auto it = repository.find(descTemplateInfo.id);
        if (it == repository.end()) {
            auto descManager = descTemplateInfo.descriptorGenerator(core);
            auto &layout = layoutRepo.pick(descManager[0].layoutBuilder);

            auto [newIt, _] = repository.try_emplace(descTemplateInfo.id, std::make_unique<Descriptor>(core, layout, std::move(descManager)));
            it = newIt;
        }
        return *it->second;
    }
}
