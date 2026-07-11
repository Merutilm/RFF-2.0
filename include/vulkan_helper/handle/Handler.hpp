//
// Created by Merutilm on 2025-07-07.
//
#pragma once

namespace merutilm::vkh {

    struct Handler {

        virtual ~Handler() = default;

    protected:
        virtual void init() = 0;

        virtual void cleanup() = 0;
    };
}
