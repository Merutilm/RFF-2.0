//
// Created by Merutilm on 2025-07-07.
//
#pragma once
#include <mutex>

namespace merutilm::vkh {

    struct Handler {

        virtual ~Handler() = default;

        virtual void init() = 0;

        virtual void cleanup() = 0;
    };
}
