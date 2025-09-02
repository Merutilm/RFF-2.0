//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "vkh_base.hpp"

namespace merutilm::vkh {
    struct logger {
        logger() = delete;

        template<typename... Args>
        static void log_err_silent(std::format_string<Args...> message, Args &&... args) {
            const std::tm tm = get_tm();
            std::cerr << current_put_time(&tm) << " | " << std::format(message, std::forward<Args>(args)...) << "\n" <<
                    std::flush;
        }

        template<typename... Args>
        static void w_log_err_silent(std::wformat_string<Args...> message, Args &&... args) {
            const std::tm tm = get_tm();
            std::wcerr << w_current_put_time(&tm) << " | " << std::format(message, std::forward<Args>(args)...) << "\n" <<
                    std::flush;
        }

        template<typename... Args>
        static void log_err(std::format_string<Args...> message, Args &&... args) {
            log_err_silent(message, std::forward<Args>(args)...);
            auto fmt = std::format(message, std::forward<Args>(args)...);
            MessageBox(nullptr, fmt.data(), "Error", MB_ICONERROR | MB_OK);
        }

        template<typename... Args>
        static void w_log_err(std::wformat_string<Args...> message, Args &&... args) {
            w_log_err_silent(message, std::forward<Args>(args)...);
            auto fmt = std::format(message, std::forward<Args>(args)...);
            MessageBoxW(nullptr, fmt.data(), L"Error", MB_ICONERROR | MB_OK);
        }

        template<typename... Args>
        static void log(std::format_string<Args...> message, Args &&... args) {
            const std::tm tm = get_tm();
            std::cout << current_put_time(&tm) << " | " << std::format(message, std::forward<Args>(args)...) << "\n" <<
                    std::flush;
        }

        template<typename... Args>
        static void w_log(std::wformat_string<Args...> message, Args &&... args) {
            const std::tm tm = get_tm();
            std::wcout << w_current_put_time(&tm) << " | " << std::format(message, std::forward<Args>(args)...) << "\n" <<
                    std::flush;
        }

        static std::tm get_tm() {
            const auto t = std::chrono::system_clock::now();
            const auto time = std::chrono::system_clock::to_time_t(t);
            std::tm tm = {};
            localtime_s(&tm, &time);
            return tm;
        }

        static std::_Put_time<char> current_put_time(const std::tm * const tm) {
            return std::put_time(tm, "%Y/%m/%d, %H:%M:%S");
        }

        static std::_Put_time<wchar_t> w_current_put_time(const std::tm * const tm) {
            return std::put_time(tm, L"%Y/%m/%d, %H:%M:%S");
        }
    };
}
