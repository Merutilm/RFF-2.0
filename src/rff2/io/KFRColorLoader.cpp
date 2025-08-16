//
// Created by Merutilm on 2025-07-16.
//

#include "KFRColorLoader.hpp"
#include <fstream>
#include "../ui/IOUtilities.h"

namespace merutilm::rff2 {
    std::vector<NormalizedColor> KFRColorLoader::loadPaletteSettings() {
        const auto pFile = IOUtilities::ioFileDialog("Open KFR Palette", Constants::Extension::DESC_KFR,
                                                     IOUtilities::OPEN_FILE, Constants::Extension::KFR);
        if (pFile == nullptr) {
            return {};
        }
        const auto &file = *pFile;
        std::ifstream stream(file, std::ios::in);
        if (!stream.is_open()) {
            MessageBox(nullptr, "Can't open KFR Palette", "Error", MB_OK | MB_ICONERROR);
            return {};
        }
        std::string line;
        const std::string token = "Colors: ";
        while (getline(stream, line)) {
            if (!line.starts_with(token)) {
                continue;
            }

            line = line.substr(token.length(), line.size());
            auto split = Utilities::split(line, ',');
            if (split.empty()) {
                return {};
            }
            auto result = std::vector<float>(split.size());
            std::ranges::transform(split, result.begin(), [](std::string str) {
                std::erase(str, ' ');
                return std::stof(str) / 255.0f;
            });
            auto out = std::vector<NormalizedColor>();
            for (uint32_t i = 0; i < result.size(); i += 3) {
                out.emplace_back(result[i + 2], result[i + 1], result[i + 0]);
            }
            return out;
        }
        return {};
    }
}
