//
// Created by Merutilm on 2025-05-28.
//
#include "SlopePresets.h"



std::string SlopePresets::Normal::getName() {
    return "Normal";
}

SlopeSettings SlopePresets::Normal::slopeSettings() {
    return SlopeSettings{300, 0.5f, 1, 60, 135};
}
