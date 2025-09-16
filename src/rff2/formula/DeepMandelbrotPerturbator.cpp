//
// Created by Merutilm on 2025-05-18.
//

#include "DeepMandelbrotPerturbator.h"

namespace merutilm::rff2 {


    DeepMandelbrotPerturbator::DeepMandelbrotPerturbator(ParallelRenderState &state, const FractalAttribute &calc,
                                                         const dex &dcMax, const int exp10,
                                                         const uint64_t initialPeriod,
                                                         ApproxTableCache &tableRef,
                                                         std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                         std::function<void(uint64_t, double)> &&
                                                         actionPerCreatingTableIteration,
                                                         const bool arbitraryPrecisionFPGBn,
                                                         std::unique_ptr<DeepMandelbrotReference> reusedReference,
                                                         std::unique_ptr<DeepMPATable> reusedTable,
                                                         const dex &offR,
                                                         const dex &offI) : MandelbrotPerturbator(state, calc),
                                                                                   dcMax(dcMax), offR(offR), offI(offI) {
        if (reusedReference == nullptr) {
            reference = DeepMandelbrotReference::createReference(state, calc, exp10, initialPeriod, dcMax,
                                                                 arbitraryPrecisionFPGBn,
                                                                 std::move(actionPerRefCalcIteration));
        } else {
            reference = std::move(reusedReference);
        }

        if (reference == Constants::NullPointer::PROCESS_TERMINATED_REFERENCE) {
            return;
        }

        if (reusedTable == nullptr) {
            table = std::make_unique<DeepMPATable>(state, *reference, &calc.mpaAttribute, dcMax,
                                                   tableRef,
                                                   std::move(actionPerCreatingTableIteration));
        } else {
            table = std::move(reusedTable);
        }
    }


