//
// Created by Merutilm on 7/4/26.
//

#pragma once
#include <filesystem>

#include "vulkan_helper/base/exception.hpp"

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <limits.h>
#include <unistd.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <vector>
#endif

namespace merutilm::vkh::ExecutableUtils {

    inline std::filesystem::path getExecutablePath() {
#ifdef _WIN32

        std::string buffer(MAX_PATH, '\0');

        DWORD len = GetModuleFileName(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));

        if (len == 0)
            throw exception_init("failed to load executable path");

        buffer.resize(len);
        return std::filesystem::path(buffer);

#elif __linux__

        char buffer[PATH_MAX];

        const ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

        if (len == -1)
            throw exception_init("failed to load executable path");

        buffer[len] = '\0';
        return std::filesystem::path(buffer);

#elif __APPLE__

        uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size);

        std::vector<char> buffer(size);

        if (_NSGetExecutablePath(buffer.data(), &size) != 0)
            throw exception_init("failed to load executable path");

        return std::filesystem::canonical(buffer.data());

#else

#error Unsupported platform

#endif
    }

    inline std::filesystem::path getExecutableDirectory() { return getExecutablePath().parent_path(); }

}
