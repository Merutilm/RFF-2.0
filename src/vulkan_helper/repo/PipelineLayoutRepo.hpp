//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "Repo.hpp"
#include "../hash/PipelineLayoutManagerPtrHasher.hpp"
#include "../hash/PointerDerefEquals.hpp"
#include "../manage/PipelineLayoutManager.hpp"
#include "../impl/PipelineLayout.hpp"

namespace merutilm::vkh {
    struct PipelineLayoutRepo final : Repository<PipelineLayoutManagerPtr, PipelineLayoutManager &&, PipelineLayout,
                PipelineLayoutRef, PipelineLayoutManagerPtrHasher, PointerDerefEquals> {
        using Repository::Repository;

        PipelineLayoutRef pick(PipelineLayoutManager &&layoutManager) override {
            const PipelineLayoutManagerPtr ptr = layoutManager.get();
            return *repository.try_emplace(ptr, Factory::create<PipelineLayout>(core, std::move(layoutManager))).
                    first->
                    second;
        }
    };
}
