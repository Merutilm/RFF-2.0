//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <string>
#include <vector>

#include "ShdPalColorSmoothingMethod.h"
#include "CalDecimalizeIterationMethod.h"
#include "CalMPACompressionMethod.h"
#include "CalMPASelectionMethod.h"
#include "CalReuseReferenceMethod.h"
#include "StripeType.h"


namespace merutilm::rff2 {
    struct Selectable {
        template<typename E> requires std::is_enum_v<E> || std::is_same_v<E, bool>
        static std::vector<E> values() {
            if constexpr (std::is_same_v<E, CalReuseReferenceMethod>) {
                using enum CalReuseReferenceMethod;
                return {
                    CURRENT_REFERENCE,
                    CENTERED_REFERENCE,
                    DISABLED
                };
            }
            if constexpr (std::is_same_v<E, CalDecimalizeIterationMethod>) {
                using enum CalDecimalizeIterationMethod;
                return {
                    LINEAR,
                    SQUARE_ROOT,
                    LOG,
                    LOG_LOG
                };
            }
            if constexpr (std::is_same_v<E, CalMPASelectionMethod>) {
                using enum CalMPASelectionMethod;
                return {
                    LOWEST,
                    HIGHEST
                };
            }
            if constexpr (std::is_same_v<E, CalMPACompressionMethod>) {
                using enum CalMPACompressionMethod;
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
            if constexpr (std::is_same_v<E, StripeType>) {
                using enum StripeType;
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
        static std::string toString(const E &value) {
            if constexpr (std::is_same_v<E, CalReuseReferenceMethod>) {
                switch (value) {
                    using enum CalReuseReferenceMethod;
                    case CURRENT_REFERENCE: return "Current";
                    case CENTERED_REFERENCE: return "Centered";
                    case DISABLED: return "Disabled";
                    default: break;
                }

            }
            if constexpr (std::is_same_v<E, CalDecimalizeIterationMethod>) {
                switch (value) {
                    using enum CalDecimalizeIterationMethod;
                    case NONE: return "None";
                    case LINEAR: return "Linear";
                    case SQUARE_ROOT: return "Square root";
                    case LOG: return "Log";
                    case LOG_LOG: return "LogLog";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, CalMPASelectionMethod>) {
                switch (value) {
                    using enum CalMPASelectionMethod;
                    case LOWEST: return "Lowest";
                    case HIGHEST: return "Highest";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, CalMPACompressionMethod>) {
                switch (value) {
                    using enum CalMPACompressionMethod;
                    case NO_COMPRESSION: return "No compression";
                    case LITTLE_COMPRESSION: return "Little compression";
                    case STRONGEST: return "Strongest";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, ShdPalColorSmoothingMethod>) {
                switch (value) {
                    using enum ShdPalColorSmoothingMethod;
                    case NONE: return "None";
                    case NORMAL: return "Normal";
                    case REVERSED: return "Reversed";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, StripeType>) {
                switch (value) {
                    using enum StripeType;
                    case NONE: return "None";
                    case SINGLE_DIRECTION: return "Single Direction";
                    case SMOOTH: return "Smooth";
                    case SQUARED: return "Squared";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, bool>)  {
                return value ? "O" : "X";
            }

            return "Unknown Symbol";
        }
    };
}