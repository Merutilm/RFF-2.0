//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "dex.h"


namespace merutilm::rff2 {
    struct dex_std {
        explicit dex_std() = delete;

        static dex max(dex a, dex b);

        static dex min(dex a, dex b);

        static dex clamp(dex target, dex mn, dex mx);

        static dex abs(dex v);
    };


    inline dex dex_std::max(const dex a, const dex b) {
        const auto result = a > b ? a : b;
        return result;
    }

    inline dex dex_std::min(const dex a, const dex b) {
        const auto result = a < b ? a : b;
        return result;
    }

    inline dex dex_std::clamp(const dex target, const dex mn, const dex mx) {
        return max(min(target, mx), mn);
    }


    inline dex dex_std::abs(const dex v) {
        return v.sgn() == -1 ? -v : v;
    }

} // namespace merutilm::rff2
