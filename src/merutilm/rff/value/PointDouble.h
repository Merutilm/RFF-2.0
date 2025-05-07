//
// Created by Merutilm on 2025-05-08.
//

#pragma once

class PointDouble {
    const int x;
    const int y;
    public:
    PointDouble(int x, int y);
    PointDouble();

    int getX() const;
    int getY() const;
};
