//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include "../core/vkh_core.hpp"
#include "Repository.hpp"
#include "../impl/DescriptorSetLayout.hpp"

namespace merutilm::vkh {
    struct GlobalDescriptorSetLayoutRepo final : Repository<DescriptorSetLayoutBuilder, const DescriptorSetLayoutBuilder &,
                DescriptorSetLayout, DescriptorSetLayoutRef,
                DescriptorSetLayoutBuilderHasher, std::equal_to<> > {
        using Repository::Repository;

        DescriptorSetLayoutRef pick(const DescriptorSetLayoutBuilder &layoutBuilder) override {
            auto it = repository.find(layoutBuilder);
            if (it == repository.end()) {
                auto[newIt, _] = repository.try_emplace(layoutBuilder, factory::create<DescriptorSetLayout>(core, layoutBuilder));
                it = newIt;
            }
            return *it->second;
        }
    };
}
