//
// Created by Merutilm on 2025-05-31.
//

#include "ResolutionPresets.h"

#include <array>


namespace merutilm::rff2 {
    std::wstring ResolutionPresets::L1::getName() const {
        return L"640x360";
    }
    std::array<int, 2> ResolutionPresets::L1::genResolution() const {
        return {640, 360};
    }
    std::wstring ResolutionPresets::L2::getName() const {
        return L"960x540";
    }
    std::array<int, 2> ResolutionPresets::L2::genResolution() const {
        return {960, 540};
    }
    std::wstring ResolutionPresets::L3::getName() const {
        return L"1280x720";
    }
    std::array<int, 2> ResolutionPresets::L3::genResolution() const {
        return {1280, 720};
    }
    std::wstring ResolutionPresets::L4::getName() const {
        return L"1600x900";
    }
    std::array<int, 2> ResolutionPresets::L4::genResolution() const {
        return {1600, 900};
    }
    std::wstring ResolutionPresets::L5::getName() const {
        return L"1920x1080";
    }
    std::array<int, 2> ResolutionPresets::L5::genResolution() const {
        return {1920, 1080};
    }
}