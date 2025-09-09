//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vector>

#include "Repository.hpp"
#include "GlobalShaderModuleRepo.hpp"

namespace merutilm::vkh {
    class RepositoriesImpl {

        std::vector<Repo> repositories = {};

    public:

        explicit RepositoriesImpl() {
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

        template<typename RepoType> requires std::is_base_of_v<RepoAbstract, RepoType>
        void addRepository(CoreRef core) {
            if (getRepository<RepoType>() != nullptr) {
                throw exception_invalid_args("Repository already exists");
            }
            repositories.push_back(std::make_unique<RepoType>(core));
        }

    };

    using Repositories = std::unique_ptr<RepositoriesImpl>;
    using RepositoriesPtr = RepositoriesImpl *;
    using RepositoriesRef = RepositoriesImpl &;
};
