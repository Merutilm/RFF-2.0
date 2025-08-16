//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "Repo.hpp"
#include "../impl/Sampler.hpp"
#include "../hash/SamplerCreateInfoEquals.hpp"
#include "../hash/SamplerCreateInfoHasher.hpp"

namespace merutilm::vkh {

    struct SamplerRepo final : public Repo<VkSamplerCreateInfo, VkSamplerCreateInfo &&, Sampler, SamplerCreateInfoHasher, SamplerCreateInfoEquals>{

        using Repo::Repo;

        const Sampler &pick(VkSamplerCreateInfo && samplerCreateInfo) override {
            return *repository.try_emplace(samplerCreateInfo, std::make_unique<Sampler>(core, std::move(samplerCreateInfo))).first->second;
        }
    };
}
