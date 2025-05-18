//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "MandelbrotReference.h"
#include "Perturbator.h"
#include "../mrtbrilliant/MPATable.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/CalculationSettings.h"

struct MandelbrotPerturbator : public Perturbator{

    ParallelRenderState &state;
    const CalculationSettings calc;

    explicit MandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings& calculationSettings);

    ~MandelbrotPerturbator() override = default;

    virtual const MandelbrotReference *getReference() const = 0;

    const CalculationSettings &getCalculationSettings() const;

    virtual double_exp getDcMaxExp() const = 0;

    virtual MPATable &getTable() const = 0;

    virtual double iterate(const double_exp &dcr, const double_exp &dci) const = 0;

    // template <typename N> requires std::is_same_v<N, double_exp> || std::is_same_v<N, double>
    // std::unique_ptr<MandelbrotPerturbator> reuse(
    // const CalculationSettings &calc, N dcMax,
    // int exp10);
};

// DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR
// DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR
// DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR
// DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR
// DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR  DEFINITION OF MANDELBROT PERTURBATOR



//
// template<typename N> requires std::is_same_v<N, double_exp> || std::is_same_v<N, double>
// std::unique_ptr<MandelbrotPerturbator> MandelbrotPerturbator::reuse(const CalculationSettings &calc, N dcMax, int exp10) {
//
// }

