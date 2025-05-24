//
// Created by Merutilm on 2025-05-18.
//

#include "DeepMPATable.h"

#include <execution>

#include "../calc/double_exp.h"


DeepMPATable::DeepMPATable(const ParallelRenderState &state, const DeepMandelbrotReference &reference,
                           const MPASettings *mpaSettings, const double_exp &dcMax,
                           ApproxTableCache &tableRef,
                           std::function<void(uint64_t, double)> &&
                           actionPerCreatingTableIteration) : MPATable(state, reference, mpaSettings, dcMax, tableRef, std::move(actionPerCreatingTableIteration)) {

}
