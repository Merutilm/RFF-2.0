//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vector>

#include "SharedDescriptorRepo.hpp"
#include "DescriptorSetLayoutRepo.hpp"
#include "PipelineLayoutRepo.hpp"
#include "Repo.hpp"
#include "ShaderModuleRepo.hpp"
#include "../exception/exception.hpp"

namespace merutilm::vkh {
    class Repositories {

        std::vector<std::unique_ptr<IRepo> > repositories = {};

    public:

        explicit Repositories(const Core &engine) {
            addRepository<DescriptorSetLayoutRepo>(engine);
            addRepository<SharedDescriptorRepo>(engine);
            addRepository<PipelineLayoutRepo>(engine);
            addRepository<ShaderModuleRepo>(engine);
        }

        template<typename RepoType> requires std::is_base_of_v<IRepo, RepoType>
        RepoType *getRepository() {
            for (const auto &repository : repositories) {
                auto repo = dynamic_cast<RepoType *>(repository.get());
                if (repo != nullptr) {
                    return repo;
                }
            }
            return nullptr;
        }

        template<typename RepoType> requires std::is_base_of_v<IRepo, RepoType>
        void addRepository(const Core &engine) {
            if (getRepository<RepoType>() != nullptr) {
                throw exception_invalid_args("Repository already exists");
            }
            repositories.push_back(std::make_unique<RepoType>(engine));
        }

    };
};
