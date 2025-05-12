//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <functional>
#include <thread>


class ParallelRenderState {
    std::mutex mutex;
    std::jthread thread = std::jthread([](std::stop_token){});

public:
    ParallelRenderState() = default;

    void createThread(std::function<void(std::stop_token)> &&func);

    std::stop_token stopToken() const;

    bool interruptRequested() const;

    void cancel();

    void interrupt();

private:
    void cancelUnsafe();

};
