//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "RFFApplication.hpp"

namespace merutilm::rff2 {
    struct CallbackShader {
        static std::function<void()> fnPalette(RFFApplication &arm){return []{};}
        static std::function<void()> fnStripe(RFFApplication &arm){return []{};}
        static std::function<void()> fnSlope(RFFApplication &arm){return []{};}
        static std::function<void()> fnColor(RFFApplication &arm){return []{};}
        static std::function<void()> fnFog(RFFApplication &arm){return []{};}
        static std::function<void()> fnBloom(RFFApplication &arm){return []{};}
        static std::function<void()> fnLoadKfrPalette(RFFApplication &arm);
    };
} // namespace merutilm::rff2
