//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <string>
#include <vector>

#include "ShdPalColorSmoothingMethod.h"
#include "FrtDecimalizeIterationMethod.h"
#include "FrtMPACompressionMethod.h"
#include "FrtMPASelectionMethod.h"
#include "FrtReuseReferenceMethod.h"
#include "ShdStripeType.h"


namespace merutilm::rff2 {
    struct Selectable {
        template<typename E> requires std::is_enum_v<E> || std::is_same_v<E, bool>
        static std::vector<E> values() {
            if constexpr (std::is_same_v<E, FrtReuseReferenceMethod>) {
                using enum FrtReuseReferenceMethod;
                return {
                    CURRENT_REFERENCE,
                    CENTERED_REFERENCE,
                    DISABLED
                };
            }
            if constexpr (std::is_same_v<E, FrtDecimalizeIterationMethod>) {
                using enum FrtDecimalizeIterationMethod;
                return {
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
            if constexpr (std::is_same_v<E, FrtMPACompressionMethod>) {
                using enum FrtMPACompressionMethod;
                return {
                    NO_COMPRESSION,
                    LITTLE_COMPRESSION,
                    STRONGEST
                };
            }
            if constexpr (std::is_same_v<E, ShdPalColorSmoothingMethod>) {
                using enum ShdPalColorSmoothingMethod;
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
            if constexpr (std::is_same_v<E, bool>) {
                return {true, false};
            }
            return {};
        }

        template<typename E> requires std::is_enum_v<E> || std::is_same_v<E, bool>
        static std::wstring toString(const E &value) {
            if constexpr (std::is_same_v<E, FrtReuseReferenceMethod>) {
                switch (value) {
                    using enum FrtReuseReferenceMethod;
                    case CURRENT_REFERENCE: return L"Current";
                    case CENTERED_REFERENCE: return L"Centered";
                    case DISABLED: return L"Disabled";
                    default: break;
                }

            }
            if constexpr (std::is_same_v<E, FrtDecimalizeIterationMethod>) {
                switch (value) {
                    using enum FrtDecimalizeIterationMethod;
                    case NONE: return L"None";
                    case LINEAR: return L"Linear";
                    case SQUARE_ROOT: return L"Square root";
                    case LOG: return L"Log";
                    case LOG_LOG: return L"LogLog";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, FrtMPASelectionMethod>) {
                switch (value) {
                    using enum FrtMPASelectionMethod;
                    case LOWEST: return L"Lowest";
                    case HIGHEST: return L"Highest";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, FrtMPACompressionMethod>) {
                switch (value) {
                    using enum FrtMPACompressionMethod;
                    case NO_COMPRESSION: return L"No compression";
                    case LITTLE_COMPRESSION: return L"Little compression";
                    case STRONGEST: return L"Strongest";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, ShdPalColorSmoothingMethod>) {
                switch (value) {
                    using enum ShdPalColorSmoothingMethod;
                    case NONE: return L"None";
                    case NORMAL: return L"Normal";
                    case REVERSED: return L"Reversed";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, ShdStripeType>) {
                switch (value) {
                    using enum ShdStripeType;
                    case NONE: return L"None";
                    case SINGLE_DIRECTION: return L"Single Direction";
                    case SMOOTH: return L"Smooth";
                    case SQUARED: return L"Squared";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, bool>)  {
                return value ? L"O" : L"X";
            }

            return L"Unknown Symbol";
        }
    };
}