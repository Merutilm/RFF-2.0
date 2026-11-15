//
// Created by Merutilm on 11/15/26.
//

#pragma once
#include <cstdint>
#include <vector>

#include "../calc/iteration_types.hpp"
namespace merutilm::rff2::ArrayUtils {

    static size_t binarySearch(const std::vector<itt::ref> &arr, const itt::ref key) {
        if (arr.empty()) {
            return UINT64_MAX;
        }

        size_t low = 0;
        size_t high = arr.size() - 1;

        while (low <= high) {
            const size_t mid = (low + high) >> 1;
            if (const itt::ref value = arr[mid]; value < key) {
                low = mid + 1;
            } else if (value > key) {
                if (mid == 0) {
                    return UINT64_MAX;
                }
                high = mid - 1;
            } else
                return mid;
        }
        return UINT64_MAX;
    }
}