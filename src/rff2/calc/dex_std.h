//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "dex.h"


namespace merutilm::rff2 {
    struct dex_std {
        explicit dex_std() = delete;

        static void max(dex *result, const dex &a, const dex &b, dex *temp);

        static void max(dex *result, const dex &a, const dex &b);

        static dex max(const dex &a, const dex &b);

        static void min(dex *result, const dex &a, const dex &b, dex *temp);

        static void min(dex *result, const dex &a, const dex &b);

        static dex min(const dex &a, const dex &b);

        static void clamp(dex *result, const dex &target, const dex &mn, const dex &mx);

        static dex clamp(const dex &target, const dex &mn, const dex &mx);

        static void abs(dex *result, const dex &v);

        static dex abs(const dex &v);
    };

    inline void dex_std::max(dex *result, const dex &a, const dex &b, dex *temp) {
        dex::sub(temp, a, b);
        if (temp->sgn() == 1) {
            dex::cpy(result, a);
        } else {
            dex::cpy(result, b);
        }
    }


    inline void dex_std::max(dex *result, const dex &a, const dex &b) {
        dex::cpy(result, a > b ? a : b);
    }

    inline dex dex_std::max(const dex &a, const dex &b) {
        dex result = dex::ZERO;
        max(&result, a, b);
        return result;
    }

    inline void dex_std::min(dex *result, const dex &a, const dex &b, dex *temp) {
        dex::sub(temp, a, b);
        if (temp->sgn() == 1) {
            dex::cpy(result, b);
        } else {
            dex::cpy(result, a);
        }
    }

    inline void dex_std::min(dex *result, const dex &a, const dex &b) {
        dex::cpy(result, a < b ? a : b);
    }

    inline dex dex_std::min(const dex &a, const dex &b) {
        dex result = dex::ZERO;
        min(&result, a, b);
        return result;
    }


    inline void dex_std::clamp(dex *result, const dex &target, const dex &mn,
                               const dex &mx) {
        min(result, target, mx);
        max(result, mn, target);
    }

    inline dex dex_std::clamp(const dex &target, const dex &mn, const dex &mx) {
        dex result = dex::ZERO;
        clamp(&result, target, mn, mx);
        return result;
    }


    inline void dex_std::abs(dex *result, const dex &v) {
        dex::cpy(result, v < 0 ? -v : v);
    }

    inline dex dex_std::abs(const dex &v) {
        dex result = dex::ZERO;
        abs(&result, v);
        return result;
    }
}
