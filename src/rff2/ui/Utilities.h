//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include <cmath>
#include <filesystem>
#include <string>

#include "../constants/Constants.hpp"
#include "../settings/Selectable.h"
#include "imgui.h"
#include "vulkan_helper/util/ExecutableUtils.hpp"

namespace merutilm::rff2::Utilities {

    static std::string formatTime(const float seconds) {
        const auto secondsI = static_cast<uint32_t>(seconds);
        const auto millis = static_cast<uint32_t>(std::fmod(seconds, 1) * 1000);
        const auto sec = secondsI % 60;
        const auto min = secondsI / 60 % 60;
        const auto hour = secondsI / 3600;
        return std::format("{:02}:{:02}:{:02}:{:03}", hour, min, sec, millis);
    }

    static std::string formatTime(const uint32_t seconds) {
        const auto sec = seconds % 60;
        const auto min = seconds / 60 % 60;
        const auto hour = seconds / 3600;
        return std::format("{:02}:{:02}:{:02}", hour, min, sec);
    }


    static std::filesystem::path getDefaultPath() {
        return std::filesystem::path(vkh::ExecutableUtils::getExecutablePath()).parent_path().parent_path();
    }

    static bool endsWith(const std::string &str, const std::string &suffix) {
        return str.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }

    static std::string joinString(const std::string &delimiter, const std::vector<std::string> &arr) {
        std::ostringstream v;
        for (int i = 0; i < arr.size(); ++i) {
            if (i > 0) {
                v << delimiter;
            }
            v << arr[i];
        }
        return v.str();
    }

    static std::vector<std::string> split(const std::string &input, const char delimiter) {
        std::vector<std::string> split;
        std::stringstream ss(input);
        std::string val;

        while (getline(ss, val, delimiter)) {
            split.push_back(val);
        }

        return split;
    }

    template <typename Enum> requires std::is_enum_v<Enum>
    static bool imguiDropdown(const char * label, Enum *currentValue) {
        static const std::vector<Enum> values = Selectable::values<Enum>();
        static std::vector<const char*> valueStr;
        valueStr.reserve(values.size());

        const bool newlyAdded = valueStr.empty();

        int valueIndex = 0;
        for (int i = 0; i < values.size(); ++i) {
            if (newlyAdded) valueStr.push_back(Selectable::toString(values[i]));
            if (*currentValue == values[i]) {
                valueIndex = i;
            }
        }

        const bool result = ImGui::Combo(label, &valueIndex, valueStr.data(), valueStr.size());
        if (result) {
            *currentValue = static_cast<Enum>(values[valueIndex]);
        }
        return result;
    }

    static void imguiHelpMarker(const char* desc)
    {
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(desc);
            ImGui::EndTooltip();
        }
    }

    static int getRefreshInterval(const float logZoom) { return std::max(1, static_cast<int>(100000.0 / logZoom)); }
} // namespace merutilm::rff2::Utilities
