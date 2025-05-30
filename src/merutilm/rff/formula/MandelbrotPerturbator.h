//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "MandelbrotReference.h"
#include "Perturbator.h"
#include "../mrthy/MPATable.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/CalculationSettings.h"

struct MandelbrotPerturbator : public Perturbator{

    ParallelRenderState &state;
    const CalculationSettings calc;

    explicit MandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings& calculationSettings);

    ~MandelbrotPerturbator() override = default;

    virtual const MandelbrotReference *getReference() const = 0;

    const CalculationSettings &getCalculationSettings() const;

    virtual double_exp getDcMaxAsDoubleExp() const = 0;

    virtual double iterate(const double_exp &dcr, const double_exp &dci) const = 0;


};



