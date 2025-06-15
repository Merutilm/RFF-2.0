//
// Created by Merutilm on 2025-05-18.
//

#include "MandelbrotPerturbator.h"


merutilm::rff::MandelbrotPerturbator::MandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calculationSettings) : state(state), calc(calculationSettings) {

}


const merutilm::rff::CalculationSettings &merutilm::rff::MandelbrotPerturbator::getCalculationSettings() const {
    return calc;
}
