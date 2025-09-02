//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vector>

#include "SharedDescriptorRepo.hpp"
#include "DescriptorSetLayoutRepo.hpp"
#include "PipelineLayoutRepo.hpp"
#include "Repository.hpp"
#include "SamplerRepo.hpp"
#include "ShaderModuleRepo.hpp"

namespace merutilm::vkh {
    class RepositoriesImpl {

        std::vector<Repo> repositories = {};

    public:

        explicit RepositoriesImpl(CoreRef core) {
            addRepository<DescriptorSetLayoutRepo>(core);
            addRepository<SharedDescriptorRepo>(core);
            addRepository<PipelineLayoutRepo>(core);
            addRepository<ShaderModuleRepo>(core);
            addRepository<SamplerRepo>(core);
        }

        template<typename RepoType> requires std::is_base_of_v<RepoAbstract, RepoType>
        RepoType *getRepository() {
            for (const auto &repository : repositories) {
                auto repo = dynamic_cast<RepoType *>(repository.get());
                if (repo != nullptr) {
                    return repo;
                }
            }
            return nullptr;
        }

        DescriptorRequiresRepoContext getDescriptorRequiresRepositoryContext() {
            return {
                .layoutRepo = *getRepository<DescriptorSetLayoutRepo>(),
                .samplerRepo = *getRepository<SamplerRepo>(),
            };
        }

        template<typename RepoType> requires std::is_base_of_v<RepoAbstract, RepoType>
        void addRepository(CoreRef engine) {
            if (getRepository<RepoType>() != nullptr) {
                throw exception_invalid_args("Repository already exists");
            }
            repositories.push_back(std::make_unique<RepoType>(engine));
        }

    };

    using Repositories = std::unique_ptr<RepositoriesImpl>;
    using RepositoriesPtr = RepositoriesImpl *;
    using RepositoriesRef = RepositoriesImpl &;
};
