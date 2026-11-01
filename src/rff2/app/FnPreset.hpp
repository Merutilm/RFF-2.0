//
// Created by Merutilm on 7/14/26.
//

#pragma once
#include "RFF2.hpp"
#include "imgui.h"
namespace merutilm::rff2 {
    
    class RFF2;
    struct FnPreset {

        constexpr static uint32_t COLS = 2;
        
        static void calculation(RFF2& app);
        static void render(RFF2& app);
        static void resolution(RFF2& app);
        static void shader(RFF2& app);


    };
} // namespace merutilm::rff2
