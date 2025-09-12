//
// Created by Merutilm on 2025-05-16.
//

#include "MandelbrotLocator.h"

#include "../formula/Perturbator.h"
#include "../calc/dex_exp.h"
#include "../data/ApproxTableCache.h"
#include "../formula/DeepMandelbrotPerturbator.h"


namespace merutilm::rff2 {
    std::unique_ptr<fp_complex> MandelbrotLocator::findCenter(const MandelbrotPerturbator *perturbator) {
        const int exp10 = Perturbator::logZoomToExp10(perturbator->getCalculationSettings().logZoom);
        fp_complex_calculator center = perturbator->getReference()->center.edit(exp10);
        const fp_complex_calculator dc = findCenterOffset(*perturbator)->edit(exp10);
        const double dr = dc.getRealClone().double_value();
        const double di = dc.getImagClone().double_value();
        if (const dex dcMax = perturbator->getDcMaxAsDoubleExp();
            dr * dr + di * di > dcMax * dcMax) {
            return nullptr;
            }
        center += dc;
        return std::make_unique<fp_complex>(center);
    }


    std::unique_ptr<fp_complex> MandelbrotLocator::findCenterOffset(const MandelbrotPerturbator &perturbator) {
        const int exp10 = Perturbator::logZoomToExp10(perturbator.getCalculationSettings().logZoom);
        const MandelbrotReference *reference = perturbator.getReference();
        if (reference == Constants::NullPointer::PROCESS_TERMINATED_REFERENCE) {
            return nullptr;
        }

        fp_complex_calculator bn = reference->fpgBn.edit(exp10);
        fp_complex_calculator z = reference->fpgReference.edit(exp10);
        z /= bn.negate();
        return std::make_unique<fp_complex>(z);
    }

    std::unique_ptr<MandelbrotLocator> MandelbrotLocator::locateMinibrot(ParallelRenderState &state,
                                                                         const MandelbrotPerturbator *perturbator,
                                                                         ApproxTableCache &approxTableCache,
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
        // specific small number. O(w_log N)


        std::unique_ptr<MandelbrotPerturbator> result = findAccurateCenterPerturbator(
            state, perturbator, approxTableCache, actionWhileFindingMinibrotCenter, actionWhileCreatingTable);

        if (result == nullptr) {
            return nullptr;
        }
        dex resultDcMax = result->getDcMaxAsDoubleExp();
        FractalAttribute resultCalc = result->getCalculationSettings();
        resultCalc.absoluteIterationMode = false;
        float resultZoom = resultCalc.logZoom;
        const uint64_t maxIteration = resultCalc.maxIteration;
        float zoomIncrement = resultZoom / 4;

        while (zoomIncrement > ZOOM_INCREMENT_LIMIT) {
            if (state.interruptRequested()) {
                return nullptr;
            }

            if (checkMaxIterationOnly(*result, maxIteration)) {
                resultZoom -= zoomIncrement;
                resultDcMax = resultDcMax * dex_exp::exp10(zoomIncrement);
            } else {
                resultZoom += zoomIncrement;
                resultDcMax = resultDcMax / dex_exp::exp10(zoomIncrement);
            }

            actionWhileFindingMinibrotZoom(resultZoom);
            resultCalc.logZoom = resultZoom;
            if (const auto v = dynamic_cast<LightMandelbrotPerturbator *>(result.get())) {
                result = v->reuse(resultCalc, static_cast<double>(resultDcMax), approxTableCache);
            }
            if (const auto v = dynamic_cast<DeepMandelbrotPerturbator *>(result.get())) {
                result = v->reuse(resultCalc, resultDcMax, approxTableCache);
            }
            zoomIncrement /= 2;
        }

        return std::make_unique<MandelbrotLocator>(std::move(result));
    }

    /**
     * This method moves the perturbator, so the paramed perturbator is no longer available.
     * Use the return value instead of this.
     * @param state the state
     * @param perturbator the perturbator to move
     * @param approxTableCache the cache of table
     * @param actionWhileFindingMinibrotCenter action 1
     * @param actionWhileCreatingTable action 2
     * @return result table
     */
    std::unique_ptr<MandelbrotPerturbator> MandelbrotLocator::findAccurateCenterPerturbator(ParallelRenderState &state,
        const MandelbrotPerturbator *perturbator,
        ApproxTableCache &approxTableCache,
        const std::function<void(uint64_t, int)> &
        actionWhileFindingMinibrotCenter,
        const std::function<void(uint64_t, float)> &
        actionWhileCreatingTable) {
        // multiply zoom by 2 and find center offset.
        // set the center to center + centerOffset.

        uint64_t longestPeriod = perturbator->getReference()->longestPeriod();

        const float logZoom = perturbator->getCalculationSettings().logZoom;
        const FractalAttribute &calc = perturbator->getCalculationSettings();
        FractalAttribute doubledZoomCalc = calc;
        const uint64_t maxIteration = doubledZoomCalc.maxIteration;
        const float doubledLogZoom = logZoom * 2;
        const int doubledExp10 = Perturbator::logZoomToExp10(doubledLogZoom);
        auto e = doubledZoomCalc.center.edit(doubledExp10);
        doubledZoomCalc.absoluteIterationMode = false;
        doubledZoomCalc.center = fp_complex(e += findCenterOffset(*perturbator)->edit(doubledExp10));
        doubledZoomCalc.logZoom = doubledLogZoom;

        dex doubledZoomDcMax = perturbator->getDcMaxAsDoubleExp() / dex_exp::exp10(logZoom);


        int centerFixCount = 0;

        std::unique_ptr<MandelbrotPerturbator> doubledZoomPerturbator = nullptr;

        while (doubledZoomPerturbator == nullptr || !checkMaxIterationOnly(*doubledZoomPerturbator, maxIteration)) {
            if (state.interruptRequested()) {
                return nullptr;
                //try to save the vector
            }

            fp_complex off = *findCenterOffset(doubledZoomPerturbator == nullptr ? *perturbator : *doubledZoomPerturbator);
            e = doubledZoomCalc.center.edit(doubledExp10);
            doubledZoomCalc.center = fp_complex(e += off.edit(doubledExp10));
            ++centerFixCount;

            if (logZoom < Constants::Fractal::ZOOM_DEADLINE / 2) {
                doubledZoomPerturbator = std::make_unique<LightMandelbrotPerturbator>(
                    state, doubledZoomCalc, static_cast<double>(doubledZoomDcMax),
                    Perturbator::logZoomToExp10(doubledLogZoom), longestPeriod,
                    approxTableCache,
                    [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                        actionWhileFindingMinibrotCenter(p, centerFixCount);
                    }, actionWhileCreatingTable, true);
            } else {
                doubledZoomPerturbator = std::make_unique<DeepMandelbrotPerturbator>(
                    state, doubledZoomCalc, doubledZoomDcMax, Perturbator::logZoomToExp10(doubledLogZoom), longestPeriod,
                    approxTableCache,
                    [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                        actionWhileFindingMinibrotCenter(p, centerFixCount);
                    }, actionWhileCreatingTable, true);
            }
        }


        return doubledZoomPerturbator;
    }

    bool MandelbrotLocator::checkMaxIterationOnly(const MandelbrotPerturbator &perturbator,
                                                  const uint64_t maxIteration) {
        return perturbator.iterate(perturbator.getDcMaxAsDoubleExp(),
                                   perturbator.getDcMaxAsDoubleExp() / Constants::Fractal::INTENTIONAL_ERROR_DCLMB) == static_cast<
                   double>(maxIteration);
    }
}