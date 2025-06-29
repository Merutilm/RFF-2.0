//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include "AnimationSettings.h"
#include "DataSettings.h"
#include "ExportSettings.h"


namespace merutilm::rff2 {
    struct VideoSettings {
        DataSettings dataSettings;
        AnimationSettings animationSettings;
        ExportSettings exportSettings;
    };
}