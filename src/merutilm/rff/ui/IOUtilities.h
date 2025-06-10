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

#include "RFFUtilities.h"


struct IOUtilities {
    IOUtilities() = delete;

    static constexpr char OPEN_FILE = 0;
    static constexpr char SAVE_FILE = 1;

    static std::unique_ptr<std::filesystem::path> ioFileDialog(std::string_view title, std::string_view desc,
                                                               char type, std::vector<std::string> &&extensions);

    static std::unique_ptr<std::filesystem::path> ioDirectoryDialog(std::string_view title);

    static std::string fileNameFormat(unsigned int n, std::string_view extension);

    static std::filesystem::path generateFileName(const std::filesystem::path &dir, std::string_view extension);

    static int fileNameCount(const std::filesystem::path &dir, std::string_view extension);

    template<typename T> requires std::is_arithmetic_v<T>
    static void encodeAndWrite(std::ofstream &out, const T &t);

    template<typename T> requires std::is_arithmetic_v<T>
    static void encodeAndWrite(std::ofstream &out, const std::vector<T> &t);

    template<typename T> requires std::is_arithmetic_v<T>
    static void readAndDecode(std::ifstream &in, T *t);

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

template<typename T> requires std::is_arithmetic_v<T>
void IOUtilities::encodeAndWrite(std::ofstream &out, const std::vector<T> &t) {
    std::vector<char> ot;
    for (double et : t) {
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
