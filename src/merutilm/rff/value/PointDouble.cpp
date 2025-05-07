//
// Created by Merutilm on 2025-05-08.
//

#include "PointDouble.h"


PointDouble::PointDouble() : PointDouble(0, 0){}

PointDouble::PointDouble(const int x, const int y) : x(x), y(y) {}

int PointDouble::getX() const {
    return x;
}

int PointDouble::getY() const {
    return y;
}
