//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <vector>
#include "../calc/rff_random.hpp"
#include "../constants/FractalConstants.hpp"
#include "ParallelRenderState.h"
namespace merutilm::rff2 {
    template<typename T>
    using ParallelArrayRenderer = std::function<T(uint16_t x, uint16_t y, uint16_t xRes, uint16_t yRes, float xRat,
                                                  float yRat, uint32_t index, T value)>;


    template<typename T>
    class ParallelArrayDispatcher {
        ParallelRenderState &state;
        std::vector<T> &arr;
        uint32_t threads;
        uint16_t xRes;
        uint16_t yRes;
        uint32_t blockSize;
        ParallelArrayRenderer<T> func;

    public:
        ParallelArrayDispatcher(ParallelRenderState &state, std::vector<T> &arr, uint16_t xRes, uint16_t yRes,
                                uint32_t threads, uint32_t blockSize, ParallelArrayRenderer<T> func);

        void dispatch() const;

    private:
        void process(glm::uvec2 startPoint) const;
    };

    // DEFINITION OF PARALLEL ARRAY DISPATCHER


    template<typename T>
    ParallelArrayDispatcher<T>::ParallelArrayDispatcher(ParallelRenderState &state, std::vector<T> &arr,
                                                        const uint16_t xRes, const uint16_t yRes,
                                                        const uint32_t threads,
                                                        const uint32_t blockSize,
                                                        ParallelArrayRenderer<T> func) :
        state(state), arr(arr), threads(threads), xRes(xRes), yRes(yRes), blockSize(blockSize), func(std::move(func)) {}

    template<typename T>
    void ParallelArrayDispatcher<T>::dispatch() const {
        if (state.interruptRequested()) {
            return;
        }

        std::vector<std::jthread> threadPool;
        threadPool.reserve(threads);

        std::vector<glm::uvec2> chunkStartPoints;
        std::atomic<uint32_t> counter;

        for (uint32_t i = 0; i * blockSize < xRes; ++i) {
            for (uint32_t j = 0; j * blockSize < yRes; ++j) {
                chunkStartPoints.emplace_back(i * blockSize, j * blockSize);
            }
        }
        std::ranges::shuffle(chunkStartPoints, rff_random::gen);


        for (uint32_t i = 0; i < threads; ++i) {
            threadPool.emplace_back([this, &counter, &chunkStartPoints] {
                uint32_t c = counter++;
                while (c < chunkStartPoints.size()) {
                    if (state.interruptRequested()) return;
                    const glm::uvec2 startPoint = chunkStartPoints[c];
                    process(startPoint);
                    c = counter++;
                }
            });
        }

        for (auto &t: threadPool) {
            if (t.joinable()) {
                t.join();
            }
        }
    }


    template<typename T>
    void ParallelArrayDispatcher<T>::process(const glm::uvec2 startPoint) const {

        for (uint32_t i = 0; i < blockSize; ++i) {

            auto x = startPoint.x + i;
            if (x >= xRes) break;

            for (uint32_t j = 0; j < blockSize; ++j) {
                auto y = startPoint.y + j;
                if (y >= yRes) break;

                uint32_t index = y * xRes + x;
                arr[index] = func(x, y, xRes, yRes, static_cast<float>(x) / xRes, static_cast<float>(y) / yRes, index,
                                  arr[index]);
            }
        }
    }


} // namespace merutilm::rff2
