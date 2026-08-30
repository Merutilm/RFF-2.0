//
// Created by Merutilm on 7/15/26.
//

#pragma once
#include <atomic>
namespace merutilm::rff2 {
    struct VideoKeyframeProgressInfo {
        std::mutex mutex;
        std::atomic<bool> keyframeGenerating;
        std::atomic<bool> setCurrentframeAsCompleted;
    };
}