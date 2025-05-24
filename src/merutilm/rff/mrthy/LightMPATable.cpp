//
// Created by Merutilm on 2025-05-11.
//

#include "LightMPATable.h"

#include <execution>



LightMPATable::LightMPATable(const ParallelRenderState &state, const LightMandelbrotReference &reference,
                             const MPASettings *mpaSettings, const double dcMax,
                             ApproxTableCache &tableRef,
                             std::function<void(uint64_t, double)> &&
                             actionPerCreatingTableIteration) : MPATable(state, reference, mpaSettings, dcMax, tableRef, std::move(actionPerCreatingTableIteration)){
}

