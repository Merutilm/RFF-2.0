//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include <string>
#include <ctime>
#include <iomanip>
#include <iostream>

struct RFFLogger {
    RFFLogger() = delete;

    static void log(const std::string &message) {
        const auto t = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(t);
        std::tm tm = {};
        localtime_s(&tm, &time);
        std::cout << std::put_time(&tm, "%Y/%m/%d, %H:%M:%S") << " | " << message << "\n" << std::flush;
    }
};
