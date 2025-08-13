//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include <string>
#include <iostream>
#include <windows.h>
#include <chrono>

namespace merutilm::vkh {
    struct logger {
        logger() = delete;


        template<typename... Args>
        static void log_err_silent(std::format_string<Args...> message, Args &&... args) {
            std::cerr << current_put_time() << " | " << std::format(message, std::forward<Args>(args)...) << "\n" <<
                    std::flush;
        }

        template<typename... Args>
        static void log_err(std::format_string<Args...> message, Args &&... args) {
            log_err_silent(message, std::forward<Args>(args)...);
            auto fmt = std::format(message, std::forward<Args>(args)...);
            MessageBox(nullptr, fmt.data(), "Error", MB_ICONERROR | MB_OK);
        }

        template<typename... Args>
        static void log(std::format_string<Args...> message, Args &&... args) {
            std::cout << current_put_time() << " | " << std::format(message, std::forward<Args>(args)...) << "\n" <<
                    std::flush;
        }

        static std::_Put_time<char> current_put_time() {
            const auto t = std::chrono::system_clock::now();
            const auto time = std::chrono::system_clock::to_time_t(t);
            std::tm tm = {};
            localtime_s(&tm, &time);
            return std::put_time(&tm, "%Y/%m/%d, %H:%M:%S");
        }
    };
}
