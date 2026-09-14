//
// Created by Merutilm on 2026-05-13.
//

#pragma once
#include <utility>
#include "../app/FnListeners.hpp"
#include "../settings/FractalSettings.h"
#include "MB2Perturbator.h"
#include "MB2Reference.h"
#include "SeriesApproximationData.hpp"
namespace merutilm::rff2 {

    struct MB2RenderDataBase {

        ParallelRenderState &state;
        FractalSettings fractalSettings;
        bool computeShaderUsed;
        std::unique_ptr<ApproxTableCacheBase> *cache;
        Reference::CreationResult lastCreationResult = Reference::CreationResult::UNDEFINED;

        explicit MB2RenderDataBase(ParallelRenderState &state, FractalSettings frt, bool computeShaderUsed,
                                   std::unique_ptr<ApproxTableCacheBase> &cache) :
            state(state), fractalSettings(std::move(frt)), computeShaderUsed(computeShaderUsed), cache(&cache) {}

        virtual ~MB2RenderDataBase() = default;

        [[nodiscard]] virtual MB2ReferenceBase *getReference() const = 0;
        [[nodiscard]] virtual MB2PerturbatorBase *getPerturbator() const = 0;

        virtual void translate(float logZoom, dex dcMax, const FrtPerturbSettings &ptbSettings,
                               const fixed_point_complex &newCenter,
                               FnListeners::FnSeriesApproxW &&fnSeriesApprox) = 0;

        static int logZoomToExp10(const float logZoom) {
            return -static_cast<int>(logZoom) - Constants::Fractal::EXP10_ADDITION;
        }
    };

    template<Number Num>
    struct MB2RenderData final : MB2RenderDataBase {


        std::unique_ptr<MB2Reference<Num>> reference;
        std::unique_ptr<MPATable<Num>> table;
        std::unique_ptr<SeriesApproximationData> seriesApproxData;
        std::unique_ptr<MB2Perturbator<Num>> perturbator;

        template<FnListeners::FnRefCalc FnRefCalc, FnListeners::FnSeriesApprox FnSeriesApprox,
                 FnListeners::FnCreatingTable FnCreatingTable>
        explicit MB2RenderData(vkh::Core &core, ParallelRenderState &state, const FractalSettings &frt,
                               bool computeShaderUsed, std::unique_ptr<ApproxTableCacheBase> &cache, dex dcMax, int exp10, uint64_t refInitialCapacity,
                               FnRefCalc &&fnRefCalc, FnSeriesApprox &&fnSeriesApprox,
                               FnCreatingTable &&fnCreatingTable);


        [[nodiscard]] MB2ReferenceBase *getReference() const override { return reference.get(); }

        [[nodiscard]] MB2Perturbator<Num> *getPerturbator() const override { return perturbator.get(); }

        template<FnListeners::FnSeriesApprox FnSeriesApprox>
        void generateSeriesApproxTerms(dex dcMax, FnSeriesApprox &&fnSeriesApprox);

        void translate(float logZoom, dex dcMax, const FrtPerturbSettings &ptbSettings,
                       const fixed_point_complex &newCenter,
                       FnListeners::FnSeriesApproxW &&fnSeriesApprox) override;

        void applyAutoMaxIteration();
    };


    template<Number Num>
    template<FnListeners::FnRefCalc FnRefCalc, FnListeners::FnSeriesApprox FnSeriesApprox,
             FnListeners::FnCreatingTable FnCreatingTable>
    MB2RenderData<Num>::MB2RenderData(vkh::Core &core, ParallelRenderState &state, const FractalSettings &frt,
                                      const bool computeShaderUsed, std::unique_ptr<ApproxTableCacheBase> &cache,
                                      const dex dcMax, const int exp10, const uint64_t refInitialCapacity,
                                      FnRefCalc &&fnRefCalc, FnSeriesApprox &&fnSeriesApprox,
                                      FnCreatingTable &&fnCreatingTable) :
        MB2RenderDataBase(state, frt, computeShaderUsed, cache) {

        this->lastCreationResult = MB2Reference<Num>::generateReference(
                state, frt.general, frt.reference, exp10, refInitialCapacity, dcMax, fnRefCalc, &reference);

        if (this->lastCreationResult != Reference::CreationResult::SUCCESS) {
            table = nullptr;
            perturbator = nullptr;
            return;
        }

        applyAutoMaxIteration();

        seriesApproxData = std::make_unique<SeriesApproximationData>();
        generateSeriesApproxTerms(dcMax, std::forward<FnSeriesApprox>(fnSeriesApprox));

        if (!dynamic_cast<ApproxTableCache<Num> *>(cache.get()))
            cache = std::make_unique<ApproxTableCache<Num>>(core);


        table = std::make_unique<MPATable<Num>>(state, *reference, cache, fractalSettings.general, fractalSettings.mpa,
                                                computeShaderUsed, Num(dcMax),
                                                std::forward<FnCreatingTable>(fnCreatingTable));
        perturbator = std::make_unique<MB2Perturbator<Num>>(state, dcMax, fractalSettings.general, fractalSettings.sa,
                                                            fractalSettings.perturb, *seriesApproxData, *reference,
                                                            dynamic_cast<MPATable<Num> *>(table.get()));
    }


