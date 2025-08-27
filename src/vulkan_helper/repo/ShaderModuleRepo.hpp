//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "../impl/ShaderModule.hpp"
#include "../struct/StringHasher.hpp"

namespace merutilm::vkh {
    struct ShaderModuleRepo final : Repository<std::string, const std::string&, ShaderModule, ShaderModuleRef, StringHasher, std::equal_to<>>{

        using Repository::Repository;


        ShaderModuleRef pick(const std::string &filename) override {
            return *repository.try_emplace(filename, Factory::create<ShaderModule>(core, filename)).first->second;
        }

    };
}
