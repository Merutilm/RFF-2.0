//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include <string>
#include <ctime>
#include <filesystem>

#include "../constants/Constants.hpp"

namespace merutilm::rff2 {
    struct Utilities {
        Utilities() = delete;

        static std::wstring elapsed_time(const std::chrono::time_point<std::chrono::system_clock> start) {
            const auto current = std::chrono::system_clock::now();
            const auto elapsed = std::chrono::milliseconds((current - start).count() / 1000000);
            const auto hms = std::chrono::hh_mm_ss(elapsed);
            return std::format(L"T : {:02d}:{:02d}:{:02d}:{:03d}", hms.hours().count(),
                                         hms.minutes().count(), hms.seconds().count(), hms.subseconds().count());
        }

        static float getCurrentTime() {
            return static_cast<float>(std::chrono::system_clock::now().time_since_epoch().count() - Constants::Fractal::INIT_TIME)
                   / 1e9;
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

        static int getRefreshInterval(const float logZoom) {
            return std::max(1, static_cast<int>(100000.0 / logZoom));
        };
    };
}