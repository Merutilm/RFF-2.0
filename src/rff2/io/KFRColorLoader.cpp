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
        const auto pFile =
                IOUtilities::ioFileDialog(Constants::File::DESC_KFR, IOUtilities::OPEN_FILE, Constants::File::EXT_KFR);
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
            auto result = std::vector<float>();
            result.reserve(split.size());
            for (auto &str: split) {
                str.erase(std::ranges::remove_if(str, [](const unsigned char c) { return std::isspace(c); }).begin(),
                          str.end());

                if (!str.empty()) {
                    result.push_back(std::stof(str) / 255.0f);
                }
            }

            auto out = std::vector<glm::vec4>();
            for (uint32_t i = 0; i < result.size(); i += 3) {
                out.emplace_back(result[i + 2], result[i + 1], result[i + 0], 1);
            }
            return out;
        }
        return {};
    }
} // namespace merutilm::rff2
