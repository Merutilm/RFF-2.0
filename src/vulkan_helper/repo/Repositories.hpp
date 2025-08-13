//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vector>

#include "SharedDescriptorRepo.hpp"
#include "DescriptorSetLayoutRepo.hpp"
#include "PipelineLayoutRepo.hpp"
#include "Repo.hpp"
#include "SamplerRepo.hpp"
#include "ShaderModuleRepo.hpp"
#include "../exception/exception.hpp"

namespace merutilm::vkh {
    class Repositories {

        std::vector<std::unique_ptr<IRepo> > repositories = {};

    public:

        explicit Repositories(const Core &core) {
            addRepository<DescriptorSetLayoutRepo>(core);
            addRepository<SharedDescriptorRepo>(core);
            addRepository<PipelineLayoutRepo>(core);
            addRepository<ShaderModuleRepo>(core);
            addRepository<SamplerRepo>(core);
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

        DescriptorRequiresRepoContext getDescriptorRequiresRepositoryContext() {
            return {
                .layoutRepo = *getRepository<DescriptorSetLayoutRepo>(),
                .samplerRepo = *getRepository<SamplerRepo>(),
            };
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
