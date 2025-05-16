//
// Created by Merutilm on 2025-05-16.
//

#include "MandelbrotLocator.h"

#include "../formula/Perturbator.h"
#include <cmath>
#include <iostream>


std::unique_ptr<Center> MandelbrotLocator::findCenter(const LightMandelbrotPerturbator *perturbator) {
    const int exp10 = Perturbator::logZoomToExp10(perturbator->getCalculationSettings().logZoom);
    GMPComplexCalculator center = perturbator->getReference()->center.edit(exp10);
    const GMPComplexCalculator dc = findCenterOffset(*perturbator)->edit(exp10);
    const double dr = dc.getRealClone().doubleValue();
    const double di = dc.getImagClone().doubleValue();
    if (const double dcMax = perturbator->getDcMax();
        dr * dr + di * di > dcMax * dcMax) {
        return nullptr;
    }
    center += dc;
    return std::make_unique<Center>(center);
}


std::unique_ptr<Center> MandelbrotLocator::findCenterOffset(const LightMandelbrotPerturbator &perturbator) {
    const int exp10 = Perturbator::logZoomToExp10(perturbator.getCalculationSettings().logZoom);
    const LightMandelbrotReference *reference = perturbator.getReference();
    if (reference == RFF::NullPointer::PROCESS_TERMINATED_REFERENCE) {
        return nullptr;
    }

    GMPComplexCalculator bn = reference->fpgBn.edit(exp10);
    GMPComplexCalculator z = reference->lastReference.edit(exp10);
    z /= bn.negate();
    return std::make_unique<Center>(z);
}

std::unique_ptr<MandelbrotLocator> MandelbrotLocator::locateMinibrot(ParallelRenderState &state,
                                                                     std::unique_ptr<LightMandelbrotPerturbator> perturbator,
                                                                     const std::function<void(uint64_t, int)> &
                                                                     actionWhileFindingMinibrotCenter,
                                                                     const std::function<void (uint64_t, float)> &
                                                                     actionWhileCreatingTable,
                                                                     const std::function<void(float)>
                                                                     &actionWhileFindingMinibrotZoom) {
    // code flowing
    // e.g. zoom * 2 -> zoom * 1.5 -> zoom * 1.75.....
    // it is not required reference calculations.
    // check 'dcMax' iterate and check its iteration is max iteration
    // if true, zoom out. otherwise, zoom in.
    // it can approximate zoom when repeats until zoom increment is lower than
    // specific small number. O(log N)


    std::unique_ptr<LightMandelbrotPerturbator> result = findAccurateCenterPerturbator(state, std::move(perturbator), actionWhileFindingMinibrotCenter, actionWhileCreatingTable);
    perturbator = nullptr;

    double resultDcMax = result->getDcMax();
    CalculationSettings resultCalc = result->getCalculationSettings();
    float resultZoom = resultCalc.logZoom;
    const uint64_t maxIteration = resultCalc.maxIteration;
    float zoomIncrement = resultZoom / 4;

    while (zoomIncrement > RFF::Locator::ZOOM_INCREMENT_LIMIT) {
        if (state.interruptRequested()) {
            return std::make_unique<MandelbrotLocator>(std::move(result));
        }

        if (checkMaxIterationOnly(*result, maxIteration)) {
            resultZoom -= zoomIncrement;
            resultDcMax = resultDcMax * pow(10, zoomIncrement);
        } else {
            resultZoom += zoomIncrement;
            resultDcMax = resultDcMax / pow(10, zoomIncrement);
        }

        actionWhileFindingMinibrotZoom(resultZoom);

        resultCalc.logZoom = resultZoom;
        result = result->reuse(resultCalc, resultDcMax, Perturbator::logZoomToExp10(resultZoom));
        zoomIncrement /= 2;
    }

    return std::make_unique<MandelbrotLocator>(std::move(result));
}

/**
 * This method moves the perturbator, so the paramed perturbator is no longer available.
 * Use the return value instead of this.
 * @param state the state
 * @param perturbator the perturbator to move
 * @param actionWhileFindingMinibrotCenter action 1
 * @param actionWhileCreatingTable action 2
 * @return result table
 */
std::unique_ptr<LightMandelbrotPerturbator> MandelbrotLocator::findAccurateCenterPerturbator(ParallelRenderState &state,
                                                                                             std::unique_ptr<LightMandelbrotPerturbator> perturbator,
                                                                                             const std::function<void(uint64_t, int)> &
                                                                                             actionWhileFindingMinibrotCenter,
                                                                                             const std::function<void(uint64_t, float)> &
                                                                                             actionWhileCreatingTable) {
    // multiply zoom by 2 and find center offset.
    // set the center to center + centerOffset.

    uint64_t longestPeriod = perturbator->getReference()->longestPeriod();

    const int logZoom = perturbator->getCalculationSettings().logZoom;
    const CalculationSettings &calc = perturbator->getCalculationSettings();
    CalculationSettings doubledZoomCalc = calc;
    const uint64_t maxIteration = doubledZoomCalc.maxIteration;
    const int doubledLogZoom = logZoom * 2;
    const int exp10 = Perturbator::logZoomToExp10(doubledLogZoom);

    doubledZoomCalc.center = Center(doubledZoomCalc.center.edit(exp10) += findCenterOffset(*perturbator.get())->edit(exp10));
    doubledZoomCalc.logZoom = static_cast<float>(doubledLogZoom);

    double doubledZoomDcMax = perturbator->getDcMax() / pow(10, logZoom);


    int centerFixCount = 0;

    std::unique_ptr<LightMandelbrotPerturbator> doubledZoomPerturbator = std::move(perturbator);
    perturbator = nullptr;

    while (!checkMaxIterationOnly(*doubledZoomPerturbator, maxIteration)) {
        if (state.interruptRequested()) {
            return doubledZoomPerturbator;
            //try to save the vector
        }

        Center off = *findCenterOffset(*doubledZoomPerturbator);
        doubledZoomCalc.center = Center(doubledZoomCalc.center.edit(exp10) += off.edit(exp10));

        ++centerFixCount;
        // if(perturbator.getExp10() < DoubleExponent.EXP_DEADLINE / 2){
        doubledZoomPerturbator = std::make_unique<LightMandelbrotPerturbator>(
            state, doubledZoomCalc, doubledZoomDcMax, Perturbator::logZoomToExp10(doubledLogZoom), longestPeriod,
            doubledZoomPerturbator->getTable().extractVector(), //IT MOVES THE TABLE!!!!!!!!!!!!!!!!!!!!!!
            [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                actionWhileFindingMinibrotCenter(p, centerFixCount);
            }, actionWhileCreatingTable, true);
        // }else{
        //     doubledZoomScene = new DeepMandelbrotPerturbator(state, currentID, doubledZoomCalc, doubledZoomDcMax, doubledZoomExp10, longestPeriod, p -> actionWhileFindingMinibrotCenter.accept(p, centerFixCount.get()), actionWhileCreatingTable, true);
        // }
    }


    return doubledZoomPerturbator;
}

bool MandelbrotLocator::checkMaxIterationOnly(const LightMandelbrotPerturbator &perturbator,
                                              const uint64_t maxIteration) {
    return perturbator.iterate(perturbator.getDcMax(), 0) / static_cast<double>(maxIteration) == 1;
}
