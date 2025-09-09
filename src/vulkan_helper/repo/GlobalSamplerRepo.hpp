//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "../core/vkh_core.hpp"
#include "Repository.hpp"

#include "../impl/Sampler.hpp"
#include "../hash/SamplerCreateInfoEquals.hpp"
#include "../hash/SamplerCreateInfoHasher.hpp"

namespace merutilm::vkh {

    struct GlobalSamplerRepo final : public Repository<VkSamplerCreateInfo, const VkSamplerCreateInfo &, Sampler, SamplerRef, SamplerCreateInfoHasher, SamplerCreateInfoEquals>{

        using Repository::Repository;

        SamplerRef pick(const VkSamplerCreateInfo & samplerCreateInfo) override {
            auto it = repository.find(samplerCreateInfo);
            if (it == repository.end()) {
                auto[newIt, _] = repository.try_emplace(samplerCreateInfo, factory::create<Sampler>(core, samplerCreateInfo));
                it = newIt;
            }
            return *it->second;
        }
    };
}
