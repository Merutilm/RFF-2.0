//
// Created by Merutilm on 2025-05-28.
//
#include "FogPresets.h"

std::string FogPresets::Medium::getName() {
    return "Medium";
}

FogSettings FogPresets::Medium::fogSettings() {
    return FogSettings{0.1, 0.5};
}
