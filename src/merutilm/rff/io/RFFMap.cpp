//
// Created by Merutilm on 2025-05-08.
//

#include "RFFMap.h"




RFFMap::RFFMap(const double zoom, const uint64_t period, const uint64_t maxIteration,
               const Matrix<double> &iterations) : zoom(zoom), period(period), maxIteration(maxIteration),
                                                iterations(iterations) {
}
RFFMap RFFMap::nullRFFMap() {
    return RFFMap(0, 0, 0, Matrix<double>(0, 0));
};

