//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/repo/GlobalPipelineLayoutRepo.hpp>

namespace merutilm::vkh {
    PipelineLayout & GlobalPipelineLayoutRepo::pick(PipelineLayoutManager &&layoutManager) {
        const PipelineLayoutBuilder builder = layoutManager.builders; //clone the builder
        auto it = repository.find(builder);
        if (it == repository.end()) {
            auto [newIt, _] = repository.try_emplace(
                builder, std::make_unique<PipelineLayout>(core, std::move(layoutManager)));
            it = newIt;
        }

        return *it->second;
    }
}
