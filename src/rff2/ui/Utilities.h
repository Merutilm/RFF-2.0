//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include <filesystem>
#include <string>
#include "../constants/Constants.hpp"

namespace merutilm::rff2::Utilities {

    static std::wstring formatTime(const float seconds) {
        const auto secondsI = static_cast<uint32_t>(seconds);
        const auto millis = static_cast<uint32_t>(std::fmodf(seconds, 1) * 1000);
        const auto sec = secondsI % 60;
        const auto min = secondsI / 60 % 60;
        const auto hour = secondsI / 3600;
        return std::format(L"{:02}:{:02}:{:02}:{:03}", hour, min, sec, millis);
    }

    static std::wstring formatTime(const uint32_t seconds) {
        const auto sec = seconds % 60;
        const auto min = seconds / 60 % 60;
        const auto hour = seconds / 3600;
        return std::format(L"{:02}:{:02}:{:02}", hour, min, sec);
    }

    static std::wstring elapsed_time(const std::chrono::time_point<std::chrono::high_resolution_clock> start) {
        const auto current = std::chrono::high_resolution_clock::now();
        const auto elapsed = std::chrono::duration<float>(current - start).count();
        return std::format(L"T : {}", formatTime(elapsed));
    }


    static float getCurrentTime() {
        using namespace std::chrono;
        const float time = static_cast<float>(high_resolution_clock::now().time_since_epoch().count() -
                                              Constants::Fractal::INIT_TIME) /
                           1e9f;
        return time;
    }


    static std::filesystem::path getDefaultPath() {
        std::array<wchar_t, MAX_PATH> buffer;
        GetModuleFileNameW(nullptr, buffer.data(), buffer.size());
        return std::filesystem::path(buffer.data()).parent_path().parent_path();
    }

    static bool endsWith(const std::wstring &str, const std::wstring &suffix) {
        return str.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }

    static std::wstring joinString(const std::wstring &delimiter, const std::vector<std::wstring> &arr) {
        std::wostringstream v;
        for (int i = 0; i < arr.size(); ++i) {
            if (i > 0) {
                v << delimiter;
            }
            v << arr[i];
        }
        return v.str();
    }

    static std::vector<std::wstring> split(const std::wstring &input, const wchar_t delimiter) {
        std::vector<std::wstring> split;
        std::wstringstream ss(input);
        std::wstring val;

        while (getline(ss, val, delimiter)) {
            split.push_back(val);
        }

        return split;
    }

    static int getRefreshInterval(const float logZoom) { return std::max(1, static_cast<int>(100000.0 / logZoom)); }
} // namespace merutilm::rff2::Utilities
