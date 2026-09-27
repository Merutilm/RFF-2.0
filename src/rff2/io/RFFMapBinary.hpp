//
// Created by Merutilm on 9/27/26.
//
#pragma once
#include "RFFBinary.hpp"
namespace merutilm::rff2 {

    struct RFFMapBinary : RFFBinary {
        double logZoom;

    protected:
        explicit RFFMapBinary(const double logZoom) : logZoom(logZoom) {
            //noop
        }
    public:
        [[nodiscard]] bool hasData() const {
            return logZoom != 0;
        }
    };
}