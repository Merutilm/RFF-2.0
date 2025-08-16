//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include <memory>
#include <unordered_map>

#include "Repo.hpp"
#include "../impl/DescriptorSetLayout.hpp"

namespace merutilm::vkh {
    struct DescriptorSetLayoutRepo final : Repo<DescriptorSetLayoutBuilder, const DescriptorSetLayoutBuilder &, DescriptorSetLayout,
                DescriptorSetLayoutBuilderHasher, std::equal_to<>> {
        using Repo::Repo;

        const DescriptorSetLayout &pick(const DescriptorSetLayoutBuilder &layoutBuilder) override {
            return *repository.try_emplace(layoutBuilder, std::make_unique<DescriptorSetLayout>(core, layoutBuilder)).
                    first->second;
        }
    };
}
