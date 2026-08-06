//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <string>
#include <vector>

#include "FrtDecimalizeIterationMethod.h"
#include "FrtMPASelectionMethod.h"
#include "FrtReferenceReuseMethod.h"
#include "ShdIterationColoringMethod.hpp"
#include "ShdPalSingleIterationColoringMethod.h"
#include "ShdStripeType.h"


namespace merutilm::rff2 {
    struct Selectable {
        template<typename E> requires std::is_enum_v<E>
        static std::vector<E> values() {
            if constexpr (std::is_same_v<E, FrtReferenceReuseMethod>) {
                using enum FrtReferenceReuseMethod;
                return {
                    CURRENT_REFERENCE,
                    CENTERED_REFERENCE,
                    DISABLED
                };
            }
            if constexpr (std::is_same_v<E, FrtDecimalizeIterationMethod>) {
                using enum FrtDecimalizeIterationMethod;
                return {
                    NONE,
                    LINEAR,
                    SQUARE_ROOT,
                    LOG,
                    LOG_LOG
                };
            }
            if constexpr (std::is_same_v<E, FrtMPASelectionMethod>) {
                using enum FrtMPASelectionMethod;
                return {
                    LOWEST,
                    HIGHEST
                };
            }
            if constexpr (std::is_same_v<E, ShdIterationColoringMethod>) {
                using enum ShdIterationColoringMethod;
                return {
                    LINEAR,
                    SQUARE_ROOT,
                    LOG
                };
            }
            if constexpr (std::is_same_v<E, ShdPalSingleIterationColoringMethod>) {
                using enum ShdPalSingleIterationColoringMethod;
                return {
                    NONE,
                    NORMAL,
                    REVERSED
                };
            }
            if constexpr (std::is_same_v<E, ShdStripeType>) {
                using enum ShdStripeType;
                return {
                    NONE,
                    SINGLE_DIRECTION,
                    SMOOTH,
                    SQUARED
                };
            }
            return {};
        }

        template<typename E> requires std::is_enum_v<E>
        static const char * toString(const E &value) {
            if constexpr (std::is_same_v<E, FrtReferenceReuseMethod>) {
                switch (value) {
                    using enum FrtReferenceReuseMethod;
                    case CURRENT_REFERENCE: return "Current";
                    case CENTERED_REFERENCE: return "Centered";
                    case DISABLED: return "Disabled";
                    default: break;
                }

            }
            if constexpr (std::is_same_v<E, FrtDecimalizeIterationMethod>) {
                switch (value) {
                    using enum FrtDecimalizeIterationMethod;
                    case NONE: return "None";
                    case LINEAR: return "Linear";
                    case SQUARE_ROOT: return "Square root";
                    case LOG: return "Log";
                    case LOG_LOG: return "LogLog";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, FrtMPASelectionMethod>) {
                switch (value) {
                    using enum FrtMPASelectionMethod;
                    case LOWEST: return "Lowest";
                    case HIGHEST: return "Highest";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, ShdIterationColoringMethod>) {
                switch (value) {
                    using enum ShdIterationColoringMethod;
                    case LINEAR: return "Linear";
                    case SQUARE_ROOT: return "Square root";
                    case LOG: return "Log";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, ShdPalSingleIterationColoringMethod>) {
                switch (value) {
                    using enum ShdPalSingleIterationColoringMethod;
                    case NONE: return "None";
                    case NORMAL: return "Normal";
                    case REVERSED: return "Reversed";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, ShdStripeType>) {
                switch (value) {
                    using enum ShdStripeType;
                    case NONE: return "None";
                    case SINGLE_DIRECTION: return "Single Direction";
                    case SMOOTH: return "Smooth";
                    case SQUARED: return "Squared";
                    default: break;
                }
            }

            return "Unknown Symbol";
        }
    };
}