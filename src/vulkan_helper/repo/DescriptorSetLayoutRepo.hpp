//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include <memory>
#include <unordered_map>

#include "Repo.hpp"
#include "../def/Factory.hpp"
#include "../impl/DescriptorSetLayout.hpp"

namespace merutilm::vkh {
    struct DescriptorSetLayoutRepo final : Repository<DescriptorSetLayoutBuilder, const DescriptorSetLayoutBuilder &,
                DescriptorSetLayout, DescriptorSetLayoutRef,
                DescriptorSetLayoutBuilderHasher, std::equal_to<> > {
        using Repository::Repository;

        DescriptorSetLayoutRef pick(const DescriptorSetLayoutBuilder &layoutBuilder) override {
            return *repository.try_emplace(layoutBuilder, Factory::create<DescriptorSetLayout>(core, layoutBuilder)).
                    first->second;
        }
    };
}
