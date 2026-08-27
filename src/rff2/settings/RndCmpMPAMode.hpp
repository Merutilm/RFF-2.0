//
// Created by Merutilm on 8/26/26.
//

#pragma once
#include <cstdint>

namespace merutilm::rff2 {

    enum class RndCmpMPAMode : uint32_t{
        OFF,
        FIRST_REFITERATION_ONLY,
        FULL
    };
}