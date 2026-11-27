//
// Created by Merutilm on 2026-05-13.
//

#pragma once
#include <utility>

#include "../algorithm/MB2Perturbator.h"
#include "../algorithm/MB2ReferenceGenerator.hpp"
#include "../algorithm/RFFResult.hpp"
#include "../app/FnListeners.hpp"
#include "../settings/FractalSettings.h"
#include "ReferenceData.hpp"
namespace merutilm::rff2 {

    struct MB2RenderDataBase {

        ParallelRenderState &state;
        FractalSettings fractalSettings;
        bool computeShaderUsed;
        ApproxTableBuffer &buffer;

        explicit MB2RenderDataBase(ParallelRenderState &state, FractalSettings frt, bool computeShaderUsed,
                                   ApproxTableBuffer &buffer) :
            state(state), fractalSettings(std::move(frt)), computeShaderUsed(computeShaderUsed), buffer(buffer) {}

        virtual ~MB2RenderDataBase() = default;

        [[nodiscard]] virtual ReferenceDataBase *getReference() const = 0;
        [[nodiscard]] virtual MB2PerturbatorBase *getPerturbator() const = 0;

        virtual RFFResult regenerateApprox(FnListeners::FnCreatingTableW &&fnCreatingTable) = 0;

        virtual RFFResult translate(double logZoom, dex dcMax, const FrtPerturbSettings &ptbSettings,
                               const fixed_point_complex &newCenter) = 0;
    };

    template<Number Num>
    struct MB2RenderData final : MB2RenderDataBase {

        std::unique_ptr<ReferenceData<Num>> reference;
        std::unique_ptr<MPAData<Num>> table;
        std::unique_ptr<MB2Perturbator<Num>> perturbator;

        using MB2RenderDataBase::MB2RenderDataBase;

        template<FnListeners::FnRefCalc FnRefCalc,
             FnListeners::FnCreatingTable FnCreatingTable>
        static RFFResult createRenderData(ParallelRenderState &state, const FractalSettings &frt, bool computeShaderUsed,
                               ApproxTableBuffer &buffer, dex dcMax, int64_t exp10, uint64_t refInitialCapacity,
                               FnRefCalc &&fnRefCalc,
                               FnCreatingTable &&fnCreatingTable, std::unique_ptr<MB2RenderDataBase> &result);

        RFFResult regenerateApprox(FnListeners::FnCreatingTableW &&fnCreatingTable) override;

        [[nodiscard]] ReferenceDataBase *getReference() const override { return reference.get(); }

        [[nodiscard]] MB2Perturbator<Num> *getPerturbator() const override { return perturbator.get(); }

        [[nodiscard]] RFFResult translate(double logZoom, dex dcMax, const FrtPerturbSettings &ptbSettings,
                       const fixed_point_complex &newCenter) override;

        void applyAutoMaxIteration();
    };


    template<Number Num>
    template<FnListeners::FnRefCalc FnRefCalc,
             FnListeners::FnCreatingTable FnCreatingTable>
    RFFResult MB2RenderData<Num>::createRenderData(ParallelRenderState &state, const FractalSettings &frt, const bool computeShaderUsed,
                               ApproxTableBuffer &buffer, const dex dcMax, const int64_t exp10, const uint64_t refInitialCapacity,
                               FnRefCalc &&fnRefCalc,
                               FnCreatingTable &&fnCreatingTable, std::unique_ptr<MB2RenderDataBase> &result) {
        result = std::make_unique<MB2RenderData>(state, frt, computeShaderUsed, buffer);
        MB2RenderData &instance = dynamic_cast<MB2RenderData &>(*result);
        MB2ReferenceGenerator<Num, FnRefCalc> generator(state, frt.general, frt.reference, exp10, refInitialCapacity,
                                                               dcMax, {}, std::move(fnRefCalc));

        if (const RFFResult r = generator.generate(); r != RFFResult::SUCCESS) return r;

        instance.reference = std::move(generator).extract();
        instance.applyAutoMaxIteration();

        return instance.regenerateApprox(std::move(fnCreatingTable));
    }

    template<Number Num>
    RFFResult MB2RenderData<Num>::regenerateApprox(FnListeners::FnCreatingTableW &&fnCreatingTable) {
        if (!reference) return RFFResult::ERROR_NULL;

        MB2MPAGenerator generator(state, *reference, buffer, fractalSettings.general, fractalSettings.mpa,
                                                computeShaderUsed, Num(reference->dcMax), std::move(fnCreatingTable));
        generator.generate();
        table = std::move(generator).extract();
        complex<dex> off = perturbator ? perturbator->off : complex<dex>::ZERO;
        dex dcMax = perturbator ? perturbator->dcMax : reference->dcMax;
        perturbator = std::make_unique<MB2Perturbator<Num>>(
                state, reference->dcMax, fractalSettings.general, fractalSettings.perturb,
                *reference, dynamic_cast<MPAData<Num> &>(*table));
        perturbator->off = off;
        perturbator->dcMax = dcMax;
        return state.interruptRequested() ? RFFResult::TERMINATED : RFFResult::SUCCESS;
    }


    template<Number Num>
    RFFResult MB2RenderData<Num>::translate(const double logZoom, const dex dcMax, const FrtPerturbSettings &ptbSettings,
                                       const fixed_point_complex &newCenter) {
        if (!reference || !perturbator || !table) {
            // try to use incomplete reference
            return RFFResult::ERROR_NULL;
        }


        const int64_t exp10 = Perturbator::getExp10WithSettings(reference->refSettings, logZoom);
        fixed_point_complex center = newCenter.create_variant(exp10);
        const fixed_point_complex refCenter = reference->c.create_variant(exp10);
        fixed_point_complex::sub(center, center, refCenter);

        const dex distance = static_cast<complex<dex>>(center).norm();

        perturbator->off = {static_cast<dex>(center.get_real()), static_cast<dex>(center.get_imag())};
        perturbator->dcMax = dcMax + distance;

        fractalSettings.perturb = ptbSettings;
        fractalSettings.general.logZoom = logZoom;

        applyAutoMaxIteration();
        return RFFResult::SUCCESS;
    }

    template<Number Num>
    void MB2RenderData<Num>::applyAutoMaxIteration() {
        auto &ptbSettings = fractalSettings.perturb;
        if (ptbSettings.autoMaxIteration) {
            ptbSettings.maxIteration =
                    std::max(ptbSettings.maxIteration,
                             static_cast<itt::ptb>(reference->calcPeriod()) * ptbSettings.autoIterationMultiplier);
        }
    }

    using FloatMB2RenderData = MB2RenderData<float>;
    using DoubleMB2RenderData = MB2RenderData<double>;

    using FexMB2RenderData = MB2RenderData<fex>;
    using DexMB2RenderData = MB2RenderData<dex>;

} // namespace merutilm::rff2
