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
    struct PipelineLayoutRepo final : Repo<PipelineLayoutManagerPtr, PipelineLayoutManager &&, PipelineLayout, PipelineLayoutManagerPtrHasher, PointerDerefEquals>{

        using Repo::Repo;

        const PipelineLayout &pick(PipelineLayoutManager &&layoutManager) override {
            const PipelineLayoutManagerPtr ptr = layoutManager.get();
            return *repository.try_emplace(ptr, std::make_unique<PipelineLayout>(core, std::move(layoutManager))).first->
                    second;
        }

    };
}