    double DeepMandelbrotPerturbator::iterate(const dex &dcr, const dex &dci) const {
        if (state.interruptRequested()) return 0.0;

        const dex dcr1 = dcr + offR;
        const dex dci1 = dci + offI;

        uint64_t iteration = 0;
        uint64_t refIteration = 0;
        int absIteration = 0;
        const uint64_t maxRefIteration = reference->longestPeriod();
        dex dzr = dex::ZERO;
        dex dzi = dex::ZERO;
        dex zr = dex::ZERO;
        dex zi = dex::ZERO;


        const dex zrMin = dex::value(-2);
        const dex zrMax = dex::value(0.25);

        double cd = 0;
        double pd = cd;
        const bool isAbs = calc.absoluteIterationMode;
        const uint64_t maxIteration = calc.maxIteration;
        const float bailout = calc.bailout;
        const float bailout2 = bailout * bailout;
        auto temps = std::array<dex, 4>();


        while (iteration < maxIteration) {
            if (table != nullptr) {
                if (const DeepPA *mpaPtr = table->lookup(refIteration, dzr, dzi, temps); mpaPtr != nullptr) {
                    const DeepPA &mpa = *mpaPtr;

                    dex::mul(&temps[0], mpa.anr, dzr);
                    dex::mul(&temps[1], mpa.ani, dzi);
                    dex::sub(&temps[0], temps[0], temps[1]);
                    dex::mul(&temps[1], mpa.bnr, dcr1);
                    dex::add(&temps[0], temps[0], temps[1]);
                    dex::mul(&temps[1], mpa.bni, dci1);
                    dex::sub(&temps[0], temps[0], temps[1]);
                    dex::mul(&temps[1], mpa.anr, dzi);
                    dex::mul(&temps[2], mpa.ani, dzr);
                    dex::add(&temps[1], temps[1], temps[2]);
                    dex::mul(&temps[2], mpa.bnr, dci1);
                    dex::add(&temps[1], temps[1], temps[2]);
                    dex::mul(&temps[2], mpa.bni, dcr1);
                    dex::cpy(&dzr, temps[0]);
                    dex::add(&dzi, temps[1], temps[2]);

                    iteration += mpa.skip;
                    refIteration += mpa.skip;
                    ++absIteration;

                    if (iteration >= maxIteration) {
                        return static_cast<double>(isAbs ? absIteration : maxIteration);
                    }
                    continue;
                }
            }


            if (refIteration != maxRefIteration) {
                if (const uint64_t index = ArrayCompressor::compress(reference->compressor, refIteration);
                    index == 0) {
                    dex::cpy(&temps[0], dzr);
                    dex::cpy(&temps[1], dzi);
                    } else {
                        dex::cpy(&temps[0], reference->refReal[index]);
                        dex::cpy(&temps[1], reference->refImag[index]);
                        dex::mul_2exp(&temps[0], temps[0], 1);
                        dex::mul_2exp(&temps[1], temps[1], 1);
                        dex::add(&temps[0], temps[0], dzr);
                        dex::add(&temps[1], temps[1], dzi);
                    }


                if (temps[0].sgn() == 0 && temps[1].sgn() == 0) {
                    dex::cpy(&dzr, dcr1);
                    dex::cpy(&dzi, dci1);
                } else {
                    dex::mul(&temps[2], temps[0], dzr);
                    dex::mul(&temps[3], temps[1], dzi);
                    dex::sub(&temps[3], temps[2], temps[3]);
                    dex::mul(&temps[2], temps[0], dzi);
                    dex::mul(&temps[0], temps[1], dzr);
                    dex::add(&temps[2], temps[2], temps[0]);
                    dex::add(&dzr, temps[3], dcr1);
                    dex::add(&dzi, temps[2], dci1);
                }


                ++refIteration;
                ++iteration;
                ++absIteration;
            }

            const uint64_t index = ArrayCompressor::compress(reference->compressor, refIteration);
            dex::add(&zr, reference->refReal[index], dzr);
            dex::add(&zi, reference->refImag[index], dzi);


            dex::sub(&temps[0], zr, zrMin);
            dex::sub(&temps[1], zrMax, zr);

            if (zi.sgn() == 0 && temps[0].sgn() != -1 && temps[1].sgn() != -1) {
                //IT IS NOT SATISFIED MPA SKIP RADIUS CONDITION.
                //WHEN THE MAX ITERATION IS HIGH, REPEATS SEMI-INFINITELY.
                return static_cast<double>(maxIteration);
            }

            const auto zr0 = static_cast<double>(zr);
            const auto zi0 = static_cast<double>(zi);
            const auto dzr0 = static_cast<double>(dzr);
            const auto dzi0 = static_cast<double>(dzi);

            pd = cd;
            cd = zr0 * zr0 + zi0 * zi0;

            if (refIteration == maxRefIteration || cd < dzr0 * dzr0 + dzi0 * dzi0) {
                refIteration = 0;
                dex::cpy(&dzr, zr);
                dex::cpy(&dzi, zi);
            }

            dzr.try_normalize();
            dzi.try_normalize();
            if (cd > bailout2) break;
            if (absIteration % Constants::Fractal::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) return 0.0;
        }

        if (isAbs) {
            return absIteration;
        }

        if (iteration >= maxIteration) {
            return static_cast<double>(maxIteration);
        }

        const double fpd = sqrt(pd);
        const double fcd = sqrt(cd);

        return getDoubleValueIteration(iteration, fpd, fcd, calc.decimalizeIterationMethod, bailout);
    }


    std::unique_ptr<DeepMandelbrotPerturbator> DeepMandelbrotPerturbator::reuse(
        const FractalAttribute &calc, const dex &dcMax, ApproxTableCache &tableRef) {
        dex offR = dex::ZERO;
        dex offI = dex::ZERO;
        uint64_t longestPeriod = 1;
        std::unique_ptr<DeepMandelbrotReference> reusedReference = nullptr;

        const int exp10 = logZoomToExp10(calc.logZoom);

        if (reference == Constants::NullPointer::PROCESS_TERMINATED_REFERENCE) {
            //try to use process-terminated reference
            MessageBox(nullptr, "Please do not try to use PROCESS-TERMINATED Reference.", "Warning",
                       MB_OK | MB_ICONWARNING);
        } else {
            fp_complex_calculator centerOffset = calc.center.edit(exp10);
            centerOffset -= reference->center.edit(exp10);
            centerOffset.getReal().double_exp_value(&offR);
            centerOffset.getImag().double_exp_value(&offI);
            longestPeriod = reference->longestPeriod();
            reusedReference = std::move(reference);
        }


        return std::make_unique<DeepMandelbrotPerturbator>(state, calc, dcMax, exp10, longestPeriod,
                                                           tableRef,
                                                           [](uint64_t) {
                                                               //no action because the reference is already declared
                                                           }, [](uint64_t, double) {
                                                               //same reason
                                                           }, false, std::move(reusedReference),
                                                           std::move(table), offR, offI);
    }
}
