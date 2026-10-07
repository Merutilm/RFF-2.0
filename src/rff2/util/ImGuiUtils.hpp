//
// Created by Merutilm on 10/7/26.
//

#pragma once

#include "../settings/Selectable.h"
#include "imgui.h"

// Use PascalCase for method name in accordance with the ImGui naming convention
namespace merutilm::rff2::ImGuiUtils {

    template<typename Enum>
        requires std::is_enum_v<Enum>
    static bool Dropdown(const char *label, Enum *currentValue) {
        static const std::vector<Enum> values = Selectable::values<Enum>();
        static std::vector<const char *> valueStr;
        valueStr.reserve(values.size());

        const bool newlyAdded = valueStr.empty();

        int valueIndex = 0;
        for (int i = 0; i < values.size(); ++i) {
            if (newlyAdded)
                valueStr.push_back(Selectable::toString(values[i]));
            if (*currentValue == values[i]) {
                valueIndex = i;
            }
        }

        const bool result = ImGui::Combo(label, &valueIndex, valueStr.data(), static_cast<int>(valueStr.size()));
        if (result) {
            *currentValue = static_cast<Enum>(values[valueIndex]);
        }
        return result;
    }

    static void BeginSettings(const char* txt) {
        ImGui::SeparatorText(txt);
        ImGui::Indent();
        ImGui::PushID(txt);
    }
    static void EndSettings() {
        ImGui::PopID();
        ImGui::Unindent();
    }

    static void NextSettings(const char* txt) {
        EndSettings();
        BeginSettings(txt);
    }

    static void HelpMarker(const char *desc) {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(desc);
            ImGui::EndTooltip();
        }
    }
}