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

        static void nextPresetExecutor(const char *txt) {
            endPresetExecutor();
            beginPresetExecutor(txt);
        }

        static void beginPresetExecutor(const char *txt) {
            Utilities::imguiBeginSettings(txt);
            ImGui::BeginTable(txt, COLS);
        }
        static void endPresetExecutor() {
            ImGui::EndTable();
            Utilities::imguiEndSettings();
        }
        
        template<typename P> requires std::is_base_of_v<Preset, P>
        static void addPresetExecutor(RFF2 &app, P preset) {
            const std::string name = preset.getName();
            ImGui::TableNextColumn();
            if (ImGui::Button(name.data(), ImVec2(-FLT_MIN, 0))) {
                app.applyPreset(preset);
            }
        }

    };
} // namespace merutilm::rff2
