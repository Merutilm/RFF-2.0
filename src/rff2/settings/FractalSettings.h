#pragma once

#include "FrtGeneralSettings.hpp"
#include "FrtMPASettings.h"
#include "FrtPerturbSettings.hpp"
#include "FrtReferenceSettings.hpp"
#include "FrtSASettings.hpp"

namespace merutilm::rff2 {
    struct FractalSettings final {
        FrtGeneralSettings general;
        FrtReferenceSettings reference;
        FrtSASettings sa;
        FrtMPASettings mpa;
        FrtPerturbSettings perturb;
    };
}
