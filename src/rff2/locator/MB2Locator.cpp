//
// Created by Merutilm on 2025-05-16.
//

#include "MB2Locator.h"

#include "../calc/dex_exp.h"
#include "../formula/DeepMB2Perturbator.h"
#include "../formula/Perturbator.h"


namespace merutilm::rff2 {
    std::unique_ptr<fixed_point_complex_i1> MB2Locator::findCenter(const MB2Perturbator *perturbator) {
        const int exp10 = Perturbator::logZoomToExp10(perturbator->getCalculationSettings().logZoom);
        fixed_point_complex_i1 center = perturbator->getReference()->center.create_variant(exp10);
        const fixed_point_complex_i1 dc = findCenterOffset(*perturbator)->create_variant(exp10);

        const double dr = dc.clone_real().double_value();
        const double di = dc.clone_imag().double_value();
        if (const dex dcMax = perturbator->getDcMaxAsDoubleExp(); dex(dr * dr + di * di) > dcMax * dcMax) {
            return nullptr;
        }
        fixed_point_complex::add(center, center, dc);
        return std::make_unique<fixed_point_complex_i1>(center);
    }


    std::unique_ptr<fixed_point_complex_i1> MB2Locator::findCenterOffset(const MB2Perturbator &perturbator) {
        const int exp10 = Perturbator::logZoomToExp10(perturbator.getCalculationSettings().logZoom);
        const MB2Reference *reference = perturbator.getReference();
        if (!reference) return nullptr;

        fixed_point_complex bn = reference->fpgBn.create_variant(exp10, -exp10 * 2);
        fixed_point_complex z = reference->fpgReference.create_variant(exp10, -exp10 * 2);
        fixed_point_complex::neg(bn);
        fixed_point_complex::div(z, z, bn);
        return std::make_unique<fixed_point_complex_i1>(z.real, z.imag, exp10);
    }

    std::unique_ptr<MB2Locator> MB2Locator::locateMinibrot(ParallelRenderState &state,
                                                                         const MB2Perturbator *perturbator,
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


        std::unique_ptr<MB2Perturbator> result = findAccurateCenterPerturbator(
            state, perturbator, actionWhileFindingMinibrotCenter, actionWhileCreatingTable);

        if (result == nullptr) {
            return nullptr;
        }
        dex resultDcMax = result->getDcMaxAsDoubleExp();
        FractalSettings resultCalc = result->getCalculationSettings();
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
            if (const auto v = dynamic_cast<LightMB2Perturbator *>(result.get())) {
                result = v->reuse(resultCalc, static_cast<double>(resultDcMax));
            }
            if (const auto v = dynamic_cast<DeepMB2Perturbator *>(result.get())) {
                result = v->reuse(resultCalc, resultDcMax);
            }
            zoomIncrement /= 2;
        }

        return std::make_unique<MB2Locator>(std::move(result));
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
    std::unique_ptr<MB2Perturbator> MB2Locator::findAccurateCenterPerturbator(ParallelRenderState &state,
        const MB2Perturbator *perturbator,
        const std::function<void(uint64_t, int)> &
        actionWhileFindingMinibrotCenter,
        const std::function<void(uint64_t, float)> &
        actionWhileCreatingTable) {
        // multiply zoom by 2 and find center offset.
        // set the center to center + centerOffset.

        uint64_t longestPeriod = perturbator->getReference()->longestPeriod();
        uint64_t refLen = perturbator->getReference()->length();

        const float logZoom = perturbator->getCalculationSettings().logZoom;
        const FractalSettings &calc = perturbator->getCalculationSettings();
        FractalSettings doubledZoomCalc = calc;
        const uint64_t maxIteration = doubledZoomCalc.maxIteration;
        const float doubledLogZoom = logZoom * 2;
        const int doubledExp10 = Perturbator::logZoomToExp10(doubledLogZoom);



        doubledZoomCalc.absoluteIterationMode = false;
        doubledZoomCalc.logZoom = doubledLogZoom;


        dex doubledZoomDcMax = perturbator->getDcMaxAsDoubleExp() / dex_exp::exp10(logZoom);


        int centerFixCount = 0;

        std::unique_ptr<MB2Perturbator> doubledZoomPerturbator = nullptr;

        while (doubledZoomPerturbator == nullptr || !checkMaxIterationOnly(*doubledZoomPerturbator, maxIteration)) {
            if (state.interruptRequested()) {
                return nullptr;
                // try to save the vector
            }

            auto center = doubledZoomCalc.center.create_variant(doubledExp10);
            auto centerOffset = findCenterOffset(doubledZoomPerturbator == nullptr ? *perturbator : *doubledZoomPerturbator)->create_variant(doubledExp10);


            fixed_point_complex::add(center, center, centerOffset);
            doubledZoomCalc.center = center;
            ++centerFixCount;

            if (logZoom < Constants::Fractal::ZOOM_DEADLINE / 2) {
                doubledZoomPerturbator = std::make_unique<LightMB2Perturbator>(
                    state, doubledZoomCalc, static_cast<double>(doubledZoomDcMax),
                    Perturbator::logZoomToExp10(doubledLogZoom), refLen, longestPeriod,
                    [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                        actionWhileFindingMinibrotCenter(p, centerFixCount);
                    }, actionWhileCreatingTable, true);
            } else {
                doubledZoomPerturbator = std::make_unique<DeepMB2Perturbator>(
                    state, doubledZoomCalc, doubledZoomDcMax, Perturbator::logZoomToExp10(doubledLogZoom), refLen, longestPeriod,
                    [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                        actionWhileFindingMinibrotCenter(p, centerFixCount);
                    }, actionWhileCreatingTable, true);
            }
        }


        return doubledZoomPerturbator;
    }

    bool MB2Locator::checkMaxIterationOnly(const MB2Perturbator &perturbator,
                                                  const uint64_t maxIteration) {
        FractalSettings fs = perturbator.calc;
        fs.maxIteration = maxIteration;
        fs.absoluteIterationMode = false;
        fs.decimalizeIterationMethod = FrtDecimalizeIterationMethod::NONE;
        const auto it = static_cast<uint64_t>(perturbator.iterate(fs, perturbator.getDcMaxAsDoubleExp(),
                                   perturbator.getDcMaxAsDoubleExp() /
                                           dex(Constants::Fractal::INTENTIONAL_ERROR_DCLMB)));

        return it == maxIteration;
    }
}