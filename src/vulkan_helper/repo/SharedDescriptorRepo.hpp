//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include "DescriptorSetLayoutRepo.hpp"
#include "Repo.hpp"
#include "../def/Descriptor.hpp"
#include "../struct/DescriptorTemplateInfo.hpp"

namespace merutilm::vkh {
    struct SharedDescriptorRepo final : Repo<uint32_t, const DescriptorTemplateInfo &, Descriptor, std::hash<uint32_t>, std::equal_to<>, DescriptorSetLayoutRepo&> {

        using Repo::Repo;

        const Descriptor &pick(const DescriptorTemplateInfo &descTemplateInfo, DescriptorSetLayoutRepo& layoutRepo) override {
            if (repository.contains(descTemplateInfo.id)) {
                return *repository.at(descTemplateInfo.id);
            }

            auto descManager = descTemplateInfo.descriptorGenerator(core);
            auto &layout = layoutRepo.pick(descManager->getLayoutBuilder());
            repository[descTemplateInfo.id] = std::make_unique<Descriptor>(core, layout, std::move(descManager));
            return *repository.at(descTemplateInfo.id);
        }
    };
}
