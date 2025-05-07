//
// Created by Merutilm on 2025-05-04.
//

#include "SelectableParser.h"

#include "ReuseReferenceMethod.h"
#include "../ui/RFFConstants.h"


std::string SelectableParser::parse(const DecimalizeIterationMethod &value) {
    switch (value) {
        using enum DecimalizeIterationMethod;
        case NONE: return "None";
        case LINEAR: return "Linear";
        case SQUARE_ROOT: return "Square root";
        case LOG: return "Log";
        case LOG_LOG: return "LogLog";
        default: return RFFConstants::Parser::UNKNOWN;
    }
}

std::string SelectableParser::parse(const ReuseReferenceMethod &value) {
    switch (value) {
        using enum ReuseReferenceMethod;
        case CURRENT_REFERENCE: return "Current";
        case CENTERED_REFERENCE: return "Centered";
        case DISABLED: return "Disabled";
        default: return RFFConstants::Parser::UNKNOWN;
    }

}

std::string SelectableParser::parse(const MPASelectionMethod &value) {
    switch (value) {
        using enum MPASelectionMethod;
        case LOWEST : return "Lowest";
        case HIGHEST : return "Highest";
        default: return RFFConstants::Parser::UNKNOWN;
    }
}

std::string SelectableParser::parse(const MPACompressionMethod &value) {
    switch (value) {
        using enum MPACompressionMethod;
        case NO_COMPRESSION : return "No compression";
        case LITTLE_COMPRESSION : return "Little compression";
        case STRONGEST : return "Strongest";
        default: return RFFConstants::Parser::UNKNOWN;
    }
}

