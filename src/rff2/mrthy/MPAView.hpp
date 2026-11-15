//
// Created by Merutilm on 11/9/26.
//

#pragma once
#include "../calc/iteration_types.hpp"
#include "SRPCCursor.hpp"
namespace merutilm::rff2 {


    template<Number Num>
    class MPAView : public SRPCCursor<itt::ref> {
        const MPAData &data;
        size_t currentLevels = 0;

    public:
        explicit MPAView(const MPAData &data);


        [[nodiscard]] const PA<Num> *lookup(ReferenceView<Num> &referenceView, itt::ref refIteration,
                              const complex<Num> &dz) const noexcept;


        void lookupBatch(std::array<PA<Num>, RFF_SIMD_CHUNK_SIZE> &results,
                         const std::array<itt::ref, RFF_SIMD_CHUNK_SIZE> &refIterations,
                         const std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> &dzs,
                         const std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> &finishes) const noexcept;
    private:
        void nextPulledTableIndex() {
            next(); // SRPCCursor::originalIndex is equal to pulledTableIndex
            //TODO
        }

        void jumpPulledTableIndex(ReferenceView<Num> &referenceView, size_t level) {
            // referenceView.manipulateSegment(segment)
            //TODO
        }

    };

    template<Number Num>
    MPAView<Num>::MPAView(const MPAData &data) : SRPCCursor(data.compressor), data(data) {}

    template<Number Num>
    const PA<Num> *MPAView<Num>::lookup(ReferenceView<Num> &referenceView, const itt::ref refIteration,
                                        const complex<Num> &dz) const noexcept {

        if (refIteration == 0 || !data.buffer) {
            return nullptr;
        }

        // TODO optimize
        const MPAIndexMapper mapper = MPAUtils::getFlattenIndexMapper(data, refIteration);

        if (mapper.mapped == UINT64_MAX) {
            return nullptr;
        }


        MPAUtils::debugCheckMPAFromMapper(data.buffer->mpaTable.sizeUsed, mapper.mapped, data.info.tablePeriods.size(),
                                          mapper.generatedLevels);

        const auto table = MPAUtils::getMPAFromMapper<Num>(data, mapper);
        const Num r = dz.max();

        switch (data.mpaSettings.selectionMethod) {
            using enum FrtMPASelectionMethod;
            case LOWEST: {
                const PA<Num> *pa = nullptr;

                for (const PA<Num> &test: table) {

                    if (test.isValid(r)) {
                        pa = &test;
                    } else
                        return pa;
                }
                return pa;
            }
            case HIGHEST: {
                const PA<Num> &pa = table.front();
                // This table cannot be empty because the pre-processing is done.

                if (!pa.isValid(r)) {
                    return nullptr;
                }

                for (size_t j = table.size(); j > 1; --j) {
                    const PA<Num> &test = table[j - 1];

                    if (test.isValid(r)) {
                        return &test;
                    }
                }

                return &pa;
            }
            default:
                return nullptr;
        }
    }
    template<Number Num>
    void MPAView<Num>::lookupBatch(std::array<PA<Num>, 16> &results, const std::array<itt::ref, 16> &refIterations,
                                   const std::array<complex<Num>, 16> &dzs,
                                   const std::array<uint8_t, 16> &finishes) const noexcept {
        // TODO Change to AVX512 instructions
        // if (mpaPeriod == nullptr) {
        //     std::ranges::fill(results, PA<Num>::IDENTITY);
        //     return;
        // }
        //
        // std::array<MPAIndexMapper, RFF_SIMD_CHUNK_SIZE> mapperTemp;
        // std::array<Num, RFF_SIMD_CHUNK_SIZE> radiusTemp;
        // std::array<std::span<const PA<Num>>, RFF_SIMD_CHUNK_SIZE> tableTemp;
        //
        // for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
        //     mapperTemp[i] = getFlattenIndexMapper(refIterations[i]);
        //     if (mapperTemp[i].mapped == UINT64_MAX) {
        //         mapperTemp[i].mapped = 0;
        //     } else
        //         MPAUtils::debugCheckMPAFromMapper(buffer.mpaTable.sizeUsed, mapperTemp[i].mapped,
        //                                 data.info.tablePeriods.size(), mapperTemp[i].generatedLevels);
        //     tableTemp[i] = getMPAFromMapper(mapperTemp[i]);
        //     radiusTemp[i] = dzs[i].max();
        // }
        //
        // switch (mpaSettings.selectionMethod) {
        //     using enum FrtMPASelectionMethod;
        //     case LOWEST: {
        //         for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
        //             const PA<Num> *pa = &PA<Num>::IDENTITY;
        //             if (!finishes[i]) {
        //                 for (const PA<Num> &test: tableTemp[i]) {
        //                     if (test.isValid(radiusTemp[i]))
        //                         pa = &test;
        //                     else
        //                         break;
        //                 }
        //             }
        //             results[i] = *pa;
        //         }
        //         break;
        //     }
        //     case HIGHEST: {
        //         for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
        //             const PA<Num> *pa = &PA<Num>::IDENTITY;
        //             if (!finishes[i] && !tableTemp[i].empty() && tableTemp[i].front().isValid(radiusTemp[i])) {
        //                 for (size_t j = tableTemp[i].size(); j > 0; --j) {
        //                     const PA<Num> &test = tableTemp[i][j - 1];
        //                     if (test.isValid(radiusTemp[i])) {
        //                         pa = &test;
        //                         break;
        //                     }
        //                 }
        //             }
        //             results[i] = *pa;
        //         }
        //         break;
        //     }
        //     default:
        //         break;
        // }
    }

} // namespace merutilm::rff2
