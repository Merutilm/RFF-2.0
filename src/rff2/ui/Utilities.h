//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include <string>
#include <ctime>
#include <filesystem>
#include <iostream>

#include "../constants/Constants.hpp"

namespace merutilm::rff2 {
    struct Utilities {
        Utilities() = delete;

        static void logErr(const std::string &message) {
            std::cerr << getCurrentPutTime() << " | " << message << "\n" << std::flush;
            MessageBox(nullptr, message.data(), "Error", MB_ICONERROR | MB_OK);
        }

        static void log(const std::string &message) {
            std::cout << getCurrentPutTime() << " | " << message << "\n" << std::flush;
        }

        static std::_Put_time<char> getCurrentPutTime() {
            const auto t = std::chrono::system_clock::now();
            const auto time = std::chrono::system_clock::to_time_t(t);
            std::tm tm = {};
            localtime_s(&tm, &time);
            return std::put_time(&tm, "%Y/%m/%d, %H:%M:%S");
        }

        static std::string elapsed_time(const std::chrono::time_point<std::chrono::system_clock> start) {
            const auto current = std::chrono::system_clock::now();
            const auto elapsed = std::chrono::milliseconds((current - start).count() / 1000000);
            const auto hms = std::chrono::hh_mm_ss(elapsed);
            return std::format("T : {:02d}:{:02d}:{:02d}:{:03d}", hms.hours().count(),
                                         hms.minutes().count(), hms.seconds().count(), hms.subseconds().count());
        }

        static float getCurrentTime() {
            return static_cast<float>(std::chrono::system_clock::now().time_since_epoch().count() - Constants::Render::INIT_TIME)
                   / 1e9;
        }


        static std::filesystem::path getDefaultPath() {
            std::array<char, MAX_PATH> buffer;
            GetModuleFileName(nullptr, buffer.data(), buffer.size());
            return std::filesystem::path(buffer.data()).parent_path().parent_path();
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

        static int getRefreshInterval(const float logZoom) {
            return std::max(1, static_cast<int>(100000.0 / logZoom));
        };
    };
}