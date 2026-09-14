//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "../parallel/ParallelRenderState.h"
#include "MB2RenderData.hpp"

namespace merutilm::rff2 {

    struct MB2LocateResult {
        fixed_point_complex center;
        double logZoom;
    };


    struct BlockResult {
        fixed_point_complex residual;
        fixed_point_complex an;
        fixed_point_complex bn;
        complex<dex> fzgAn;
    };

    struct MB2Locator {
        static constexpr float MINIBROT_LOG_ZOOM_OFFSET = 2.f;

        static fixed_point_complex calcCenterOffset(const MB2ReferenceBase &reference) {
            const int exp10 = Perturbator::logZoomToExp10(reference.logZoom);
            fixed_point_complex off(0, 0, exp10);
            calcCenterOffset(off, reference.checkpoints.back().complex.create_variant(exp10),
                             fixed_point_complex(reference.fpgBn, exp10));
            return off;
        }

        static void calcCenterOffset(fixed_point_complex &result, const fixed_point_complex &z,
                                     const fixed_point_complex &bn) {
            fixed_point_complex::div(result, z, bn);
            fixed_point_complex::neg(result);
        }


        static void processPartition(const ParallelRenderState &state, BlockResult &blockResult,
                                     const fixed_point_complex &currentCenter,
                                     const std::vector<ReferenceCheckpoint> &checkpoints, const uint32_t partitionIndex,
                                     const int32_t exp10) {

            const ReferenceCheckpoint &currentCheckpoint = checkpoints[partitionIndex];
            const ReferenceCheckpoint &nextCheckpoint = checkpoints[partitionIndex + 1];

            const uint64_t startIteration = currentCheckpoint.refIteration;
            const uint64_t endIteration = nextCheckpoint.refIteration;


            // clone z
            const fixed_point_complex one(1.0, 0.0, exp10);
            const fixed_point_complex c = currentCenter.create_variant(exp10);
            const fixed_point_complex zExpected = nextCheckpoint.complex.create_variant(exp10);
            fixed_point_complex z = currentCheckpoint.complex.create_variant(exp10);
            fixed_point_complex an(1, 0, exp10);
            fixed_point_complex bn(0, 0, exp10);

            // An, Bn generation
            for (uint64_t iteration = startIteration; iteration < endIteration; ++iteration) {

                if (state.interruptRequested() &&
                    iteration % Constants::Fractal::PARALLEL_OPERATION_INTERRUPT_CHECK_INTERVAL)
                    return;

                if (iteration > 0) {
                    fixed_point_complex::mul(an, an, z);
                    fixed_point_complex::dbl(an, an);
                }

                fixed_point_complex::mul(bn, bn, z);
                fixed_point_complex::dbl(bn, bn);
                fixed_point_complex::add(bn, bn, one);

                fixed_point_complex::sqr(z, z);
                fixed_point_complex::add(z, z, c);
            }

            blockResult.residual.set_exp10(exp10);
            fixed_point_complex::sub(blockResult.residual, z, zExpected);
            blockResult.an = startIteration == 0 ? fixed_point_complex(0, 0, exp10) : std::move(an);
            blockResult.bn = std::move(bn);
            blockResult.fzgAn = static_cast<complex<dex>>(an);
        }

        template<FnListeners::FnLocatingMB2 FnLocatingMB2>
        static void processPartitions(const ParallelRenderState &state, std::vector<BlockResult> &blockResults,
                                      const fixed_point_complex &currentCenter,
                                      const std::vector<ReferenceCheckpoint> &checkpoints, const int32_t dcCurrExp10, const int32_t exp10,
                                      std::mutex &partitionPickerMutex, uint32_t &processedPartition,
                                      FnLocatingMB2 &&fnLocatingMB2) {
            while (true) {
                uint32_t partitionIndex = 0;
                {
                    std::scoped_lock lock(partitionPickerMutex);
                    partitionIndex = processedPartition++;

                    if (partitionIndex >= checkpoints.size() - 1) {
                        return;
                    }

                    fnLocatingMB2(dcCurrExp10, partitionIndex, static_cast<uint32_t>(checkpoints.size() - 1));
                }


                BlockResult &blockResult = blockResults[partitionIndex];

                processPartition(state, blockResult, currentCenter, checkpoints, partitionIndex, exp10);
            }
        }


