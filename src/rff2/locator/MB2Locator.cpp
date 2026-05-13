//
// Created by Merutilm on 2025-05-16.
//

#include "MB2Locator.h"

#include "../calc/dex_exp.h"
#include "../formula/DeepMB2Perturbator.h"
#include "../formula/MB2RenderData.hpp"
#include "../formula/Perturbator.h"


namespace merutilm::rff2 {
    std::unique_ptr<fixed_point_complex_i1> MB2Locator::findCenter(const MB2RenderDataBase &data) {
        const int exp10 = Perturbator::logZoomToExp10(data.fractalSettings.general.logZoom);
        fixed_point_complex_i1 center = data.getReference()->center.create_variant(exp10);
        const fixed_point_complex_i1 dc = findCenterOffset(data)->create_variant(exp10);

        const double dr = dc.clone_real().double_value();
        const double di = dc.clone_imag().double_value();
        if (const dex dcMax = data.getPerturbator()->dcMax; dex(dr * dr + di * di) > dcMax * dcMax) {
            return nullptr;
        }
        fixed_point_complex::add(center, center, dc);
        return std::make_unique<fixed_point_complex_i1>(center);
    }


    std::unique_ptr<fixed_point_complex_i1> MB2Locator::findCenterOffset(const MB2RenderDataBase &data) {
        const int exp10 = Perturbator::logZoomToExp10(data.fractalSettings.general.logZoom);
        const MB2Reference *reference = data.getReference();
        if (!reference) return nullptr;

        fixed_point_complex bn = reference->fpgBn.create_variant(exp10, -exp10 * 2);
        fixed_point_complex z = reference->fpgReference.create_variant(exp10, -exp10 * 2);
        fixed_point_complex::neg(bn);
        fixed_point_complex::div(z, z, bn);
        return std::make_unique<fixed_point_complex_i1>(z.real, z.imag, exp10);
    }

    std::unique_ptr<MB2Locator> MB2Locator::locateMinibrot(ParallelRenderState &state,
                                                                         const MB2RenderDataBase *data,
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


        std::unique_ptr<MB2RenderDataBase> result = findAccurateCenterPerturbator(
            state, data, actionWhileFindingMinibrotCenter, actionWhileCreatingTable);

        if (result == nullptr) {
            return nullptr;
        }
        dex resultDcMax = result->getPerturbator()->dcMax;

        auto &[bailout, logZoom] = result->fractalSettings.general;
        float resultZoom = logZoom;
        float zoomIncrement = resultZoom / 4;

        while (zoomIncrement > ZOOM_INCREMENT_LIMIT) {
            if (state.interruptRequested()) {
                return nullptr;
            }

            if (checkMaxIterationOnly(*result)) {
                resultZoom -= zoomIncrement;
                resultDcMax = resultDcMax * dex_exp::exp10(zoomIncrement);
            } else {
                resultZoom += zoomIncrement;
                resultDcMax = resultDcMax / dex_exp::exp10(zoomIncrement);
            }

            actionWhileFindingMinibrotZoom(resultZoom);
            logZoom = resultZoom;
            result->translate(logZoom, resultDcMax, result->fractalSettings.perturb, result->fractalSettings.reference.center);
            zoomIncrement /= 2;
        }

        return std::make_unique<MB2Locator>(std::move(result));
    }

    /**
     * This method moves the data, so the paramed data is no longer available.
     * Use the return value instead of this.
     * @param state the state
     * @param data the perturbator to move
     * @param actionWhileFindingMinibrotCenter action 1
     * @param actionWhileCreatingTable action 2
     * @return result table
     */
    std::unique_ptr<MB2RenderDataBase> MB2Locator::findAccurateCenterPerturbator(
            ParallelRenderState &state, const MB2RenderDataBase *data,
            const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter,
            const std::function<void(uint64_t, float)> &actionWhileCreatingTable) {
        // multiply zoom by 2 and find center offset.
        // set the center to center + centerOffset.

        uint64_t longestPeriod = data->getReference()->longestPeriod();
        uint64_t refLen = data->getReference()->length();

        const float logZoom = data->fractalSettings.general.logZoom;
        const FractalSettings &calc = data->fractalSettings;
        FractalSettings doubledZoomCalc = calc;
        const float doubledLogZoom = logZoom * 2;
        const int doubledExp10 = Perturbator::logZoomToExp10(doubledLogZoom);

        doubledZoomCalc.general.logZoom = doubledLogZoom;
        doubledZoomCalc.perturb.absoluteIterationMode = false;
        doubledZoomCalc.perturb.decimalizeIterationMethod = FrtDecimalizeIterationMethod::NONE;


        dex doubledZoomDcMax = data->getPerturbator()->dcMax / dex_exp::exp10(logZoom);


        int centerFixCount = 0;

        std::unique_ptr<MB2RenderDataBase> doubledZoomData = nullptr;

        while (doubledZoomData == nullptr || !doubledZoomData->getPerturbator() || !checkMaxIterationOnly(*doubledZoomData)) {
            if (state.interruptRequested()) {
                return nullptr;
            }

            auto center = doubledZoomCalc.reference.center.create_variant(doubledExp10);
            auto centerOffset = findCenterOffset(doubledZoomData == nullptr ? *data : *doubledZoomData)->create_variant(doubledExp10);

            fixed_point_complex::add(center, center, centerOffset);

            if (centerOffset.is_strict_zero()) {
                MessageBox(nullptr, "No center found", "Error",
                                       MB_OK | MB_ICONERROR);
                return nullptr;
            }
            doubledZoomCalc.reference.center = center;
            ++centerFixCount;

            if (doubledLogZoom < Constants::Fractal::ZOOM_DEADLINE) {
                doubledZoomData = std::make_unique<MB2RenderData<LightPA>>(
                    state, doubledZoomCalc, doubledZoomDcMax,
                    Perturbator::logZoomToExp10(doubledLogZoom), refLen, longestPeriod,
                    [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                        actionWhileFindingMinibrotCenter(p, centerFixCount);
                    }, actionWhileCreatingTable, true);
            } else {
                doubledZoomData = std::make_unique<MB2RenderData<DeepPA>>(
                    state, doubledZoomCalc, doubledZoomDcMax, Perturbator::logZoomToExp10(doubledLogZoom), refLen, longestPeriod,
                    [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                        actionWhileFindingMinibrotCenter(p, centerFixCount);
                    }, actionWhileCreatingTable, true);
            }


        }


        return doubledZoomData;
    }

    bool MB2Locator::checkMaxIterationOnly(const MB2RenderDataBase &renderData) {

        const auto it = static_cast<uint64_t>(renderData.getPerturbator()->iterate(
                renderData.getPerturbator()->dcMax,
                renderData.getPerturbator()->dcMax / dex(Constants::Fractal::INTENTIONAL_ERROR_DCLMB)));

        return it == renderData.fractalSettings.perturb.maxIteration;
    }
}