//
// Created by Merutilm on 2025-07-22.
//

#pragma once
#include <string>

namespace merutilm::mvk {

    template<typename T>
    struct PushConstantReserve {
        using type = T;
        uint32_t binding;
    };
}
