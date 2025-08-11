//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <algorithm>
#include <vector>
#include <array>

namespace merutilm::rff2 {
    template<typename T>
    class Matrix {
        uint16_t width;
        uint16_t height;
        std::vector<T> canvas;

    public:
        Matrix(const uint16_t width, const uint16_t height) : width(width), height(height),
                                                              canvas(std::vector<T>(width * height)) {
        }

        Matrix(const uint16_t width, const uint16_t height, const std::vector<T> &data) : width(width), height(height),
            canvas(data) {
        }

        const T &operator[](uint32_t i) const {
            return canvas[i];
        }

        T &operator[](uint32_t i) {
            return canvas[i];
        }

        const T &operator()(const uint16_t x, const uint16_t y) const {
            return canvas[getIndex(x, y)];
        }

        T &operator()(const uint16_t x, const uint16_t y) {
            return canvas[getIndex(x, y)];
        }

        uint32_t getIndex(uint16_t x, uint16_t y) const {
            x = std::clamp(x, static_cast<uint16_t>(0), static_cast<uint16_t>(width - 1));
            y = std::clamp(y, static_cast<uint16_t>(0), static_cast<uint16_t>(height - 1));
            return static_cast<uint32_t>(width) * y + x;
        }

        std::array<uint16_t, 2> getLocation(const uint32_t i) const {
            const uint16_t px = i % width;
            const uint16_t py = i / width;
            return {px, py};
        }

        uint16_t getWidth() const {
            return width;
        }

        uint16_t getHeight() const {
            return height;
        }

        uint32_t getLength() const {
            return static_cast<uint32_t>(width) * height;
        }

        const std::vector<T> &getCanvas() const {
            return canvas;
        }

        std::vector<T> getCanvasClone() const {
            return canvas;
        }
    };
}