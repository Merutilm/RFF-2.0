//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <vector>
#include "../constants/FractalConstants.hpp"
#include "ParallelRenderState.h"
namespace merutilm::rff2 {
    template<typename T>
    using ParallelArrayRenderer = std::function<T(uint16_t x, uint16_t y, uint16_t xRes, uint16_t yRes, float xRat, float yRat, uint32_t index,
                                                  T value)>;


    template<typename T>
    class ParallelArrayDispatcher {
        ParallelRenderState &state;
        std::vector<T> &arr;
        ParallelArrayRenderer<T> renderer;
        uint32_t threads;
        uint16_t xRes;
        uint16_t yRes;

    public:
        ParallelArrayDispatcher(ParallelRenderState &state, std::vector<T> &arr, uint16_t xRes, uint16_t yRes, uint32_t threads,
                                ParallelArrayRenderer<T> renderer);


        void dispatch();

    private:
        static std::vector<uint32_t> getRenderPriority(uint32_t count);


        void renderForward(uint32_t start, std::vector<uint32_t> &indexOff, uint32_t batchSize,
                           std::vector<std::atomic<bool>> &rendered);


        void renderBackward(uint32_t len, std::vector<std::atomic<bool> > &rendered);
    };

    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER


    template<typename T>
    ParallelArrayDispatcher<T>::ParallelArrayDispatcher(ParallelRenderState &state, std::vector<T> &arr, const uint16_t xRes, const uint16_t yRes, const uint32_t threads,
                                                        ParallelArrayRenderer<T> renderer) : state(state), arr(arr), renderer(std::move(renderer)), threads(threads),
        xRes(xRes), yRes(yRes) {
    }

    template<typename T>
    void ParallelArrayDispatcher<T>::dispatch() {
        if (state.interruptRequested()) {
            return;
        }

        auto threadPool = std::vector<std::jthread>();
        threadPool.reserve(threads);
        auto len = arr.size();
        auto rendered = std::vector<std::atomic<bool> >(len);
        auto batchSize = len / threads + 1;

        std::vector<uint32_t> indexOff = getRenderPriority(batchSize);

        for (uint32_t start = 0; start < len; start += batchSize) {
            threadPool.emplace_back([start, batchSize, &indexOff, this, &rendered, len] {
                renderForward(start, indexOff, batchSize, rendered);
                renderBackward(len, rendered);
            });
        }


        for (auto &t: threadPool) {
            if (t.joinable()) {
                t.join();
            }
        }
    }


    template<typename T>
    std::vector<uint32_t> ParallelArrayDispatcher<T>::getRenderPriority(const uint32_t count) {
        auto result = std::vector<uint32_t>(count, 0);
        uint32_t countDiv = count >> 1;
        uint32_t repetition = 1;
        uint32_t index = 1;

        while (countDiv > 0) {
            for (uint16_t j = 0; j < repetition; ++j) {
                result[index] = result[j] + countDiv;
                ++index;
            }

            repetition <<= 1;
            countDiv >>= 1;
        }

        auto cpy = result;
        cpy.resize(index);
        std::ranges::sort(cpy);

        uint16_t ci = 0;
        while (index < result.size()) {
            if (
                const uint16_t missing = ci + countDiv;
                cpy.size() <= ci || cpy[ci] != missing) {
                result[index] = missing;
                ++index;
                ++countDiv;
                } else ++ci;
        }
        return result;
    }


    template<typename T>
    void ParallelArrayDispatcher<T>::renderForward(const uint32_t start, std::vector<uint32_t> &indexOff, const uint32_t batchSize,
                                                   std::vector<std::atomic<bool> > &rendered) {
        if (start >= rendered.size()) {
            return;
        }



        for (const uint32_t i : indexOff) {
            if (i % Constants::Fractal::PARALLEL_OPERATION_INTERRUPT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return;
            }

            uint32_t index = start + i;
            if (index >= arr.size()) continue;

            auto x = static_cast<uint16_t>(index % xRes);
            auto y = static_cast<uint16_t>(index / xRes);

            if (!rendered[index].exchange(true)) {
                arr[index] = renderer(x, y, xRes, yRes, static_cast<float>(x) / xRes,
                                     static_cast<float>(y) / yRes, index, arr[index]);
            }
        }
    }


    template<typename T>
    void ParallelArrayDispatcher<T>::renderBackward(const uint32_t len,
                                                    std::vector<std::atomic<bool> > &rendered) {
        for (uint32_t i = len - 1; i > 0; --i) {
            if (i % Constants::Fractal::PARALLEL_OPERATION_INTERRUPT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return;
            }
            auto px = static_cast<uint16_t>(i % xRes);
            auto py = static_cast<uint16_t>(i / xRes);

            if (!rendered[i].exchange(true)) {
                T c = renderer(px, py, xRes, yRes, static_cast<float>(px) / xRes, static_cast<float>(py) / yRes, i,
                                    arr[i]);
                arr[i] = std::move(c);
            }
        }
    }
}