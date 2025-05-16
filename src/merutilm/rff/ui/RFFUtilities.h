//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include <string>
#include <ctime>
#include <iomanip>
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
        return static_cast<float>(std::chrono::system_clock::now().time_since_epoch().count() - RFF::Render::INIT_TIME) / 1e9;
    }

    static int getRefreshInterval(float logZoom) {
        return std::max(1, static_cast<int>(100000.0 / logZoom));
    };
};
