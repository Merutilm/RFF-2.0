//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include <cmath>
#include <filesystem>
#include <string>

#include <format>
#include <sstream>
#include <stack>
#include <iomanip>

#include "../settings/Selectable.h"
#include "imgui.h"
#include "vulkan_helper/util/ExecutableUtils.hpp"

namespace merutilm::rff2::Utilities {

    inline std::string formatTime(const double seconds) {
        const auto secondsI = static_cast<uint64_t>(seconds);
        const auto millis = static_cast<uint64_t>(std::fmod(seconds, 1) * 1000);
        const auto sec = secondsI % 60;
        const auto min = secondsI / 60 % 60;
        const auto hour = secondsI / 3600 % 24;
        const auto day = secondsI / 86400;
        if (day > 0) {
            return std::format("{:02}:{:02}:{:02}:{:02}:{:03}", day, hour, min, sec, millis);
        }else {
            return std::format("{:02}:{:02}:{:02}:{:03}", hour, min, sec, millis);
        }
    }

    inline std::string formatTime(const uint64_t seconds) {
        const auto sec = seconds % 60;
        const auto min = seconds / 60 % 60;
        const auto hour = seconds / 3600 % 24;
        const auto day = seconds / 86400;
        if (day > 0) {
            return std::format("{:02}:{:02}:{:02}:{:02}", day, hour, min, sec);
        }else {
            return std::format("{:02}:{:02}:{:02}", hour, min, sec);
        }
    }


    inline std::filesystem::path getDefaultPath() {
        return std::filesystem::path(vkh::ExecutableUtils::getExecutablePath()).parent_path().parent_path();
    }

    inline bool endsWith(const std::string_view str, const std::string_view suffix) {
        return str.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }

    inline std::string joinString(const std::string &delimiter, const std::vector<std::string> &arr) {
        std::ostringstream v;
        for (int i = 0; i < arr.size(); ++i) {
            if (i > 0) {
                v << delimiter;
            }
            v << arr[i];
        }
        return v.str();
    }

    inline std::vector<std::string> split(const std::string &input, const char delimiter) {
        std::vector<std::string> split;
        std::stringstream ss(input);
        std::string val;

        while (getline(ss, val, delimiter)) {
            split.push_back(val);
        }

        return split;
    }


    inline std::string formatByte(const size_t size) {
        const uint64_t k = 1024;
        const uint64_t m = k * 1024;
        const uint64_t g = m * 1024;

        if (size >= g) {
            return std::format("{:.2F} GiB", static_cast<double>(size) / g);
        }
        if (size >= m) {
            return std::format("{:.2F} MiB", static_cast<double>(size) / m);
        }
        if (size >= k) {
            return std::format("{:.2F} KiB", static_cast<double>(size) / k);
        }
        return std::format("{:d} B", size);
    }
    inline std::string formatThousands(uint64_t value) {
        if (value == 0) return "0";

        std::array<char, 27> buffer{}; //27 = max string length of uint64_t + comma
        constexpr uint64_t len = buffer.size();
        char* p = buffer.data() + len - 1;
        *p = '\0';

        do {
            *--p = static_cast<char>('0' + static_cast<char>(value % 10));
            value /= 10;

            if (value != 0 && (buffer.data() + len - p) % 4 == 0) {
                *--p = ',';
            }
        } while (value != 0);
        assert(p >= buffer.data());
        return std::string(p);
    }
} // namespace merutilm::rff2::Utilities
