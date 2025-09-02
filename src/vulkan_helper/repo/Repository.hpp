//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "RepoAbstract.hpp"
#include "../impl/Core.hpp"

namespace merutilm::vkh {

    template<typename Key, typename KeyInput, typename Type, typename Return, typename KeyHasher, typename KeyPredicate, typename... Args>
    struct Repository : RepoAbstract{

        CoreRef core;

        std::unordered_map<Key, Type, KeyHasher, KeyPredicate> repository = {};

        explicit Repository(CoreRef core) : core(core) {}

        void remove(Key key) {
            repository.erase(key);
        }

        virtual Return pick(KeyInput keyInput, Args... args) = 0;

    };

}
