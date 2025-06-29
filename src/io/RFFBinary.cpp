//
// Created by Merutilm on 2025-06-23.
//

#include "RFFBinary.h"


namespace merutilm::rff2 {
    RFFBinary::RFFBinary(const float logZoom) : logZoom(logZoom) {
    }

    float RFFBinary::getLogZoom() const {
        return logZoom;
    }

}
