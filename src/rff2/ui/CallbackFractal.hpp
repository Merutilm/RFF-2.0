//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "RFFApplication.hpp"
namespace merutilm::rff2 {
    struct CallbackFractal {
        static std::function<void()> fnReference(RFFApplication &arm){return []{};}
        static std::function<void()> fnIterations(RFFApplication &arm){return []{};}
        static std::function<void()> fnSa(RFFApplication &arm){return []{};}
        static std::function<void()> fnMpa(RFFApplication &arm){return []{};}

        static std::function<bool()> fnGetterAutomaticIterations(RFFApplication &arm);
        static std::function<bool()> fnGetterAbsoluteIterationMode(RFFApplication &arm);

        static std::function<void(bool)> fnAutomaticIterations(RFFApplication &arm);
        static std::function<void(bool)> fnAbsoluteIterationMode(RFFApplication &arm);
    };
} // namespace merutilm::rff2