        static void translateCenter(fixed_point_complex & c, fixed_point_complex & dc, const fixed_point_complex & t, const fixed_point_complex &u, const fixed_point_complex &lastCheckpoint, fixed_point_complex & temp) {
            fixed_point_complex::add(temp, t, lastCheckpoint);
            calcCenterOffset(dc, temp, u);
            fixed_point_complex::add(c, c, dc);

        }
        static void rebaseCheckpoints(const fixed_point_complex & dc, const std::vector<fixed_point_complex> & tt, const std::vector<fixed_point_complex> & ut, std::vector<ReferenceCheckpoint> & checkpoints, fixed_point_complex & temp) {
            for (uint32_t i = 1; i < checkpoints.size(); ++i) {
                auto &checkpoint = checkpoints[i];

                fixed_point_complex::mul(temp, dc, ut[i]);
                fixed_point_complex::add(checkpoint.complex, checkpoint.complex, tt[i]);
                fixed_point_complex::add(checkpoint.complex, checkpoint.complex, temp);
            }
        }

        static void calculateAmplitudes(complex<dex> & fzgAn, complex<dex> & fpgBn, std::vector<fixed_point_complex> & tt, std::vector<fixed_point_complex> & ut, const std::vector<BlockResult> & blockResults) {
            fzgAn = complex<dex>::ONE;
            fixed_point_complex::zero(tt[0]);
            fixed_point_complex::zero(ut[0]);

            for (uint32_t i = 0; i < blockResults.size(); ++i) {
                const auto &blockResult = blockResults[i];

                fzgAn = (fzgAn * blockResult.fzgAn).try_normalized_value();

                fixed_point_complex::mul(tt[i + 1], tt[i], blockResult.an);
                fixed_point_complex::add(tt[i + 1], tt[i + 1], blockResult.residual);

                fixed_point_complex::mul(ut[i + 1], ut[i], blockResult.an);
                fixed_point_complex::add(ut[i + 1], ut[i + 1], blockResult.bn);
            }

            fpgBn = static_cast<complex<dex>>(ut.back());
        }

