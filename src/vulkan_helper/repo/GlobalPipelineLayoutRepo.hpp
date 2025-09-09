//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "Repository.hpp"
#include "../hash/PipelineLayoutBuilderHasher.hpp"
#include "../manage/PipelineLayoutManager.hpp"
#include "../impl/PipelineLayout.hpp"

namespace merutilm::vkh {
    struct GlobalPipelineLayoutRepo final : Repository<PipelineLayoutBuilder, PipelineLayoutManager &&, PipelineLayout,
                PipelineLayoutRef, PipelineLayoutBuilderHasher, std::equal_to<>> {
        using Repository::Repository;

        PipelineLayoutRef pick(PipelineLayoutManager &&layoutManager) override {
            const PipelineLayoutBuilder builder = layoutManager->builders; //clone the builder
            auto it = repository.find(builder);
            if (it == repository.end()) {
                auto [newIt, _] = repository.try_emplace(
                    builder, factory::create<PipelineLayout>(core, std::move(layoutManager)));
                it = newIt;
            }

            return *it->second;
        }
    };
}
