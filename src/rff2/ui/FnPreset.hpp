//
// Created by Merutilm on 7/14/26.
//

#pragma once
#include "RFFApplication.hpp"
#include "imgui.h"
namespace merutilm::rff2 {
    
    class RFFApplication;
    struct FnPreset {
        
        static void calculation(RFFApplication& app);
        static void render(RFFApplication& app);
        static void resolution(RFFApplication& app);
        static void shader(RFFApplication& app);
        
        template<typename P> requires std::is_base_of_v<Preset, P>
        static void addPresetExecutor(RFFApplication&app, P preset) {
            const std::string name = preset.getName();
            if (ImGui::Button(name.data(), ImVec2(-FLT_MIN, 0))) {
                app.applyPreset(preset);
            }
        }

    };
} // namespace merutilm::rff2
