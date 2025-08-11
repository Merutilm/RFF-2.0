//
// Created by Merutilm on 2025-05-09.
//

#pragma once

namespace merutilm::rff2 {
    struct GLTimeRenderer {
        virtual ~GLTimeRenderer() = default;
        virtual void setTime(float time) = 0;
    };
}