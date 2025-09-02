//
// Created by Merutilm on 2025-05-19.
//
#pragma once
#include <codecvt>
#include <locale>
#include <windows.h>
#include <string>

namespace merutilm::rff2 {
    namespace ValidCondition {
        constexpr auto POSITIVE_CHAR = [](const char &e) { return e > 0; };
        constexpr auto NEGATIVE_CHAR = [](const char &e) { return e < 0; };
        constexpr auto POSITIVE_CHAR_ZERO = [](const char &e) { return e >= 0; };
        constexpr auto NEGATIVE_CHAR_ZERO = [](const char &e) { return e <= 0; };
        constexpr auto POSITIVE_U_CHAR = [](const unsigned char &e) { return e > 0; };
        constexpr auto ALL_U_CHAR = [](const unsigned char &) { return true; };
        constexpr auto POSITIVE_SHORT = [](const short &e) { return e > 0; };
        constexpr auto NEGATIVE_SHORT = [](const short &e) { return e < 0; };
        constexpr auto POSITIVE_SHORT_ZERO = [](const short &e) { return e >= 0; };
        constexpr auto NEGATIVE_SHORT_ZERO = [](const short &e) { return e <= 0; };
        constexpr auto POSITIVE_U_SHORT = [](const unsigned short &e) { return e > 0; };
        constexpr auto ALL_U_SHORT = [](const unsigned short &) { return true; };
        constexpr auto POSITIVE_INT = [](const int &e) { return e > 0; };
        constexpr auto NEGATIVE_INT = [](const int &e) { return e < 0; };
        constexpr auto POSITIVE_INT_ZERO = [](const int &e) { return e >= 0; };
        constexpr auto NEGATIVE_INT_ZERO = [](const int &e) { return e <= 0; };
        constexpr auto POSITIVE_LONG = [](const long &e) { return e > 0; };
        constexpr auto NEGATIVE_LONG = [](const long &e) { return e < 0; };
        constexpr auto POSITIVE_LONG_ZERO = [](const long &e) { return e >= 0; };
        constexpr auto NEGATIVE_LONG_ZERO = [](const long &e) { return e <= 0; };
        constexpr auto POSITIVE_LONG_LONG = [](const long long &e) { return e > 0; };
        constexpr auto NEGATIVE_LONG_LONG = [](const long long &e) { return e < 0; };
        constexpr auto POSITIVE_LONG_LONG_ZERO = [](const long long &e) { return e >= 0; };
        constexpr auto NEGATIVE_LONG_LONG_ZERO = [](const long long &e) { return e <= 0; };
        constexpr auto POSITIVE_U_LONG = [](const unsigned long &e) { return e > 0; };
        constexpr auto ALL_U_LONG = [](const unsigned long) { return true; };
        constexpr auto POSITIVE_U_LONG_LONG = [](const unsigned long long &e) { return e > 0; };
        constexpr auto ALL_U_LONG_LONG = [](const unsigned long long) { return true; };
        constexpr auto FLOAT_ZERO_TO_ONE = [](const float &e) { return e >= 0 && e <= 1; };
        constexpr auto FLOAT_DEGREE = [](const float &e) { return e >= 0 && e < 360; };
        constexpr auto ALL_FLOAT = [](const float &) { return true; };
        constexpr auto POSITIVE_FLOAT = [](const float &e) { return e > 0; };
        constexpr auto NEGATIVE_FLOAT = [](const float &e) { return e < 0; };
        constexpr auto POSITIVE_FLOAT_ZERO = [](const float &e) { return e >= 0; };
        constexpr auto NEGATIVE_FLOAT_ZERO = [](const float &e) { return e <= 0; };
        constexpr auto DOUBLE_ZERO_TO_ONE = [](const double &e) { return e >= 0 && e <= 1; };
        constexpr auto DOUBLE_DEGREE = [](const double &e) { return e >= 0 && e < 360; };
        constexpr auto ALL_DOUBLE = [](const double &) { return true; };
        constexpr auto POSITIVE_DOUBLE = [](const double &e) { return e > 0; };
        constexpr auto NEGATIVE_DOUBLE = [](const double &e) { return e < 0; };
        constexpr auto POSITIVE_DOUBLE_ZERO = [](const double &e) { return e >= 0; };
        constexpr auto NEGATIVE_DOUBLE_ZERO = [](const double &e) { return e <= 0; };
        constexpr auto POSITIVE_LONG_DOUBLE = [](const long double &e) { return e > 0; };
        constexpr auto NEGATIVE_LONG_DOUBLE = [](const long double &e) { return e < 0; };
        constexpr auto POSITIVE_LONG_DOUBLE_ZERO = [](const long double &e) { return e >= 0; };
        constexpr auto NEGATIVE_LONG_DOUBLE_ZERO = [](const long double &e) { return e <= 0; };
    }

