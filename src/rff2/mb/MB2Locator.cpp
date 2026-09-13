//
// Created by Merutilm on 2025-05-16.
//

#include "MB2Locator.h"

#include "MB2Reference.h"
#include "MB2RenderData.hpp"
#include "Perturbator.h"


namespace merutilm::rff2 {

    std::unique_ptr<fixed_point_complex> MB2Locator::findCenterOffset(const MB2ReferenceBase &reference) {
        const int exp10 = Perturbator::logZoomToExp10(reference.generalSettings.logZoom);
        fixed_point_complex bn = reference.fpgBn.create_variant(exp10);
        fixed_point_complex z = reference.checkpoints.back().complex.create_variant(exp10);
        fixed_point_complex::neg(bn);
        fixed_point_complex::div(z, z, bn);
        return std::make_unique<fixed_point_complex>(z.real, z.imag, exp10);
    }

    std::optional<MB2Locator>
    MB2Locator::locateMinibrot(const ParallelRenderState &state, const MB2RenderDataBase &data,
                               const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter) {

        const std::unique_ptr<MB2ReferenceBase> result =
                findAccurateCenterReference(state, data, actionWhileFindingMinibrotCenter);

        if (result == nullptr) {
            return std::nullopt;
        }
        const float resultLogZoom =
                rff_math::log10((result->fzgAn * static_cast<complex<dex>>(result->fpgBn)).norm_approx()) + MINIBROT_LOG_ZOOM_OFFSET;

        return MB2Locator{std::move(result->center), resultLogZoom};
    }

    /**
     * This method moves the data, so the paramed data is no longer available.
     * Use the return value instead of this.
     * @return result table
     */
    std::unique_ptr<MB2ReferenceBase>
    MB2Locator::findAccurateCenterReference(const ParallelRenderState &state, const MB2RenderDataBase &data,
            const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter) {
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

        const dex dcMax = data.getPerturbator()->dcMax;
        const dex doubledZoomDcMax = dcMax / rff_math::exp10(logZoom);

        int centerFixCount = 0;
        fixed_point_complex centerOffset(data.getPerturbator()->dcMax, dex::ZERO, doubledExp10);
        std::unique_ptr<MB2ReferenceBase> doubledZoomReference = nullptr;

        while (doubledZoomReference == nullptr ||
               static_cast<complex<dex>>(centerOffset).norm_approx() > doubledZoomDcMax) {

            auto center = doubledZoomCalc.reference.center.create_variant(doubledExp10);
            centerOffset =
                    findCenterOffset(doubledZoomReference == nullptr ? *data.getReference() : *doubledZoomReference)
                            ->create_variant(doubledExp10);

            fixed_point_complex::add(center, center, centerOffset);

            if (state.interruptRequested()) {
                return nullptr;
            }

            doubledZoomCalc.reference.center = center;
            ++centerFixCount;


            if (doubledLogZoom < Constants::Fractal::MULTITHREAD_ZOOM_THRESHOLD) {
                std::unique_ptr<MB2Reference<double>> ref;
                MB2Reference<double>::generateReference(
                        state, doubledZoomCalc.general, doubledZoomCalc.reference,
                        Perturbator::logZoomToExp10(doubledLogZoom), refLen, longestPeriod, longestPeriod,
                        doubledZoomDcMax,
                        [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                            actionWhileFindingMinibrotCenter(p, centerFixCount);
                        },
                        &ref);
                doubledZoomReference = std::move(ref);

            } else {
                std::unique_ptr<MB2Reference<dex>> ref;
                MB2Reference<dex>::generateReference(
                        state, doubledZoomCalc.general, doubledZoomCalc.reference,
                        Perturbator::logZoomToExp10(doubledLogZoom), refLen, longestPeriod, longestPeriod,
                        doubledZoomDcMax,
                        [&actionWhileFindingMinibrotCenter, &centerFixCount](const uint64_t p) {
                            actionWhileFindingMinibrotCenter(p, centerFixCount);
                        },
                        &ref);
                doubledZoomReference = std::move(ref);
            }
        }
        return doubledZoomReference;
    }

    bool MB2Locator::checkMaxIterationOnly(const MB2RenderDataBase &renderData) {

        const auto it = static_cast<uint64_t>(renderData.getPerturbator()->iterate(
                {renderData.getPerturbator()->dcMax, renderData.getPerturbator()->dcMax / dex(2)}));

        return it == renderData.fractalSettings.perturb.maxIteration;
    }
} // namespace merutilm::rff2
