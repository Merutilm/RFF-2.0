//
// Created by Merutilm on 2026-05-13.
//

#pragma once
#include <utility>
#include "../settings/FractalSettings.h"
#include "MB2Perturbator.h"
#include "MB2Reference.h"
namespace merutilm::rff2 {

    struct MB2RenderDataBase {

        FractalSettings fractalSettings;
        Reference::CreationResult lastCreationResult = Reference::CreationResult::UNDEFINED;

        explicit MB2RenderDataBase(FractalSettings frt) : fractalSettings(std::move(frt)) {}

        virtual ~MB2RenderDataBase() = default;

        [[nodiscard]] virtual MB2ReferenceBase *getReference() const = 0;
        [[nodiscard]] virtual MB2PerturbatorBase *getPerturbator() const = 0;

        virtual void translate(float logZoom, dex dcMax, FrtPerturbSettings ptbSettings,
                               const fixed_point_complex_i1 &newCenter) = 0;

        static int logZoomToExp10(const float logZoom) {
            return -static_cast<int>(logZoom) - Constants::Fractal::EXP10_ADDITION;
        }
    };

    template<Number Num>
    struct MB2RenderData final : MB2RenderDataBase {


        std::unique_ptr<MB2Reference<Num>> reference;
        std::unique_ptr<MPATable<Num>> table;
        std::unique_ptr<MB2Perturbator<Num>> perturbator;

        explicit MB2RenderData(ParallelRenderState &state, const FractalSettings &frt, dex dcMax, int exp10,
                               uint64_t refInitialCapacity, uint64_t fixedPeriod,
                               std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                               bool arbitraryPrecisionFPGBn);

        static std::unique_ptr<MB2RenderData>
        generateNew(ParallelRenderState &state, const FractalSettings &frt, dex dcMax, int exp10,
                    uint64_t refInitialCapacity, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                    std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

        [[nodiscard]] MB2ReferenceBase *getReference() const override { return reference.get(); }

        [[nodiscard]] MB2PerturbatorBase *getPerturbator() const override { return perturbator.get(); }

        void translate(float logZoom, dex dcMax, FrtPerturbSettings ptbSettings,
                       const fixed_point_complex_i1 &newCenter) override;

        void applyAutoMaxIteration();
    };

    template<Number Num>
    MB2RenderData<Num>::MB2RenderData(ParallelRenderState &state, const FractalSettings &frt, const dex dcMax,
                                    const int exp10, const uint64_t refInitialCapacity, const uint64_t fixedPeriod,
                                    std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                    std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                                    const bool arbitraryPrecisionFPGBn) : MB2RenderDataBase(frt) {
        this->lastCreationResult = MB2Reference<Num>::generateReference(
                state, frt.general, frt.reference, exp10, refInitialCapacity, fixedPeriod, dcMax,
                arbitraryPrecisionFPGBn, std::move(actionPerRefCalcIteration), &reference);

        if (this->lastCreationResult != Reference::CreationResult::SUCCESS) {
            table = nullptr;
            perturbator = nullptr;
            return;
        }

        applyAutoMaxIteration();
        table = std::make_unique<MPATable<Num>>(state, *reference, &fractalSettings.mpa, Num(dcMax),
                                                     std::move(actionPerCreatingTableIteration));
        perturbator = std::make_unique<MB2Perturbator<Num>>(state, dcMax, fractalSettings.general, fractalSettings.perturb, *reference, table.get());
    }
    template<Number Num>
    std::unique_ptr<MB2RenderData<Num>>
    MB2RenderData<Num>::generateNew(ParallelRenderState &state, const FractalSettings &frt, dex dcMax, int exp10,
                                  uint64_t refInitialCapacity,
                                  std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                  std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) {
        return std::make_unique<MB2RenderData>(state, frt, dcMax, exp10, refInitialCapacity, 0, std::move(actionPerRefCalcIteration),
                             std::move(actionPerCreatingTableIteration), false);
    }

    template<Number Num>
    void MB2RenderData<Num>::translate(const float logZoom, const dex dcMax, const FrtPerturbSettings ptbSettings,
                                     const fixed_point_complex_i1 &newCenter) {
        if (lastCreationResult != Reference::CreationResult::SUCCESS) {
            // try to use incomplete reference
            MessageBox(nullptr, "Please do not try to use incomplete Reference.", "Warning", MB_OK | MB_ICONWARNING);
        } else {
            const int exp10 = logZoomToExp10(logZoom);
            fixed_point_complex_i1 center = newCenter.create_variant(exp10);
            const fixed_point_complex_i1 refCenter = reference->center.create_variant(exp10);
            fixed_point_complex_i1::sub(center, center, refCenter);

            perturbator->offR = center.get_real().dex_value();
            perturbator->offI = center.get_imag().dex_value();
            perturbator->dcMax = dcMax;

            fractalSettings.perturb = std::move(ptbSettings);
            fractalSettings.general.logZoom = logZoom;

            applyAutoMaxIteration();
        }
    }
    template<Number Num>
    void MB2RenderData<Num>::applyAutoMaxIteration() {
        auto &ptbSettings = fractalSettings.perturb;
        if (ptbSettings.autoMaxIteration) {
            ptbSettings.maxIteration = std::max(ptbSettings.maxIteration, reference->longestPeriod() * ptbSettings.autoIterationMultiplier);
        }
    }

    using LightMB2RenderData = MB2RenderData<double>;
    using DeepMB2RenderData = MB2RenderData<dex>;

} // namespace merutilm::rff2
