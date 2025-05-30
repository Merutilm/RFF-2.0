//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include "ParallelRenderState.h"
#include "../data/Matrix.h"
#include "../ui/RFF.h"

using ParallelRenderer = std::function<void(int x, int y, int xRes, int yRes, float xRat, float yRat, int index)>;

class ParallelDispatcher {
    ParallelRenderState &state;
    ParallelRenderer renderer;
    int xRes;
    int yRes;

public:
    ParallelDispatcher(ParallelRenderState &state, int xRes, int yRes, ParallelRenderer renderer);


    void dispatch();

private:
    static std::vector<int> getRenderPriority(int rpy);


    void renderForward(int xRes, int yRes, int y, std::vector<std::atomic<bool> > &rendered) const;


    void renderBackward(int xRes, int yRes, int len, std::vector<std::atomic<bool> > &rendered) const;
};

// DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
// DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
// DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
// DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER
// DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER  DEFINITION OF PARALLEL ARRAY DISPATCHER


inline ParallelDispatcher::ParallelDispatcher(ParallelRenderState &state, const int xRes, const int yRes, ParallelRenderer renderer) : state(state), renderer(std::move(renderer)), xRes(xRes), yRes(yRes) {
        
}

inline void ParallelDispatcher::dispatch() {
    const auto cores = std::thread::hardware_concurrency();
    const int rpy = yRes / cores + 1;
    if (state.interruptRequested()) {
        return;
    }


    const std::vector<int> rpyIndices = getRenderPriority(rpy);
    auto threadPool = std::vector<std::jthread>();
    threadPool.reserve(cores);
    auto len = xRes * yRes;
    auto rendered = std::vector<std::atomic<bool> >(len);

    for (int sy = 0; sy < yRes; sy += rpy) {
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


inline std::vector<int> ParallelDispatcher::getRenderPriority(const int rpy) {
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


inline void ParallelDispatcher::renderForward(const int xRes, const int yRes, const int y,
                                                   std::vector<std::atomic<bool> > &rendered) const {
    if (y >= yRes) {
        return;
    }

    for (int x = 0; x < xRes; ++x) {
        if (x % RFF::Render::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
            return;
        }

        int i = xRes * y + x;

        if (!rendered[i].exchange(true)) {
            renderer(x, y, xRes, yRes, static_cast<float>(x) / xRes,
                                 static_cast<float>(y) / yRes, i);
        }
    }
}


inline void ParallelDispatcher::renderBackward(const int xRes, const int yRes, const int len,
                                                std::vector<std::atomic<bool> > &rendered) const {
    for (int i = len - 1; i >= 0; --i) {
        if (i % RFF::Render::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
            return;
        }
        const int px = i % xRes;
        const int py = i / xRes;

        if (!rendered[i].exchange(true)) {
            renderer(px, py, xRes, yRes, static_cast<float>(px) / xRes, static_cast<float>(py) / yRes, i);
        }
    }
}
