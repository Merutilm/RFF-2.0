//
// Created by Merutilm on 10/7/26.
//

#pragma once

#include <algorithm>
#include <unordered_map>


#include "../constants/UIConstants.hpp"
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

    static void BeginSettings(const char *txt) {
        ImGui::SeparatorText(txt);
        ImGui::Indent();
        ImGui::PushID(txt);
    }
    static void EndSettings() {
        ImGui::PopID();
        ImGui::Unindent();
    }

    static void NextSettings(const char *txt) {
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


    static void BeginSmoothScrollableChild(const char *id, const ImVec2 &size = ImVec2(0, 0), const ImGuiChildFlags flags = 0) {
        ImGui::BeginChild(id, size, flags, ImGuiWindowFlags_NoScrollWithMouse);

        static std::unordered_map<const char*, float> dstScrollMap;
        auto result = dstScrollMap.find(id);
        if (result == dstScrollMap.end()) {
            result = dstScrollMap.try_emplace(id, 0).first;
        }
        float& dstScroll = result->second;
        const auto &io = ImGui::GetIO();

        if (ImGui::IsWindowHovered() && io.MouseWheel != 0) {
            dstScroll -= Constants::UI::SMOOTH_SCROLL_SENSITIVITY * io.MouseWheel;
        }

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            dstScroll = ImGui::GetScrollY();
        }

        dstScroll = std::clamp(dstScroll, 0.0f, ImGui::GetScrollMaxY());

        const float t = 1.0f - std::exp(-Constants::UI::SMOOTH_SCROLL_SPEED * io.DeltaTime);
        const float scroll = std::lerp(ImGui::GetScrollY(), dstScroll, t);

        ImGui::SetScrollY(scroll);
    }


    static bool BeginTabItem(const char *str) {
        if (!ImGui::BeginTabItem(str))
            return false;

        BeginSmoothScrollableChild(str);
        return true;
    }
    static void EndTabItem() {
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
} // namespace merutilm::rff2::ImGuiUtils
