//
// Created by Merutilm on 2025-05-08.
//

#pragma once

class PointDouble {
    const double x;
    const double y;
    public:
    explicit PointDouble(double x, double y);
    explicit PointDouble();

    double getX() const;
    double getY() const;
};
