//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include "VidAnimationAttribute.h"
#include "VidDataAttribute.h"
#include "VidExportAttribute.h"


namespace merutilm::rff2 {
    struct VideoAttribute {
        VidDataAttribute data;
        VidAnimationAttribute animation;
        VidExportAttribute exportation;
    };
}