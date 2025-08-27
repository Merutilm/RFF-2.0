//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include "DescriptorSetLayoutRepo.hpp"
#include "Repo.hpp"
#include "../context/DescriptorRequiresRepoContext.hpp"
#include "../impl/Descriptor.hpp"
#include "../struct/DescriptorTemplateInfo.hpp"

namespace merutilm::vkh {
    struct SharedDescriptorRepo final : Repository<uint32_t, const DescriptorTemplateInfo &, Descriptor, DescriptorRef,  std::hash<uint32_t>, std::equal_to<>,  const DescriptorRequiresRepoContext &> {

        using Repository::Repository;

        DescriptorRef pick(const DescriptorTemplateInfo &descTemplateInfo, const DescriptorRequiresRepoContext &context) override {
            if (repository.contains(descTemplateInfo.id)) {
                return *repository.at(descTemplateInfo.id);
            }

            auto descManager = descTemplateInfo.descriptorGenerator(core, context);
            auto &layout = context.layoutRepo.pick(descManager->getLayoutBuilder());
            repository[descTemplateInfo.id] = Factory::create<Descriptor>(core, layout, std::move(descManager));
            return *repository.at(descTemplateInfo.id);
        }
    };
}
