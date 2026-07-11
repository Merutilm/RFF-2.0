//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "RFFApplication.hpp"

namespace merutilm::rff2 {
    struct CallbackFile {
        static std::function<void()> fnSaveMap(RFFApplication &arm){return []{};}
        static std::function<void()> fnSaveImage(RFFApplication &arm){return []{};}
        static std::function<void()> fnSaveLocation(RFFApplication &arm){return []{};}
        static std::function<void()> fnLoadMap(RFFApplication &arm){return []{};}
        static std::function<void()> fnLoadLocation(RFFApplication &arm){return []{};}
    };
}
