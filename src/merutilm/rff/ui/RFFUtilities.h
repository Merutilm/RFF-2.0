//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include <string>
#include <ctime>
#include <filesystem>
#include <iostream>

#include "RFF.h"

struct RFFUtilities {
    RFFUtilities() = delete;

    static void log(const std::string &message) {
        const auto t = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(t);
        std::tm tm = {};
        localtime_s(&tm, &time);
        std::cout << std::put_time(&tm, "%Y/%m/%d, %H:%M:%S") << " | " << message << "\n" << std::flush;
    }

    static float getTime() {
        return static_cast<float>(std::chrono::system_clock::now().time_since_epoch().count() - RFF::Render::INIT_TIME)
               / 1e9;
    }


    static std::filesystem::path getDefaultPath() {
        std::array<char, MAX_PATH> buffer;
        GetModuleFileName(nullptr, buffer.data(), buffer.size());
        return std::filesystem::path(buffer.data()).parent_path().parent_path();
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

    static int getRefreshInterval(const float logZoom) {
        return std::max(1, static_cast<int>(100000.0 / logZoom));
    };
};
