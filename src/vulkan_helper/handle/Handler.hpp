//
// Created by Merutilm on 2025-07-07.
//
#pragma once

namespace merutilm::vkh {
    struct Handler {
        virtual ~Handler() = default;

        virtual void init() = 0;

        virtual void destroy() = 0;
    };
}
