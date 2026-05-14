//
// Created by Merutilm on 2026-05-13.
//

#pragma once
#include <utility>

#include "../mrthy/DeepMPATable.h"
#include "../mrthy/LightMPATable.h"
#include "../mrthy/LightPA.h"
#include "DeepMB2Perturbator.h"
#include "LightMB2Perturbator.h"
#include "LightMB2Reference.h"
namespace merutilm::rff2 {

    template<typename T>
    struct MB2Types;

    template<>
    struct MB2Types<LightPA> {
        using Reference = LightMB2Reference;
        using Table = LightMPATable;
        using Perturbator = LightMB2Perturbator;
    };

    template<>
    struct MB2Types<DeepPA> {
        using Reference = DeepMB2Reference;
        using Table = DeepMPATable;
        using Perturbator = DeepMB2Perturbator;
    };

    struct MB2RenderDataBase {


        explicit MB2RenderDataBase(FractalSettings frt) : fractalSettings(std::move(frt)) {}

        virtual ~MB2RenderDataBase() = default;

        FractalSettings fractalSettings;
        Reference::CreationResult lastCreationResult = Reference::CreationResult::UNDEFINED;

        [[nodiscard]] virtual MB2Reference *getReference() const = 0;
        [[nodiscard]] virtual MB2Perturbator *getPerturbator() const = 0;

        virtual void translate(float logZoom, dex dcMax, FrtPerturbSettings ptbSettings,
                               const fixed_point_complex_i1 &newCenter) = 0;

        static int logZoomToExp10(const float logZoom) {
            return -static_cast<int>(logZoom) - Constants::Fractal::EXP10_ADDITION;
        }
    };

    template<typename T>
    struct MB2RenderData final : MB2RenderDataBase {


        std::unique_ptr<typename MB2Types<T>::Reference> reference;
        std::unique_ptr<typename MB2Types<T>::Table> table;
        std::unique_ptr<typename MB2Types<T>::Perturbator> perturbator;

        explicit MB2RenderData(ParallelRenderState &state, const FractalSettings &frt, dex dcMax, int exp10,
                               uint64_t refInitialCapacity, uint64_t fixedPeriod,
                               std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                               bool arbitraryPrecisionFPGBn);

        static std::unique_ptr<MB2RenderData>
        generateNew(ParallelRenderState &state, const FractalSettings &frt, dex dcMax, int exp10,
                    uint64_t refInitialCapacity, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                    std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

        [[nodiscard]] MB2Reference *getReference() const override { return reference.get(); }

        [[nodiscard]] MB2Perturbator *getPerturbator() const override { return perturbator.get(); }

        void translate(float logZoom, dex dcMax, FrtPerturbSettings ptbSettings,
                       const fixed_point_complex_i1 &newCenter) override;

        void applyAutoMaxIteration();
    };

    template<typename T>
    MB2RenderData<T>::MB2RenderData(ParallelRenderState &state, const FractalSettings &frt, const dex dcMax,
                                    const int exp10, const uint64_t refInitialCapacity, const uint64_t fixedPeriod,
                                    std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                    std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                                    const bool arbitraryPrecisionFPGBn) : MB2RenderDataBase(frt) {
        this->lastCreationResult = MB2Types<T>::Reference::generateReference(
                state, frt.general, frt.reference, exp10, refInitialCapacity, fixedPeriod, dcMax,
                arbitraryPrecisionFPGBn, std::move(actionPerRefCalcIteration), &reference);

        if (this->lastCreationResult != Reference::CreationResult::SUCCESS) {
            table = nullptr;
            perturbator = nullptr;
            return;
        }

        applyAutoMaxIteration();
        table = std::make_unique<typename MB2Types<T>::Table>(state, *reference, &fractalSettings.mpa, dcMax,
                                                     std::move(actionPerCreatingTableIteration));
        perturbator = std::make_unique<typename MB2Types<T>::Perturbator>(state, dcMax, fractalSettings.general, fractalSettings.perturb, *reference, table.get());
    }
    template<typename T>
    std::unique_ptr<MB2RenderData<T>>
    MB2RenderData<T>::generateNew(ParallelRenderState &state, const FractalSettings &frt, dex dcMax, int exp10,
                                  uint64_t refInitialCapacity,
                                  std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                  std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) {
        return std::make_unique<MB2RenderData>(state, frt, dcMax, exp10, refInitialCapacity, 0, std::move(actionPerRefCalcIteration),
                             std::move(actionPerCreatingTableIteration), false);
    }

    template<typename T>
    void MB2RenderData<T>::translate(const float logZoom, const dex dcMax, const FrtPerturbSettings ptbSettings,
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
    template<typename T>
    void MB2RenderData<T>::applyAutoMaxIteration() {
        auto &ptbSettings = fractalSettings.perturb;
        if (ptbSettings.autoMaxIteration) {
            ptbSettings.maxIteration = std::max(ptbSettings.maxIteration, reference->longestPeriod() * ptbSettings.autoIterationMultiplier);
        }
    }

} // namespace merutilm::rff2
