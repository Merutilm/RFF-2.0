//
// Created by Merutilm on 2025-05-18.
//

#include "MandelbrotReference.h"


merutilm::rff::MandelbrotReference::MandelbrotReference(fp_complex &&center, std::vector<ArrayCompressionTool> &&compressor,
                                         std::vector<uint64_t> &&period, fp_complex &&lastReference,
                                         fp_complex &&fpgBn) : center(std::move(center)),
                                                                    compressor(std::move(compressor)),
                                                                    period(std::move(period)),
                                                                    lastReference(std::move(lastReference)),
                                                                    fpgBn(std::move(fpgBn)) {
}
