//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include "ParallelRenderState.h"
#include "../constants/Constants.hpp"
namespace merutilm::rff2 {
    using ParallelRenderer = std::function<void(uint32_t x, uint32_t y, uint32_t xRes, uint32_t yRes, float xRat,
                                                float yRat, uint32_t index)>;

    class ParallelDispatcher {
        ParallelRenderState &state;
        ParallelRenderer renderer;
        uint32_t xRes;
        uint32_t yRes;
        uint32_t threads;

    public:
        ParallelDispatcher(ParallelRenderState &state, uint32_t xRes, uint32_t yRes, uint32_t threads, ParallelRenderer renderer);


        void dispatch() const;

    private:
        static std::vector<uint32_t> getRenderPriority(uint32_t rpy);


        void renderForward(uint32_t xRes, uint32_t yRes, uint32_t y, std::vector<std::atomic<bool> > &rendered) const;


        void renderBackward(uint32_t xRes, uint32_t yRes, uint32_t len, std::vector<std::atomic<bool> > &rendered) const;
    };

    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
    // DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER


    inline ParallelDispatcher::ParallelDispatcher(ParallelRenderState &state, const uint32_t xRes, const uint32_t yRes, const uint32_t threads,
                                                  ParallelRenderer renderer) : state(state), renderer(std::move(renderer)),
                                                                               xRes(xRes), yRes(yRes), threads(threads) {
    }

    inline void ParallelDispatcher::dispatch() const {
        const uint32_t rpy = yRes / threads + 1;
        if (state.interruptRequested()) {
            return;
        }


        const std::vector<uint32_t> rpyIndices = getRenderPriority(rpy);
        auto threadPool = std::vector<std::jthread>();
        threadPool.reserve(threads);
        auto len = xRes * yRes;
        auto rendered = std::vector<std::atomic<bool> >(len);

        for (uint32_t sy = 0; sy < yRes; sy += rpy) {
            threadPool.emplace_back([sy, &rpyIndices, this, &rendered, len] {
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


    inline std::vector<uint32_t> ParallelDispatcher::getRenderPriority(const uint32_t rpy) {
        auto result = std::vector<uint32_t>(rpy, 0);
        uint32_t count = rpy >> 1;
        uint32_t repetition = 1;
        uint32_t index = 1;

        while (count > 0) {
            for (uint32_t j = 0; j < repetition; ++j) {
                result[index] = result[j] + count;
                ++index;
            }

            repetition <<= 1;
            count >>= 1;
        }

        auto cpy = result;
        cpy.resize(index);
        std::ranges::sort(cpy);

        uint32_t cpyIndex = 0;
        while (index < result.size()) {
            if (
                const uint32_t missing = cpyIndex + count;
                cpy.size() <= cpyIndex || cpy[cpyIndex] != missing) {
                result[index] = missing;
                ++index;
                ++count;
                } else ++cpyIndex;
        }
        return result;
    }


    inline void ParallelDispatcher::renderForward(const uint32_t xRes, const uint32_t yRes, const uint32_t y,
                                                  std::vector<std::atomic<bool> > &rendered) const {
        if (y >= yRes) {
            return;
        }

        for (uint32_t x = 0; x < xRes; ++x) {
            if (x % Constants::Fractal::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return;
            }

            if (const uint32_t i = static_cast<uint32_t>(xRes) * y + x;
                !rendered[i].exchange(true)
                ) {
                renderer(x, y, xRes, yRes, static_cast<float>(x) / xRes,
                         static_cast<float>(y) / yRes, i);
                }
        }
    }


    inline void ParallelDispatcher::renderBackward(const uint32_t xRes, const uint32_t yRes, const uint32_t len,
                                                   std::vector<std::atomic<bool> > &rendered) const {
        for (uint32_t i = len - 1; i > 0; --i) {
            if (i % Constants::Fractal::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return;
            }
            const uint32_t px = i % xRes;
            const uint32_t py = i / xRes;

            if (!rendered[i].exchange(true)) {
                renderer(px, py, xRes, yRes, static_cast<float>(px) / xRes, static_cast<float>(py) / yRes, i);
            }
        }
    }
}