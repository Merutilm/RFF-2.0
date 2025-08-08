//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include "DescriptorSetLayoutRepo.hpp"
#include "Repo.hpp"
#include "../def/Descriptor.hpp"
#include "../struct/DescriptorName.hpp"
#include "../struct/EnumHasher.hpp"
#include "../template/DSSharedTemplate.hpp"

namespace merutilm::mvk {
    struct SharedDescriptorRepo final : Repo<DescriptorName, DescriptorName, Descriptor, EnumHasher, std::equal_to<>, DescriptorSetLayoutRepo&, VkShaderStageFlags> {

        using Repo::Repo;

        const Descriptor &pick(const DescriptorName descName, DescriptorSetLayoutRepo& layoutRepo, const VkShaderStageFlags useStage) override {
            auto descManager = findDescriptorManager(descName, useStage);
            auto &layout = layoutRepo.pick(descManager->getLayoutBuilder());
            return *repository.try_emplace(
                descName, std::make_unique<Descriptor>(core, layout, std::move(descManager))).first->second;
        }

        std::unique_ptr<DescriptorManager> findDescriptorManager(const DescriptorName descName, const VkShaderStageFlags useStage) const {
            //  <factory method>

            switch (descName) {
                    using enum DescriptorName;
                case CAMERA_3D: {
                    return DSSharedTemplate::Camera3D::create(core, useStage);
                }
                case TIME: {
                    return DSSharedTemplate::Time::create(core, useStage);
                }
                default: {
                    //no operation
                }
            }
            return nullptr;
        }

    };
}
