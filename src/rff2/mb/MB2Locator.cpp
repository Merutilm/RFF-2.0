//
// Created by Merutilm on 2025-05-16.
//

#include "MB2Locator.h"

#include "MB2Reference.h"
#include "MB2RenderData.hpp"
#include "Perturbator.h"


namespace merutilm::rff2 {

    std::unique_ptr<fixed_point_complex> MB2Locator::findCenterOffset(const MB2RenderDataBase &data) {
        const int exp10 = Perturbator::logZoomToExp10(data.fractalSettings.general.logZoom);
        const MB2ReferenceBase *reference = data.getReference();
        if (!reference) return nullptr;

        fixed_point_complex bn = reference->fpgBn.create_variant(exp10);
        fixed_point_complex z = reference->fpgReference.create_variant(exp10);
        fixed_point_complex::neg(bn);
        fixed_point_complex::div(z, z, bn);
        return std::make_unique<fixed_point_complex>(z.real, z.imag, exp10);
    }

    std::unique_ptr<MB2Locator> MB2Locator::locateMinibrot(vkh::Core &core, ParallelRenderState &state,
                                                                         const MB2RenderDataBase &data,
                                                                         std::unique_ptr<ApproxTableCacheBase> &cache,
                                                                         const std::function<void(uint64_t, int)> &
                                                                         actionWhileFindingMinibrotCenter,
                                                                         const std::function<void (uint64_t, float)> &
                                                                         actionWhileSeriesApprox,
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


        std::unique_ptr<MB2RenderDataBase> result = findAccurateCenterPerturbator(core, state, data, cache, actionWhileFindingMinibrotCenter,
                                              actionWhileSeriesApprox, actionWhileCreatingTable);

        if (result == nullptr) {
            return nullptr;
        }
        dex resultDcMax = result->getPerturbator()->dcMax;

        auto &logZoom = result->fractalSettings.general.logZoom;
        float resultZoom = logZoom;
        float zoomIncrement = resultZoom / 4;

        while (zoomIncrement > ZOOM_INCREMENT_LIMIT) {
            if (state.interruptRequested()) {
                return nullptr;
            }

            if (checkMaxIterationOnly(*result)) {
                resultZoom -= zoomIncrement;
                resultDcMax = resultDcMax * rff_math::exp10(zoomIncrement);
            } else {
                resultZoom += zoomIncrement;
                resultDcMax = resultDcMax / rff_math::exp10(zoomIncrement);
            }

            actionWhileFindingMinibrotZoom(resultZoom);
            logZoom = resultZoom;
            result->translate(logZoom, resultDcMax, result->fractalSettings.perturb, result->fractalSettings.reference.center, actionWhileSeriesApprox);
            zoomIncrement /= 2;
        }

        return std::make_unique<MB2Locator>(std::move(result));
    }

    /**
     * This method moves the data, so the paramed data is no longer available.
     * Use the return value instead of this.
     * @return result table
     */
    std::unique_ptr<MB2RenderDataBase> MB2Locator::findAccurateCenterPerturbator(
            vkh::Core &core, ParallelRenderState &state, const MB2RenderDataBase &data, std::unique_ptr<ApproxTableCacheBase> &cache,
            const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter,
            const std::function<void (uint64_t, float)> &actionWhileSeriesApprox,
            const std::function<void(uint64_t, float)> &actionWhileCreatingTable) {
        // multiply zoom by 2 and find center offset.
        // set the center to center + centerOffset.

        uint64_t longestPeriod = data.getReference()->longestPeriod();
        uint64_t refLen = data.getReference()->length();

        const float logZoom = data.fractalSettings.general.logZoom;
        const FractalSettings &calc = data.fractalSettings;
        FractalSettings doubledZoomCalc = calc;
        const float doubledLogZoom = logZoom * 2;
        const int doubledExp10 = Perturbator::logZoomToExp10(doubledLogZoom);

        doubledZoomCalc.general.logZoom = doubledLogZoom;
        doubledZoomCalc.perturb.absoluteIterationMode = false;
        doubledZoomCalc.perturb.decimalizeIterationMethod = FrtDecimalizeIterationMethod::NONE;


        dex doubledZoomDcMax = data.getPerturbator()->dcMax / rff_math::exp10(logZoom);


        int centerFixCount = 0;

        std::unique_ptr<MB2RenderDataBase> doubledZoomData = nullptr;
        MB2ReferenceBase *oldReference = data.getReference();

        while (doubledZoomData == nullptr || !doubledZoomData->getPerturbator() || !checkMaxIterationOnly(*doubledZoomData)) {

            auto center = doubledZoomCalc.reference.center.create_variant(doubledExp10);
            auto centerOffset = findCenterOffset(doubledZoomData == nullptr ? data : *doubledZoomData)->create_variant(doubledExp10);

            fixed_point_complex::add(center, center, centerOffset);

            if (state.interruptRequested() || centerOffset.is_zero()) {
                if (centerOffset.is_zero()) vkh::logger::log_err("The center could not be found, or you are already in the center");

                if (doubledZoomData) {
                    //recover
                    data.getReference()->center = doubledZoomData->getReference()->extractCenter();
                    data.getReference()->checkpoints = doubledZoomData->getReference()->extractCheckpoints();
                }
                return nullptr;
            }
            doubledZoomCalc.reference.center = center;
            ++centerFixCount;


            if (doubledLogZoom < Constants::Fractal::MULTITHREAD_ZOOM_THRESHOLD) {
                doubledZoomData = std::make_unique<DoubleMB2RenderData>(
                    core, state, doubledZoomCalc, false, cache, doubledZoomDcMax,
                    Perturbator::logZoomToExp10(doubledLogZoom), refLen, longestPeriod, oldReference,
                    [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                        actionWhileFindingMinibrotCenter(p, centerFixCount);
                    }, actionWhileSeriesApprox, actionWhileCreatingTable);

            } else {
                doubledZoomData = std::make_unique<DexMB2RenderData>(
                    core, state, doubledZoomCalc, false, cache, doubledZoomDcMax, Perturbator::logZoomToExp10(doubledLogZoom),
                    refLen, longestPeriod, oldReference,
                    [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                        actionWhileFindingMinibrotCenter(p, centerFixCount);
                    }, actionWhileSeriesApprox, actionWhileCreatingTable);
            }
            oldReference = doubledZoomData->getReference();
        }
        return doubledZoomData;
    }

    bool MB2Locator::checkMaxIterationOnly(const MB2RenderDataBase &renderData) {

        const auto it = static_cast<uint64_t>(renderData.getPerturbator()->iterate( {
            renderData.getPerturbator()->dcMax,
            renderData.getPerturbator()->dcMax / dex(2)
        }));

        return it == renderData.fractalSettings.perturb.maxIteration;
    }
}