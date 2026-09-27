//
// Created by Merutilm on 7/15/26.
//

#pragma once
namespace merutilm::rff2 {
    struct VideoProgressInfo {
        std::mutex mutex;
        double ratio;
        std::string remainedTimeStr;
    };
}