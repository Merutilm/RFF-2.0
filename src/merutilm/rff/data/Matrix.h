//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <algorithm>
#include <vector>
#include <array>

template<typename T>
struct Matrix {
    const int width;
    const int height;
    std::vector<T> canvas;

    Matrix(const int width, const int height) : width(width), height(height), canvas(std::vector<T>(width * height)) {
    }

    Matrix(const int width, const int height, const std::vector<T> &data) : width(width), height(height), canvas(data) {
    }

    const T &operator[](int i) const {
        return canvas[i];
    }

    T &operator[](int i) {
        return canvas[i];
    }

    const T &operator()(const int x, const int y) const {
        return canvas[getIndex(x, y)];
    }

    T &operator()(const int x, const int y) {
        return canvas[getIndex(x, y)];
    }

    int getIndex(int x, int y) const {
        x = std::clamp(x, 0, width - 1);
        y = std::clamp(y, 0, height - 1);
        return width * y + x;
    }

    std::array<int, 2> getLocation(const int i) const {
        const int px = i % width;
        const int py = i / width;
        return {px, py};
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    int getLength() const {
        return width * height;
    }

    std::vector<T> &getCanvas() {
        return canvas;
    }

    std::vector<T> getCanvasClone() const {
        return canvas;
    }
};