    namespace Callback {
        constexpr auto NOTHING = [] {
            /*NO CALLBACKS*/
        };
    }

    namespace Parser {
        constexpr auto STRING = [](const std::wstring &s) {
            const int size = WideCharToMultiByte(CP_UTF8, 0, s.data(), -1, nullptr, 0, nullptr, nullptr);
            std::string str(size, 0);
            WideCharToMultiByte(CP_UTF8, 0, s.data(), -1, &str[0], size, nullptr, nullptr);
            str.pop_back();
            return str;
        };
        constexpr auto WSTRING = [](const std::wstring &s) { return s; };
        constexpr auto CHAR = [](const std::wstring &s) { return static_cast<char>(std::stoi(s) & 0xFF); };
        constexpr auto U_CHAR = [](const std::wstring &s) { return static_cast<unsigned char>(std::stoul(s) & 0xFF); };
        constexpr auto SHORT = [](const std::wstring &s) { return static_cast<short>(std::stoi(s) & 0xFFFF); };
        constexpr auto U_SHORT = [](const std::wstring &s) {
            return static_cast<unsigned short>(std::stoul(s) & 0xFFFF);
        };
        constexpr auto INT = [](const std::wstring &s) { return std::stoi(s); };
        constexpr auto LONG = [](const std::wstring &s) { return std::stol(s); };
        constexpr auto LONG_LONG = [](const std::wstring &s) { return std::stoll(s); };
        constexpr auto U_LONG = [](const std::wstring &s) { return std::stoul(s); };
        constexpr auto U_LONG_LONG = [](const std::wstring &s) { return std::stoull(s); };
        constexpr auto FLOAT = [](const std::wstring &s) { return std::stof(s); };
        constexpr auto DOUBLE = [](const std::wstring &s) { return std::stod(s); };
        constexpr auto LONG_DOUBLE = [](const std::wstring &s) { return std::stold(s); };
    }

    namespace Unparser {
        constexpr auto STRING = [](const std::string &s) {
            const int size = MultiByteToWideChar(CP_UTF8, 0, s.data(), -1, nullptr, 0);
            std::wstring str(size, 0);
            MultiByteToWideChar(CP_UTF8, 0, s.data(), -1, &str[0], size);
            str.pop_back();
            return str;
        };
        constexpr auto WSTRING = [](const std::wstring &s) { return s; };
        constexpr auto CHAR = [](const char &s) { return std::to_wstring(s); };
        constexpr auto U_CHAR = [](const unsigned char &s) { return std::to_wstring(s); };
        constexpr auto SHORT = [](const short &s) { return std::to_wstring(s); };
        constexpr auto U_SHORT = [](const unsigned short &s) { return std::to_wstring(s); };
        constexpr auto INT = [](const int &s) { return std::to_wstring(s); };
        constexpr auto LONG = [](const long &s) { return std::to_wstring(s); };
        constexpr auto LONG_LONG = [](const long long &s) { return std::to_wstring(s); };
        constexpr auto U_LONG = [](const unsigned long &s) { return std::to_wstring(s); };
        constexpr auto U_LONG_LONG = [](const unsigned long long &s) { return std::to_wstring(s); };
        constexpr auto FLOAT = [](const float &s) { return std::to_wstring(s); };
        constexpr auto DOUBLE = [](const double &s) { return std::to_wstring(s); };
        constexpr auto LONG_DOUBLE = [](const long double &s) { return std::to_wstring(s); };
    }
}
