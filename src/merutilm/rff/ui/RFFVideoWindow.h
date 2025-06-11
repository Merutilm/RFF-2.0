//
// Created by Merutilm on 2025-06-12.
//

#pragma once
#include <filesystem>

#include "../settings/Settings.h"

class RFFVideoWindow {

public:
    static void createVideo(Settings settings, const std::filesystem::path &open, const std::filesystem::path &save);
};
