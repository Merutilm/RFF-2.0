//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include "../impl/Engine.hpp"

namespace merutilm::vkh {
    struct Executor {
        const Core &core;

        explicit Executor(const Core &core) : core(core) {
        }

        virtual ~Executor() = default;

        Executor(const Executor &) = delete;

        Executor &operator=(const Executor &) = delete;

        Executor(Executor &&) = delete;

        Executor &operator=(Executor &&) = delete;

    private:
        virtual void begin() = 0;

        virtual void end() = 0;
    };
}
