//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include <string>

namespace merutilm::mvk {
    struct WindowCreateInfo {
        std::string windowAttachmentName;
        std::wstring windowTitleName;
        uint32_t width;
        uint32_t height;
        float framerate;
    };
}
