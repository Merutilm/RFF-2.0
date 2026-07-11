//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include "VidAnimationSettings.h"
#include "VidDataSettings.h"
#include "VidExportSettings.h"


namespace merutilm::rff2 {
    struct VideoSettings {
        VidDataSettings data;
        VidAnimationSettings animation;
        VidExportSettings exportation;
    };
}