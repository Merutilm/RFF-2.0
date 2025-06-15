//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <string>
#include <vector>

#include "ColorSmoothingSettings.h"
#include "DecimalizeIterationMethod.h"
#include "MPACompressionMethod.h"
#include "MPASelectionMethod.h"
#include "ReuseReferenceMethod.h"
#include "StripeType.h"


namespace merutilm::rff {
    struct Selectable {
        template<typename E> requires std::is_enum_v<E> || std::is_same_v<E, bool>
        static std::vector<E> values() {
            if constexpr (std::is_same_v<E, ReuseReferenceMethod>) {
                using enum ReuseReferenceMethod;
                return {
                    CURRENT_REFERENCE,
                    CENTERED_REFERENCE,
                    DISABLED
                };
            }
            if constexpr (std::is_same_v<E, DecimalizeIterationMethod>) {
                using enum DecimalizeIterationMethod;
                return {
                    LINEAR,
                    SQUARE_ROOT,
                    LOG,
                    LOG_LOG
                };
            }
            if constexpr (std::is_same_v<E, MPASelectionMethod>) {
                using enum MPASelectionMethod;
                return {
                    LOWEST,
                    HIGHEST
                };
            }
            if constexpr (std::is_same_v<E, MPACompressionMethod>) {
                using enum MPACompressionMethod;
                return {
                    NO_COMPRESSION,
                    LITTLE_COMPRESSION,
                    STRONGEST
                };
            }
            if constexpr (std::is_same_v<E, ColorSmoothingSettings>) {
                using enum ColorSmoothingSettings;
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
            if constexpr (std::is_same_v<E, ReuseReferenceMethod>) {
                switch (value) {
                    using enum ReuseReferenceMethod;
                    case CURRENT_REFERENCE: return "Current";
                    case CENTERED_REFERENCE: return "Centered";
                    case DISABLED: return "Disabled";
                    default: break;
                }

            }
            if constexpr (std::is_same_v<E, DecimalizeIterationMethod>) {
                switch (value) {
                    using enum DecimalizeIterationMethod;
                    case NONE: return "None";
                    case LINEAR: return "Linear";
                    case SQUARE_ROOT: return "Square root";
                    case LOG: return "Log";
                    case LOG_LOG: return "LogLog";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, MPASelectionMethod>) {
                switch (value) {
                    using enum MPASelectionMethod;
                    case LOWEST: return "Lowest";
                    case HIGHEST: return "Highest";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, MPACompressionMethod>) {
                switch (value) {
                    using enum MPACompressionMethod;
                    case NO_COMPRESSION: return "No compression";
                    case LITTLE_COMPRESSION: return "Little compression";
                    case STRONGEST: return "Strongest";
                    default: break;
                }
            }
            if constexpr (std::is_same_v<E, ColorSmoothingSettings>) {
                switch (value) {
                    using enum ColorSmoothingSettings;
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