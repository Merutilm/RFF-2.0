//
// Created by Merutilm on 2026-05-13.
//

#pragma once
#include <utility>
#include "../settings/FractalSettings.h"
#include "MB2Perturbator.h"
#include "MB2Reference.h"
#include "SeriesApproximationData.hpp"
namespace merutilm::rff2 {

    struct MB2RenderDataBase {

        ParallelRenderState &state;
        FractalSettings fractalSettings;
        Reference::CreationResult lastCreationResult = Reference::CreationResult::UNDEFINED;

        explicit MB2RenderDataBase(ParallelRenderState &state, FractalSettings frt) :
            state(state), fractalSettings(std::move(frt)) {}

        virtual ~MB2RenderDataBase() = default;

        [[nodiscard]] virtual MB2ReferenceBase *getReference() const = 0;
        [[nodiscard]] virtual MB2PerturbatorBase *getPerturbator() const = 0;

        virtual void translate(float logZoom, dex dcMax, const FrtPerturbSettings &ptbSettings,
                               const fixed_point_complex_i1 &newCenter) = 0;

        static int logZoomToExp10(const float logZoom) {
            return -static_cast<int>(logZoom) - Constants::Fractal::EXP10_ADDITION;
        }
    };

    template<Number Num>
    struct MB2RenderData final : MB2RenderDataBase {


        std::unique_ptr<MB2Reference<Num>> reference;
        std::unique_ptr<MPATableBase> table;
        std::unique_ptr<SeriesApproximationData> seriesApproxData;
        std::unique_ptr<MB2PerturbatorBase> perturbator;

        explicit MB2RenderData(ParallelRenderState &state, const FractalSettings &frt, dex dcMax, int exp10,
                               uint64_t refInitialCapacity, uint64_t fixedPeriod,
                               std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                               bool arbitraryPrecisionFPGBn);

        void createByDegree(dex dcMax, std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);


        [[nodiscard]] MB2ReferenceBase *getReference() const override { return reference.get(); }

        [[nodiscard]] MB2PerturbatorBase *getPerturbator() const override { return perturbator.get(); }

        void generateSeriesApproxTerms(dex dcMax);

        void translate(float logZoom, dex dcMax, const FrtPerturbSettings &ptbSettings,
                       const fixed_point_complex_i1 &newCenter) override;

        void applyAutoMaxIteration();
    };

    template<Number Num>
    MB2RenderData<Num>::MB2RenderData(ParallelRenderState &state, const FractalSettings &frt, const dex dcMax,
                                      const int exp10, const uint64_t refInitialCapacity, const uint64_t fixedPeriod,
                                      std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                      std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                                      const bool arbitraryPrecisionFPGBn) : MB2RenderDataBase(state, frt) {
        this->lastCreationResult = MB2Reference<Num>::generateReference(
                state, frt.general, frt.reference, exp10, refInitialCapacity, fixedPeriod, dcMax,
                arbitraryPrecisionFPGBn, std::move(actionPerRefCalcIteration), &reference);

        if (this->lastCreationResult != Reference::CreationResult::SUCCESS) {
            table = nullptr;
            perturbator = nullptr;
            return;
        }

        applyAutoMaxIteration();

        seriesApproxData = std::make_unique<SeriesApproximationData>();
        generateSeriesApproxTerms(dcMax);

        createByDegree(dcMax, std::move(actionPerCreatingTableIteration));
    }


    template<Number Num>
    void MB2RenderData<Num>::createByDegree(const dex dcMax,
                                            std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) {

         auto func = [this, dcMax, actionPerCreatingTableIteration = std::move(actionPerCreatingTableIteration)] <uint8_t D>() mutable {
             table = std::make_unique<MPATable<Num, D>>(state, *reference, &fractalSettings.mpa, Num(dcMax),
                                                           std::move(actionPerCreatingTableIteration));
             perturbator = std::make_unique<MB2Perturbator<Num, D>>(
                     state, dcMax, fractalSettings.general, fractalSettings.perturb, *seriesApproxData, *reference,
                     dynamic_cast<MPATable<Num, D> *>(table.get()));
         };


        switch (fractalSettings.mpa.maxDegree) {
            using enum FrtMPADegree;
            case P1_STANDARD: {
                func.template operator()<static_cast<uint8_t>(P1_STANDARD)>();
                break;
            }
            case P2: {
                func.template operator()<static_cast<uint8_t>(P2)>();
                break;
            }
            case P4: {
                func.template operator()<static_cast<uint8_t>(P4)>();
                break;
            }
            case P8: {
                func.template operator()<static_cast<uint8_t>(P8)>();
                break;
            }
            case P16: {
                func.template operator()<static_cast<uint8_t>(P16)>();
                break;
            }
            case P32: {
                func.template operator()<static_cast<uint8_t>(P32)>();
                break;
            }
            case P64: {
                func.template operator()<static_cast<uint8_t>(P64)>();
                break;
            }
            case P128: {
                func.template operator()<static_cast<uint8_t>(P128)>();
                break;
            }
            default:
                throw vkh::exception_invalid_args("unsupported parameter");
        }
    }


    template<Number Num>
    void MB2RenderData<Num>::generateSeriesApproxTerms(const dex dcMax) {
#ifdef ENABLE_SERIES_APPROXIMATION
        // GENERATE SA (DEPRECATED)
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
            dex rSum = dex::ZERO;
            dex dcMaxNs = dcMax;
            for (uint32_t currExp = 0; currExp < terms.size(); ++currExp) {
                if (currExp < fractalSettings.sa.appliedTermsCount) {
                    lSum += termsTemp[currExp] * dcMaxNs;
                } else {
                    rSum += termsTemp[currExp].norm_approx() * dcMaxNs;
                }
                dcMaxNs *= dcMax;
            }
            if (lSum.norm_approx() * epsilon < rSum)
                break;

            std::copy_n(termsTemp.begin(), terms.size(), terms.begin());
        }

        seriesApproxData->skippedIterations = skip;
#endif
    }

    template<Number Num>
    void MB2RenderData<Num>::translate(const float logZoom, const dex dcMax, const FrtPerturbSettings &ptbSettings,
                                       const fixed_point_complex_i1 &newCenter) {
        if (lastCreationResult != Reference::CreationResult::SUCCESS) {
            // try to use incomplete reference
            vkh::logger::log_warn("Please do not try to use incomplete Reference.");
        } else {
            const int exp10 = logZoomToExp10(logZoom);
            fixed_point_complex_i1 center = newCenter.create_variant(exp10);
            const fixed_point_complex_i1 refCenter = reference->center.create_variant(exp10);
            fixed_point_complex_i1::sub(center, center, refCenter);

            perturbator->off = {center.get_real().dex_value(), center.get_imag().dex_value()};
            perturbator->dcMax = dcMax;

            fractalSettings.perturb = ptbSettings;
            fractalSettings.general.logZoom = logZoom;

            applyAutoMaxIteration();
            generateSeriesApproxTerms(dcMax);
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

    using LightMB2RenderData = MB2RenderData<double>;
    using DeepMB2RenderData = MB2RenderData<dex>;

} // namespace merutilm::rff2