        static void setExp10( std::vector<BlockResult> &blockResults, fixed_point_complex &currentCenter, std::vector<ReferenceCheckpoint> &checkpoints,
                             fixed_point_complex &dc, fixed_point_complex &temp, std::vector<fixed_point_complex> &tt,
                             std::vector<fixed_point_complex> &ut, const int32_t doubledExp10) {

            currentCenter.set_exp10(doubledExp10);
            dc.set_exp10(doubledExp10);
            temp.set_exp10(doubledExp10);

            for (auto &tt0: tt) {
                tt0.set_exp10(doubledExp10);
            }
            for (auto &ut0: ut) {
                ut0.set_exp10(doubledExp10);
            }
            for (auto &checkpoint: checkpoints) {
                checkpoint.complex.set_exp10(doubledExp10);
            }
            for (auto &blockResult: blockResults) {
                blockResult.an.set_exp10(doubledExp10);
                blockResult.bn.set_exp10(doubledExp10);
                blockResult.residual.set_exp10(doubledExp10);
            }
        }
        template<FnListeners::FnLocatingMB2 FnLocatingMB2>
        static std::optional<MB2LocateResult> locateMinibrot(const ParallelRenderState &state,
                                                             const MB2RenderDataBase &data,
                                                             FnLocatingMB2 &&fnLocatingMB2) {
            // multiply zoom by 2 and find center offset.
            // set the center to center + centerOffset.


            const MB2ReferenceBase *reference = data.getReference();

            if (!reference) {
                return std::nullopt;
            }


            const float logZoom = data.fractalSettings.general.logZoom;
            const int32_t refExp10 = Perturbator::logZoomToExp10(logZoom);
            const int32_t doubledExp10 = refExp10 * 2;


            fixed_point_complex currentCenter = reference->center.create_variant(refExp10);
            fixed_point_complex dc(0, 0, refExp10);
            fixed_point_complex temp(0, 0, refExp10);

            calcCenterOffset(dc, reference->checkpoints.back().complex.create_variant(refExp10),
                             fixed_point_complex(reference->fpgBn, refExp10));
            fixed_point_complex::add(currentCenter, currentCenter, dc);

            dex dcd = static_cast<complex<dex>>(dc).norm_approx();

            if (dcd.is_zero()) return std::nullopt;


            const uint32_t threads = data.fractalSettings.general.threads;
            std::vector<std::unique_ptr<std::jthread>> threadPool;
            threadPool.resize(threads);

            // copy checkpoints
            std::vector<ReferenceCheckpoint> checkpoints = reference->checkpoints;
            std::vector blockResults(checkpoints.size() - 1,
                                     BlockResult{.residual = fixed_point_complex(0, 0, refExp10),
                                                 .an = fixed_point_complex(0, 0, refExp10),
                                                 .bn = fixed_point_complex(0, 0, refExp10)});



            const dex dcMax = data.getPerturbator()->dcMax;
            const dex doubledZoomDcMax = dcMax * dcMax;

            complex<dex> fzgAn = complex<dex>::ONE;
            complex<dex> fpgBn = complex<dex>::ZERO;

            std::vector tt(checkpoints.size(), fixed_point_complex{0, 0, refExp10});
            std::vector ut(checkpoints.size(), fixed_point_complex{0, 0, refExp10});

            while (dcd > doubledZoomDcMax) {


                if (dcMax < static_cast<complex<dex>>(dc).norm_approx()) {
                    vkh::logger::log_err("The center could not be found");
                    return std::nullopt;
                }


                int32_t dcCurrExp10 = rff_math::log10(dcd);
                int32_t exp10Decrement = std::max(0, refExp10 - dcCurrExp10);
                int32_t exp10 = std::max(refExp10 - exp10Decrement * 4 - Constants::Fractal::EXP10_ADDITION, doubledExp10);

                // ReSharper disable once CppTooWideScope
                std::mutex partitionPickerMutex;
                // ReSharper disable once CppTooWideScope
                uint32_t processedPartition = 0;

                for (uint32_t i = 0; i < threads; ++i) {
                    threadPool[i] = std::make_unique<std::jthread>([&state, &blockResults, &currentCenter, &checkpoints, dcCurrExp10, exp10, &partitionPickerMutex, &processedPartition,
                                                                    &fnLocatingMB2] {
                        processPartitions(state, blockResults, currentCenter, checkpoints, dcCurrExp10, exp10, partitionPickerMutex,
                                          processedPartition, std::forward<FnLocatingMB2>(fnLocatingMB2));
                    });
                }

                // wait for complete
                for (auto &thread: threadPool) {
                    if (thread->joinable()) {
                        thread->join();
                    }
                    thread = nullptr;
                }


                if (state.interruptRequested())
                    return std::nullopt;


                setExp10(blockResults, currentCenter, checkpoints, dc, temp, tt, ut, exp10);
                calculateAmplitudes(fzgAn, fpgBn, tt, ut, blockResults);
                translateCenter(currentCenter, dc, tt.back(), ut.back(), checkpoints.back().complex, temp);
                rebaseCheckpoints(dc, tt, ut, checkpoints, temp);




                // set dc radius
                dcd = static_cast<complex<dex>>(dc).norm_approx();
            }

            const auto scale = fzgAn * fpgBn;
            if (scale.is_zero()) throw vkh::exception_invalid_state("invalid operation");
            const auto resultLogZoom =
                    static_cast<float>(rff_math::log10(scale.norm_approx()) + MINIBROT_LOG_ZOOM_OFFSET);

            return MB2LocateResult{std::move(currentCenter), resultLogZoom};
        }
    };
} // namespace merutilm::rff2
