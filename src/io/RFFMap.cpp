//
// Created by Merutilm on 2025-06-23.
//

#include "RFFMap.h"


namespace merutilm::rff {
    RFFMap::RFFMap(const float logZoom) : logZoom(logZoom) {
    }

    float RFFMap::getLogZoom() const {
        return logZoom;
    }

}
