//
// Created by Merutilm on 2025-05-31.
//

#include "ResolutionPresets.h"

#include <array>


std::string merutilm::rff::ResolutionPresets::L1::getName() const {
    return "640x360";
}
std::array<int, 2> merutilm::rff::ResolutionPresets::L1::getResolution() const {
    return {640, 360};
}
std::string merutilm::rff::ResolutionPresets::L2::getName() const {
    return "960x540";
}
std::array<int, 2> merutilm::rff::ResolutionPresets::L2::getResolution() const {
    return {960, 540};
}
std::string merutilm::rff::ResolutionPresets::L3::getName() const {
    return "1280x720";
}
std::array<int, 2> merutilm::rff::ResolutionPresets::L3::getResolution() const {
    return {1280, 720};
}
std::string merutilm::rff::ResolutionPresets::L4::getName() const {
    return "1600x900";
}
std::array<int, 2> merutilm::rff::ResolutionPresets::L4::getResolution() const {
    return {1600, 900};
}
std::string merutilm::rff::ResolutionPresets::L5::getName() const {
    return "1920x1080";
}
std::array<int, 2> merutilm::rff::ResolutionPresets::L5::getResolution() const {
    return {1920, 1080};
}
