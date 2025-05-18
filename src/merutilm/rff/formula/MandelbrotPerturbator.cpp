//
// Created by Merutilm on 2025-05-18.
//

#include "MandelbrotPerturbator.h"


MandelbrotPerturbator::MandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calculationSettings) : state(state), calc(calculationSettings) {

}


const CalculationSettings &MandelbrotPerturbator::getCalculationSettings() const {
    return calc;
}
