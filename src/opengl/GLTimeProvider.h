//
// Created by Merutilm on 2025-05-09.
//

#pragma once

namespace merutilm::rff2 {
    struct GLTimeProvider {
        virtual ~GLTimeProvider() = default;

        virtual float getTime() = 0;
    };
}
