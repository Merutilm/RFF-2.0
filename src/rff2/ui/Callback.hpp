//
// Created by Merutilm on 2025-05-19.
//
#pragma once
#include <string>
#include <format>
#include <cstdint>

namespace merutilm::rff2 {
    namespace ValidCondition {
        inline static const auto POSITIVE_INT8 = [](const int8_t &e) { return e > 0; };
        inline static const auto NEGATIVE_INT8 = [](const int8_t &e) { return e < 0; };
        inline static const auto POSITIVE_INT8_ZERO = [](const int8_t &e) { return e >= 0; };
        inline static const auto NEGATIVE_INT8_ZERO = [](const int8_t &e) { return e <= 0; };
        inline static const auto POSITIVE_UINT8 = [](const uint8_t &e) { return e > 0; };
        inline static const auto ALL_UINT8 = [](const uint8_t &) { return true; };
        inline static const auto POSITIVE_INT16 = [](const int16_t &e) { return e > 0; };
        inline static const auto NEGATIVE_INT16 = [](const int16_t &e) { return e < 0; };
        inline static const auto POSITIVE_INT16_ZERO = [](const int16_t &e) { return e >= 0; };
        inline static const auto NEGATIVE_INT16_ZERO = [](const int16_t &e) { return e <= 0; };
        inline static const auto POSITIVE_UINT16 = [](const uint16_t &e) { return e > 0; };
        inline static const auto ALL_UINT16 = [](const uint16_t &) { return true; };
        inline static const auto POSITIVE_INT32 = [](const int32_t &e) { return e > 0; };
        inline static const auto NEGATIVE_INT32 = [](const int32_t &e) { return e < 0; };
        inline static const auto POSITIVE_INT32_ZERO = [](const int32_t &e) { return e >= 0; };
        inline static const auto NEGATIVE_INT32_ZERO = [](const int32_t &e) { return e <= 0; };
        inline static const auto POSITIVE_UINT32 = [](const uint32_t &e) { return e > 0; };
        inline static const auto ALL_UINT32 = [](const uint32_t) { return true; };
        inline static const auto POSITIVE_INT64 = [](const int64_t &e) { return e > 0; };
        inline static const auto NEGATIVE_INT64 = [](const int64_t &e) { return e < 0; };
        inline static const auto POSITIVE_INT64_ZERO = [](const int64_t &e) { return e >= 0; };
        inline static const auto NEGATIVE_INT64_ZERO = [](const int64_t &e) { return e <= 0; };
        inline static const auto POSITIVE_UINT64 = [](const uint64_t &e) { return e > 0; };
        inline static const auto ALL_UINT64 = [](const uint64_t) { return true; };
        inline static const auto FLOAT_ZERO_TO_ONE = [](const float &e) { return e >= 0 && e <= 1; };
        inline static const auto FLOAT_DEGREE = [](const float &e) { return e >= 0 && e < 360; };
        inline static const auto ALL_FLOAT = [](const float &) { return true; };
        inline static const auto POSITIVE_FLOAT = [](const float &e) { return e > 0; };
        inline static const auto NEGATIVE_FLOAT = [](const float &e) { return e < 0; };
        inline static const auto POSITIVE_FLOAT_ZERO = [](const float &e) { return e >= 0; };
        inline static const auto NEGATIVE_FLOAT_ZERO = [](const float &e) { return e <= 0; };
        inline static const auto DOUBLE_ZERO_TO_ONE = [](const double &e) { return e >= 0 && e <= 1; };
        inline static const auto DOUBLE_DEGREE = [](const double &e) { return e >= 0 && e < 360; };
        inline static const auto ALL_DOUBLE = [](const double &) { return true; };
        inline static const auto POSITIVE_DOUBLE = [](const double &e) { return e > 0; };
        inline static const auto NEGATIVE_DOUBLE = [](const double &e) { return e < 0; };
        inline static const auto POSITIVE_DOUBLE_ZERO = [](const double &e) { return e >= 0; };
        inline static const auto NEGATIVE_DOUBLE_ZERO = [](const double &e) { return e <= 0; };
    }

    namespace Callback {
        inline static const auto NOTHING = [] {
            /*NO CALLBACKS*/
        };
    }

    namespace Parser {
        inline static const auto STRING = [](const std::string &s) { return s; };
        inline static const auto INT8 = [](const std::string &s) { return static_cast<int8_t>(std::stoll(s) & 0xFF); };
        inline static const auto UINT8 = [](const std::string &s) { return static_cast<uint8_t>(std::stoull(s) & 0xFF); };
        inline static const auto INT16 = [](const std::string &s) { return static_cast<int16_t>(std::stoll(s) & 0xFFFF); };
        inline static const auto UINT16 = [](const std::string &s) {return static_cast<uint16_t>(std::stoull(s) & 0xFFFF);};
        inline static const auto INT32 = [](const std::string &s) { return static_cast<int32_t>(std::stoll(s)); };
        inline static const auto UINT32 = [](const std::string &s) { return static_cast<uint32_t>(std::stoull(s)); };
        inline static const auto INT64 = [](const std::string &s) { return static_cast<int64_t>(std::stoll(s)); };
        inline static const auto UINT64 = [](const std::string &s) { return static_cast<uint64_t>(std::stoull(s)); };
        inline static const auto FLOAT = [](const std::string &s) { return std::stof(s); };
        inline static const auto DOUBLE = [](const std::string &s) { return std::stod(s); };
    }

    namespace Unparser {
        inline static const auto STRING = [](const std::string &s) { return s; };
        inline static const auto INT8 = [](const uint8_t &s) { return std::format("{}", s); };
        inline static const auto UINT8 = [](const uint8_t &s) { return std::format("{}", s); };
        inline static const auto INT16 = [](const uint16_t &s) { return std::format("{}", s); };
        inline static const auto UINT16 = [](const uint16_t &s) { return std::format("{}", s); };
        inline static const auto INT32 = [](const int32_t &s) { return std::format("{}", s); };
        inline static const auto UINT32 = [](const uint32_t &s) { return std::format("{}", s); };
        inline static const auto INT64 = [](const int64_t &s) { return std::format("{}", s); };
        inline static const auto UINT64 = [](const uint64_t &s) { return std::format("{}", s); };
        inline static const auto FLOAT = [](const float &s) { return std::format("{}", s); };
        inline static const auto DOUBLE = [](const double &s) { return std::format("{}", s); };
    }
}