    template<Number Num>
    template<FnListeners::FnSeriesApprox FnSeriesApprox>
    void MB2RenderData<Num>::generateSeriesApproxTerms(const dex dcMax, FnSeriesApprox &&fnSeriesApprox) {
        if (!fractalSettings.sa.use)
            return;

        auto &terms = seriesApproxData->terms;
        terms.clear();
        terms.resize(fractalSettings.sa.appliedTermsCount + fractalSettings.sa.validatedTermsCount);
        auto termsTemp = std::vector<complex<dex>>(terms.size());

        const dex epsilon{pow(10, fractalSettings.sa.epsilonPower)};
        std::ranges::fill(terms, complex<dex>::ZERO);
        constexpr dex two{2};
        uint64_t skip = 0;


        for (skip = 0; skip < reference->longestPeriod(); ++skip) {
            if (state.interruptRequested())
                return;

            fnSeriesApprox(skip, static_cast<double>(skip) / reference->longestPeriod());

            const complex<Num> zn = reference->orbit(skip);
            const complex z2 = {dex(zn.re) * two, dex(zn.im) * two};

            // next term
            for (uint32_t currExp = 0; currExp < terms.size(); ++currExp) {

                complex sum = complex<dex>::ZERO;
                for (uint32_t j = 0; j < currExp; ++j) {
                    const complex<dex> tn = terms[j];
                    const complex<dex> ttn = terms[currExp - j - 1];
                    sum += tn * ttn;
                }

                auto coef = z2 * terms[currExp] + (currExp == 0 ? complex<dex>::ONE : sum);
                termsTemp[currExp] = coef.try_normalized_value();
            }


            // validation

            complex lSum = complex<dex>::ZERO;
            dex lSumMag = dex::ZERO;
            dex rSumMag = dex::ZERO;
            dex dcMaxNs = dcMax;
            for (uint32_t currExp = 0; currExp < terms.size(); ++currExp) {
                if (currExp < fractalSettings.sa.appliedTermsCount) {
                    lSum += termsTemp[currExp] * dcMaxNs;
                    lSumMag += termsTemp[currExp].norm_approx() * dcMaxNs;
                } else {
                    rSumMag += termsTemp[currExp].norm_approx() * dcMaxNs;
                }

                dcMaxNs *= dcMax;
            }

            const complex znDex{dex(zn.re), dex(zn.im)};
            const complex<dex> point = lSum + znDex;

            if (lSumMag * epsilon < rSumMag ||
                point.norm_sqr() > dex(fractalSettings.general.bailout * fractalSettings.general.bailout))
                break;

            std::copy_n(termsTemp.begin(), terms.size(), terms.begin());
        }

        seriesApproxData->skippedIterations = skip;
    }


    template<Number Num>
    void MB2RenderData<Num>::translate(const float logZoom, const dex dcMax, const FrtPerturbSettings &ptbSettings,
                                       const fixed_point_complex &newCenter,
                                       FnListeners::FnSeriesApproxW &&fnSeriesApprox) {
        if (lastCreationResult != Reference::CreationResult::SUCCESS) {
            // try to use incomplete reference
            vkh::logger::log_err("Please do not try to use incomplete Reference.");
        } else {
            const int exp10 = logZoomToExp10(logZoom);
            fixed_point_complex center = newCenter.create_variant(exp10);
            const fixed_point_complex refCenter = reference->center.create_variant(exp10);
            fixed_point_complex::sub(center, center, refCenter);

            perturbator->off = {static_cast<dex>(center.get_real()), static_cast<dex>(center.get_imag())};
            perturbator->dcMax = dcMax;

            fractalSettings.perturb = ptbSettings;
            fractalSettings.general.logZoom = logZoom;

            applyAutoMaxIteration();
            generateSeriesApproxTerms(dcMax, std::move(fnSeriesApprox));
        }
    }

    template<Number Num>
    void MB2RenderData<Num>::applyAutoMaxIteration() {
        auto &ptbSettings = fractalSettings.perturb;
        if (ptbSettings.autoMaxIteration) {
            ptbSettings.maxIteration = std::max(ptbSettings.maxIteration,
                                                reference->longestPeriod() * ptbSettings.autoIterationMultiplier);
        }
    }

    using FloatMB2RenderData = MB2RenderData<float>;
    using DoubleMB2RenderData = MB2RenderData<double>;

    using FexMB2RenderData = MB2RenderData<fex>;
    using DexMB2RenderData = MB2RenderData<dex>;

} // namespace merutilm::rff2
