//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include "DescriptorSetLayoutRepo.hpp"
#include "Repository.hpp"
#include "../context/DescriptorRequiresRepoContext.hpp"
#include "../impl/Descriptor.hpp"
#include "../struct/DescriptorTemplateInfo.hpp"

namespace merutilm::vkh {
    struct SharedDescriptorRepo final : Repository<uint32_t, const DescriptorTemplateInfo &, Descriptor, DescriptorRef,
                std::hash<uint32_t>, std::equal_to<>, const DescriptorRequiresRepoContext &> {
        using Repository::Repository;

        DescriptorRef pick(const DescriptorTemplateInfo &descTemplateInfo,
                           const DescriptorRequiresRepoContext &context) override {
            auto it = repository.find(descTemplateInfo.id);
            if (it == repository.end()) {
                auto descManager = descTemplateInfo.descriptorGenerator(core, context);
                auto &layout = context.layoutRepo.pick(descManager[0]->layoutBuilder);

                auto [newIt, _] = repository.try_emplace(descTemplateInfo.id,
                                                     factory::create<Descriptor>(core, layout, std::move(descManager)));
                it = newIt;
            }
            return *it->second;
        }
    };
}
