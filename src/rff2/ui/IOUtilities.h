//
// Created by Merutilm on 2025-06-08.
//

#pragma once
#include <algorithm>
#include <array>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <vector>
#include <shlobj.h>

#include "Utilities.h"

namespace merutilm::rff2 {
    struct IOUtilities {
        IOUtilities() = delete;

        static constexpr char OPEN_FILE = 0;
        static constexpr char SAVE_FILE = 1;

        static std::unique_ptr<std::filesystem::path> ioFileDialog(std::wstring_view title, std::wstring_view desc,
                                                                   char type, std::wstring_view extension);

        static std::unique_ptr<std::filesystem::path> ioDirectoryDialog(std::wstring_view title);

        static std::wstring fileNameFormat(unsigned int n, std::wstring_view extension);

        static std::filesystem::path generateFileName(const std::filesystem::path &dir, std::wstring_view extension);

        static uint32_t fileNameCount(const std::filesystem::path &dir, std::wstring_view extension);

        template<typename T> requires std::is_arithmetic_v<T>
        static void encodeAndWrite(std::ofstream &out, const T &t);

        static void encodeAndWrite(std::ofstream &out, const char *t, uint64_t length);

        template<typename T> requires std::is_arithmetic_v<T>
        static void encodeAndWrite(std::ofstream &out, const std::vector<T> &t);

        template<typename T> requires std::is_arithmetic_v<T>
        static void readAndDecode(std::ifstream &in, T *t);

        static void readAndDecode(std::ifstream &in, uint64_t length, char *t);

        template<typename T> requires std::is_arithmetic_v<T>
        static void readAndDecode(std::ifstream &in, std::vector<T> *t);

        template<typename T> requires std::is_arithmetic_v<T>
        static std::array<char, sizeof(T)> toBinaryArray(const T &v);

        template<typename T> requires std::is_arithmetic_v<T>
        static void fromBinaryArray(const std::array<char, sizeof(T)> &arr, T *result);
    };


    template<typename T> requires std::is_arithmetic_v<T>
    void IOUtilities::encodeAndWrite(std::ofstream &out, const T &t) {
        const auto ot = toBinaryArray(t);
        out.write(ot.data(), ot.size());
    }

    inline void IOUtilities::encodeAndWrite(std::ofstream &out, const char *t, const uint64_t length) {
        out.write(t, length);
    }

    template<typename T> requires std::is_arithmetic_v<T>
    void IOUtilities::encodeAndWrite(std::ofstream &out, const std::vector<T> &t) {
        std::vector<char> ot;
        for (double et: t) {
            const auto oi = toBinaryArray(et);
            ot.insert(ot.end(), oi.begin(), oi.end());
        }
        out.write(ot.data(), ot.size());
    }


    template<typename T> requires std::is_arithmetic_v<T>
    void IOUtilities::readAndDecode(std::ifstream &in, T *t) {
        auto it = std::array<char, sizeof(T)>();
        in.read(it.data(), it.size());
        fromBinaryArray(it, t);
    }

    inline void IOUtilities::readAndDecode(std::ifstream &in, const uint64_t length, char *t) {
        in.read(t, length);
    }

    template<typename T> requires std::is_arithmetic_v<T>
    void IOUtilities::readAndDecode(std::ifstream &in, std::vector<T> *t) {
        auto it = std::vector<char>(t->size() * sizeof(T));
        in.read(it.data(), it.size());

        for (uint32_t i = 0; i < it.size(); i += sizeof(T)) {
            auto iSubArr = std::array<char, sizeof(T)>();
            std::memcpy(&iSubArr, &it[i], sizeof(T));
            fromBinaryArray(iSubArr, &(*t)[i / sizeof(T)]);
        }
    }

    template<typename T> requires std::is_arithmetic_v<T>
    std::array<char, sizeof(T)> IOUtilities::toBinaryArray(const T &v) {
        std::array<char, sizeof(T)> arr;
        std::memcpy(arr.data(), &v, sizeof(T));
        return arr;
    }

    template<typename T> requires std::is_arithmetic_v<T>
    void IOUtilities::fromBinaryArray(const std::array<char, sizeof(T)> &arr, T *result) {
        std::memcpy(result, arr.data(), sizeof(T));
    }
};
