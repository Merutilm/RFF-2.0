//
// Created by Merutilm on 2025-08-29.
//

#pragma once
namespace merutilm::vkh {
    enum RenderPassProcessTypeFlagBits {
        FIRST = 0x01,
        MIDDLE = 0x02,
        LAST = 0x04
    };

    using RenderPassProcessTypeFlags = uint32_t;
}
