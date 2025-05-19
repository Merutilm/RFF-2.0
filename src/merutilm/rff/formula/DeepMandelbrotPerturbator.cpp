//
// Created by Merutilm on 2025-05-18.
//

#include "DeepMandelbrotPerturbator.h"

#include <iostream>

#include "../calc/double_exp_math.h"

DeepMandelbrotPerturbator::DeepMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc,
                                                     const double_exp &dcMax, const int exp10,
                                                     const uint64_t initialPeriod,
                                                     std::vector<std::vector<DeepPA> > &&previousAllocatedTable,
                                                     std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                     std::function<void(uint64_t, double)> &&
                                                     actionPerCreatingTableIteration) : DeepMandelbrotPerturbator(
    state, calc, dcMax, exp10, initialPeriod, std::move(previousAllocatedTable), std::move(actionPerRefCalcIteration),
    std::move(actionPerCreatingTableIteration), false) {
}

DeepMandelbrotPerturbator::DeepMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc,
                                                     const double_exp &dcMax, const int exp10,
                                                     const uint64_t initialPeriod,
                                                     std::vector<std::vector<DeepPA> > &&previousAllocatedTable,
                                                     std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                     std::function<void(uint64_t, double)> &&
                                                     actionPerCreatingTableIteration,
                                                     const bool arbitraryPrecisionFPGBn) : DeepMandelbrotPerturbator(
    state, calc, dcMax, exp10, initialPeriod, std::move(previousAllocatedTable), std::move(actionPerRefCalcIteration),
    std::move(actionPerCreatingTableIteration), arbitraryPrecisionFPGBn, nullptr, nullptr, double_exp::DEX_ZERO,
    double_exp::DEX_ZERO) {
}

DeepMandelbrotPerturbator::DeepMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc,
                                                     const double_exp &dcMax, const int exp10,
                                                     const uint64_t initialPeriod,
                                                     std::vector<std::vector<DeepPA> > &&previousAllocatedTable,
                                                     std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                     std::function<void(uint64_t, double)> &&
                                                     actionPerCreatingTableIteration,
                                                     const bool arbitraryPrecisionFPGBn,
                                                     std::unique_ptr<DeepMandelbrotReference> reusedReference,
                                                     std::unique_ptr<DeepMPATable> reusedTable,
                                                     const double_exp &offR,
                                                     const double_exp &offI) : MandelbrotPerturbator(state, calc),
                                                                               dcMax(dcMax), offR(offR), offI(offI) {
    if (reusedReference == nullptr) {
        reference = DeepMandelbrotReference::createReference(state, calc, exp10, initialPeriod, dcMax,
                                                             arbitraryPrecisionFPGBn,
                                                             std::move(actionPerRefCalcIteration));
    } else {
        reference = std::move(reusedReference);
    }

    if (reusedTable == nullptr) {
        table = std::make_unique<DeepMPATable>(state, reference.get(), &calc.mpaSettings, dcMax,
                                               std::move(previousAllocatedTable),
                                               std::move(actionPerCreatingTableIteration));
    } else {
        table = std::move(reusedTable);
    }
}


