//
// Created by Merutilm on 2025-06-08.
//

#pragma once
#include <string>
#include <vector>
#include <shlobj.h>


struct IOUtilities {
    IOUtilities() = delete;

    static constexpr char OPEN_FILE = 0;
    static constexpr char SAVE_FILE = 1;

    static std::string ioFileDialog(const std::string &title, const std::string &desc, char type, std::vector<std::string> &&extensions);

    static std::string ioDirectoryDialog(const std::string &title);
};
