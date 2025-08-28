//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "Repo.hpp"
#include "../def/Factory.hpp"
#include "../impl/Sampler.hpp"
#include "../hash/SamplerCreateInfoEquals.hpp"
#include "../hash/SamplerCreateInfoHasher.hpp"

namespace merutilm::vkh {

    struct SamplerRepo final : public Repository<VkSamplerCreateInfo, VkSamplerCreateInfo &&, Sampler, SamplerRef, SamplerCreateInfoHasher, SamplerCreateInfoEquals>{

        using Repository::Repository;

        SamplerRef pick(VkSamplerCreateInfo && samplerCreateInfo) override {
            return *repository.try_emplace(samplerCreateInfo, Factory::create<Sampler>(core, std::move(samplerCreateInfo))).first->second;
        }
    };
}