double DeepMandelbrotPerturbator::iterate(const double_exp &dcr, const double_exp &dci) const {
    if (state.interruptRequested()) return 0.0;

    const double_exp dcr1 = dcr + offR + dcMax / RFF::Render::INTENTIONAL_ERROR_DCPTB;
    const double_exp dci1 = dci + offI + dcMax / RFF::Render::INTENTIONAL_ERROR_DCPTB;

    uint64_t iteration = 0;
    uint64_t refIteration = 0;
    int absIteration = 0;
    const uint64_t maxRefIteration = reference->longestPeriod();
    double_exp dzr = double_exp::DEX_ZERO;
    double_exp dzi = double_exp::DEX_ZERO;
    double_exp zr = double_exp::DEX_ZERO;
    double_exp zi = double_exp::DEX_ZERO;
    double cd = 0;
    double pd = cd;
    const bool isAbs = calc.absoluteIterationMode;
    const uint64_t maxIteration = calc.maxIteration;
    const float bailout = calc.bailout;
    const float bailout2 = bailout * bailout;
    auto temps = std::array<double_exp, 4>();


    while (iteration < maxIteration) {
        if (table != nullptr) {
            if (const DeepPA *mpaPtr = table->lookup(refIteration, dzr, dzi, temps); mpaPtr != nullptr) {
                const DeepPA &mpa = *mpaPtr;

                double_exp::dex_mul(&temps[0], mpa.anr, dzr);
                double_exp::dex_mul(&temps[1], mpa.ani, dzi);
                double_exp::dex_sub(&temps[0], temps[0], temps[1]);
                double_exp::dex_mul(&temps[1], mpa.bnr, dcr1);
                double_exp::dex_add(&temps[0], temps[0], temps[1]);
                double_exp::dex_mul(&temps[1], mpa.bni, dci1);
                double_exp::dex_sub(&temps[0], temps[0], temps[1]);
                double_exp::dex_mul(&temps[1], mpa.anr, dzi);
                double_exp::dex_mul(&temps[2], mpa.ani, dzr);
                double_exp::dex_add(&temps[1], temps[1], temps[2]);
                double_exp::dex_mul(&temps[2], mpa.bnr, dci1);
                double_exp::dex_add(&temps[1], temps[1], temps[2]);
                double_exp::dex_mul(&temps[2], mpa.bni, dcr1);
                double_exp::dex_cpy(&dzr, temps[0]);
                double_exp::dex_add(&dzi, temps[1], temps[2]);

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
                double_exp::dex_cpy(&temps[0], dzr);
                double_exp::dex_cpy(&temps[1], dzi);
            } else {
                double_exp::dex_cpy(&temps[0], reference->refReal[index]);
                double_exp::dex_cpy(&temps[1], reference->refImag[index]);
                double_exp::dex_mul_2exp(&temps[0], temps[0], 1);
                double_exp::dex_mul_2exp(&temps[1], temps[1], 1);
                double_exp::dex_add(&temps[0], temps[0], dzr);
                double_exp::dex_add(&temps[1], temps[1], dzi);
            }


            if (temps[0].sgn() == 0 && temps[1].sgn() == 0) {
                double_exp::dex_cpy(&dzr, dcr1);
                double_exp::dex_cpy(&dzi, dci1);
            } else {
                double_exp::dex_mul(&temps[2], temps[0], dzr);
                double_exp::dex_mul(&temps[3], temps[1], dzi);
                double_exp::dex_sub(&temps[3], temps[2], temps[3]);
                double_exp::dex_mul(&temps[2], temps[0], dzi);
                double_exp::dex_mul(&temps[0], temps[1], dzr);
                double_exp::dex_add(&temps[2], temps[2], temps[0]);
                double_exp::dex_add(&dzr, temps[3], dcr1);
                double_exp::dex_add(&dzi, temps[2], dci1);
            }


            ++refIteration;
            ++iteration;
            ++absIteration;
        }

        const uint64_t index = ArrayCompressor::compress(reference->compressor, refIteration);
        double_exp::dex_add(&zr, reference->refReal[index], dzr);
        double_exp::dex_add(&zi, reference->refImag[index], dzi);
        pd = cd;

        const auto zr0 = static_cast<double>(zr);
        const auto zi0 = static_cast<double>(zi);
        const auto dzr0 = static_cast<double>(dzr);
        const auto dzi0 = static_cast<double>(dzi);
        cd = zr0 * zr0 + zi0 * zi0;

        if (refIteration == maxRefIteration || cd < dzr0 * dzr0 + dzi0 * dzi0) {
            refIteration = 0;
            double_exp::dex_cpy(&dzr, zr);
            double_exp::dex_cpy(&dzi, zi);
        }

        if (dzr.normalize_required()) double_exp::normalize(&dzr);
        if (dzi.normalize_required()) double_exp::normalize(&dzi);
        if (cd > bailout2) break;
        if (absIteration % RFF::Render::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) return 0.0;
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
    const CalculationSettings &calc, const double_exp &dcMax,
    const int exp10) {
    double_exp offR = double_exp::DEX_ZERO;
    double_exp offI = double_exp::DEX_ZERO;
    uint64_t longestPeriod = 1;
    std::unique_ptr<DeepMandelbrotReference> reusedReference = nullptr;

    if (reference == RFF::NullPointer::PROCESS_TERMINATED_REFERENCE) {
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
                                                       std::vector<std::vector<DeepPA> >(),
                                                       [](uint64_t) {
                                                           //no action because the reference is already declared
                                                       }, [](uint64_t, double) {
                                                           //same reason
                                                       }, false, std::move(reusedReference),
                                                       std::move(table), offR, offI);
}
