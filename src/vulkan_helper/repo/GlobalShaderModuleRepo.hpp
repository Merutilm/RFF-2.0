//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include "../core/vkh_base.hpp"
#include "../impl/ShaderModule.hpp"
#include "../struct/StringHasher.hpp"

namespace merutilm::vkh {
    struct GlobalShaderModuleRepo final : Repository<std::string, const std::string &, ShaderModule, ShaderModuleRef,
                StringHasher, std::equal_to<> > {
        using Repository::Repository;


        ShaderModuleRef pick(const std::string &filename) override {
            auto it = repository.find(filename);
            if (it == repository.end()) {
                auto [newIt, _] = repository.try_emplace(filename, factory::create<ShaderModule>(core, filename));
                it = newIt;
            }
            return *it->second;
        }
    };
}
