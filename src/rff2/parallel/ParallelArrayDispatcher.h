//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include "ParallelRenderState.h"
#include "../data/Matrix.h"
namespace merutilm::rff2 {
    template<typename T>
    using ParallelArrayRenderer = std::function<T(uint16_t x, uint16_t y, uint16_t xRes, uint16_t yRes, float xRat, float yRat, uint32_t index,
                                                  T value)>;


    template<typename T>
    class ParallelArrayDispatcher {
        ParallelRenderState &state;
        Matrix<T> &matrix;
        ParallelArrayRenderer<T> renderer;
        uint32_t threads;

    public:
        ParallelArrayDispatcher(ParallelRenderState &state, Matrix<T> &matrix, uint32_t threads,
                                ParallelArrayRenderer<T> renderer);


        void dispatch();

    private:
        static std::vector<uint16_t> getRenderPriority(uint16_t rpy);


        void renderForward(uint16_t xRes, uint16_t yRes, uint16_t y, std::vector<std::atomic<bool> > &rendered);


        void renderBackward(uint16_t xRes, uint16_t yRes, uint32_t len, std::vector<std::atomic<bool> > &rendered);
    };

    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER


    template<typename T>
    ParallelArrayDispatcher<T>::ParallelArrayDispatcher(ParallelRenderState &state, Matrix<T> &matrix, const uint32_t threads,
                                                        ParallelArrayRenderer<T> renderer) : state(state), matrix(matrix),
        renderer(std::move(renderer)), threads(threads) {
    }

    template<typename T>
    void ParallelArrayDispatcher<T>::dispatch() {
        const uint16_t rpy = matrix.getHeight() / threads + 1;
        if (state.interruptRequested()) {
            return;
        }


        const std::vector<uint16_t> rpyIndices = getRenderPriority(rpy);
        auto threadPool = std::vector<std::jthread>();
        threadPool.reserve(threads);
        auto xRes = matrix.getWidth();
        auto yRes = matrix.getHeight();
        auto len = matrix.getLength();
        auto rendered = std::vector<std::atomic<bool> >(len);

        for (uint16_t sy = 0; sy < matrix.getHeight(); sy += rpy) {
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


    template<typename T>
    std::vector<uint16_t> ParallelArrayDispatcher<T>::getRenderPriority(const uint16_t rpy) {
        auto result = std::vector<uint16_t>(rpy, 0);
        uint16_t count = rpy >> 1;
        uint16_t repetition = 1;
        uint16_t index = 1;

        while (count > 0) {
            for (uint16_t j = 0; j < repetition; ++j) {
                result[index] = result[j] + count;
                ++index;
            }

            repetition <<= 1;
            count >>= 1;
        }

        auto cpy = result;
        cpy.resize(index);
        std::ranges::sort(cpy);

        uint16_t cpyIndex = 0;
        while (index < result.size()) {
            if (
                const uint16_t missing = cpyIndex + count;
                cpy.size() <= cpyIndex || cpy[cpyIndex] != missing) {
                result[index] = missing;
                ++index;
                ++count;
                } else ++cpyIndex;
        }
        return result;
    }


    template<typename T>
    void ParallelArrayDispatcher<T>::renderForward(const uint16_t xRes, const uint16_t yRes, const uint16_t y,
                                                   std::vector<std::atomic<bool> > &rendered) {
        if (y >= yRes) {
            return;
        }

        for (uint16_t x = 0; x < xRes; ++x) {
            if (x % Constants::Fractal::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return;
            }

            uint32_t i = static_cast<uint32_t>(xRes) * y + x;

            if (!rendered[i].exchange(true)) {
                matrix[i] = renderer(x, y, xRes, yRes, static_cast<float>(x) / xRes,
                                     static_cast<float>(y) / yRes, i, matrix[i]);
            }
        }
    }


    template<typename T>
    void ParallelArrayDispatcher<T>::renderBackward(const uint16_t xRes, const uint16_t yRes, const uint32_t len,
                                                    std::vector<std::atomic<bool> > &rendered) {
        for (uint32_t i = len - 1; i > 0; --i) {
            if (i % Constants::Fractal::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return;
            }
            const auto [px, py] = matrix.getLocation(i);

            if (!rendered[i].exchange(true)) {
                T c = renderer(px, py, xRes, yRes, static_cast<float>(px) / xRes, static_cast<float>(py) / yRes, i,
                                    matrix[i]);
                matrix[i] = std::move(c);
            }
        }
    }
}