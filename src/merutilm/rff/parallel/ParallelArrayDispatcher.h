//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include "ParallelRenderState.h"
#include "../data/Matrix.h"

template<typename T>
using ParallelArrayRenderer = std::function<T(int x, int y, int xRes, int yRes, float xRat, float yRat, int index,
                                              T value)>;

template<typename T>
class ParallelArrayDispatcher {
    ParallelRenderState &state;
    Matrix<T> &matrix;
    ParallelArrayRenderer<T> renderer;

public:
    ParallelArrayDispatcher(ParallelRenderState &state, Matrix<T> &matrix,
                            ParallelArrayRenderer<T> renderer) : state(state), matrix(matrix),
                                                                 renderer(std::move(renderer)) {
    }


    void dispatch() {
        const auto cores = std::thread::hardware_concurrency();
        const int rpy = matrix.getHeight() / cores + 1;
        if (state.interruptRequested()) {
            return;
        }


        const std::vector<int> rpyIndices = getRenderPriority(rpy);
        auto threadPool = std::vector<std::jthread>();
        threadPool.reserve(cores);
        auto xRes = matrix.getWidth();
        auto yRes = matrix.getHeight();
        auto len = matrix.getLength();
        auto rendered = std::vector<std::atomic<bool> >(matrix.getLength());
        auto values = std::vector<int>(len, xRes * yRes);
        for (int i = 0; i < values.size(); ++i) {
            values[i] = i;
        }



        for (int sy = 0; sy < matrix.getHeight(); sy += rpy) {
            threadPool.emplace_back([sy, &rpyIndices, xRes, yRes, this, &rendered, len] {
                for (const auto vy: rpyIndices) {
                    renderForward(xRes, yRes, sy + vy, rendered);
                }
                renderBackward(xRes, yRes, len, rendered);
            });
        }


        for (auto &t: threadPool) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

private:
    static std::vector<int> getRenderPriority(const int rpy) {
        auto result = std::vector(rpy, 0);
        int count = rpy >> 1;
        int repetition = 1;
        int index = 1;

        while (count > 0) {
            for (int j = 0; j < repetition; ++j) {
                result[index] = result[j] + count;
                ++index;
            }

            repetition <<= 1;
            count >>= 1;
        }

        auto cpy = result;
        cpy.resize(index);
        std::ranges::sort(cpy);

        int cpyIndex = 0;
        while (index < result.size()) {
            if (
                const int missing = cpyIndex + count;
                cpy.size() <= cpyIndex || cpy[cpyIndex] != missing) {
                result[index] = missing;
                ++index;
                ++count;
            } else ++cpyIndex;
        }
        return result;
    }


    void renderForward(const int xRes, const int yRes, const int y, std::vector<std::atomic<bool>> &rendered) {
        if (y >= yRes) {
            return;
        }

        for (int x = 0; x < xRes; ++x) {
            if (x % RFF::Render::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return;
            }

            int i = xRes * y + x;

            if (!rendered[i].exchange(true)) {
                matrix[i] = renderer(x, y, xRes, yRes, static_cast<float>(x) / xRes,
                                     static_cast<float>(y) / yRes, i, matrix[i]);
            }
        }
    }


    void renderBackward(const int xRes, const int yRes, const int len, std::vector<std::atomic<bool> > &rendered) {
        for (int i = len - 1; i >= 0; --i) {
            if (i % RFF::Render::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return;
            }
            const auto [px, py] = matrix.getLocation(i);

            if (!rendered[i].exchange(true)) {
                double c = renderer(px, py, xRes, yRes, static_cast<float>(px) / xRes, static_cast<float>(py) / yRes, i,
                                    matrix[i]);
                matrix[i] = c;
            }
        }
    }
};
