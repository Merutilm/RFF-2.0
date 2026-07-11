//
// Created by Merutilm on 2025-07-16.
//

#include "KFRColorLoader.hpp"
#include <fstream>
#include "../constants/Constants.hpp"
#include "../ui/IOUtilities.h"
#include "../ui/Utilities.h"
#include "vulkan_helper/base/logger.hpp"

namespace merutilm::rff2 {
    std::vector<glm::vec4> KFRColorLoader::loadPaletteSettings() {
        const auto pFile = IOUtilities::ioFileDialog(Constants::File::DESC_KFR,
                                                     IOUtilities::OPEN_FILE, Constants::File::EXT_KFR);
        if (pFile == nullptr) {
            return {};
        }
        const auto &file = *pFile;
        std::ifstream stream(file, std::ios::in);
        if (!stream.is_open()) {
            vkh::logger::log_err("Can't open KFR Palette");
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
            auto out = std::vector<glm::vec4>();
            for (uint32_t i = 0; i < result.size(); i += 3) {
                out.emplace_back(result[i + 2], result[i + 1], result[i + 0], 1);
            }
            return out;
        }
        return {};
    }
}
