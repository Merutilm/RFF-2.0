//
// Created by Merutilm on 2025-05-08.
//

#include "PointDouble.h"


PointDouble::PointDouble() : PointDouble(0, 0){}

PointDouble::PointDouble(const double x, const double y) : x(x), y(y) {}

double PointDouble::getX() const {
    return x;
}

double PointDouble::getY() const {
    return y;
}
