//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "Repo.hpp"
#include "../hash/PipelineLayoutManagerPtrHasher.hpp"
#include "../hash/PointerDerefEquals.hpp"
#include "../manage/PipelineLayoutManager.hpp"
#include "../def/PipelineLayout.hpp"

namespace merutilm::mvk {
    struct PipelineLayoutRepo final : Repo<PipelineLayoutManager *, std::unique_ptr<PipelineLayoutManager> &&, PipelineLayout, PipelineLayoutManagerPtrHasher, PointerDerefEquals>{

        using Repo::Repo;

        const PipelineLayout &pick(std::unique_ptr<PipelineLayoutManager> &&layoutManager) override {
            auto *ptr = layoutManager.get();
            return *repository.try_emplace(ptr, std::make_unique<PipelineLayout>(core, std::move(layoutManager))).first->
                    second;
        }

    };
}
