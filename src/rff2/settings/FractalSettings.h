#pragma once

#include "FrtGeneralSettings.hpp"
#include "FrtMPASettings.h"
#include "FrtPerturbSettings.hpp"
#include "FrtReferenceSettings.hpp"

namespace merutilm::rff2 {
    struct FractalSettings final {
        FrtGeneralSettings general;
        FrtReferenceSettings reference;
        FrtMPASettings mpa;
        FrtPerturbSettings perturb;
    };
}
