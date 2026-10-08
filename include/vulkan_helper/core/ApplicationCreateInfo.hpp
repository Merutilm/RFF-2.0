//
// Created by Merutilm on 10/8/26.
//

#pragma once
#include <cstdint>
#include <vector>
namespace merutilm::vkh {
    struct ApplicationCreateInfo {
        uint32_t version;
        std::vector<const char *> instanceExtensions;
        std::vector<const char *> deviceExtensions;
    };
}