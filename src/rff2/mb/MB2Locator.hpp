//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "../parallel/ParallelRenderState.h"
#include "MB2RenderData.hpp"

namespace merutilm::rff2 {

    struct MB2LocateResult {
        fixed_point_complex center;
        float logZoom;
    };


    struct BlockResult {
        fixed_point_complex residual;
        fixed_point_complex an;
        fixed_point_complex bn;
        complex<dex> fzgAn;
        int32_t requiredAdditionalPrecision;
    };

    struct MB2Locator {
        static constexpr float MINIBROT_LOG_ZOOM_OFFSET = 2.f;

        static fixed_point_complex calcCenterOffset(const MB2ReferenceBase &reference) {
            const int exp10 = Perturbator::logZoomToExp10(reference.logZoom);
            fixed_point_complex off(0.0, 0.0, exp10);
            calcCenterOffset(off, reference.checkpoints.back().z.create_variant(exp10),
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
                                     const std::vector<ReferenceCheckpoint> &checkpoints,
                                     const std::vector<complex<dex>> &amplitudes, const uint32_t partitionIndex,
                                     const int32_t dcCurrExp10, const int32_t aimExp10, const bool burst) {

            const ReferenceCheckpoint &currentCheckpoint = checkpoints[partitionIndex];
            const ReferenceCheckpoint &nextCheckpoint = checkpoints[partitionIndex + 1];

            const uint64_t startIteration = currentCheckpoint.refIteration;
            const uint64_t endIteration = nextCheckpoint.refIteration;


            const int32_t cutDigitCount = getCutDigitCount(amplitudes[partitionIndex]);
            const int32_t srcExp10 = aimExp10 / 2;
            const int32_t exp10Decrement = std::max(1, srcExp10 - dcCurrExp10);

            // magic number 3 and 64 is experimental, appropriate value is unknown.
            // magic number 4 in latter is double-step behind newton precision.
            const int32_t exp10 = burst ? std::max(srcExp10 - exp10Decrement * 3 + cutDigitCount - 64, aimExp10)
                                        : std::max(srcExp10 - exp10Decrement * 4, aimExp10);

            fixed_point_complex z = currentCheckpoint.z.create_variant(exp10);
            fixed_point_complex an(1, 0, exp10);
            fixed_point_complex bn(0, 0, exp10);
            fixed_point_complex c = currentCenter.create_variant(exp10);
            fixed_point_complex one(1, 0, exp10);

            int32_t currentExp10 = exp10;
            int32_t prevExp2div64 = 0;

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

                // the code below is currently not working for specific location, i dont know why

                if (burst) {
                    const int32_t cutDigit = getCutDigitCount(static_cast<complex<dex>>(an));
                    currentExp10 = std::min(-1, exp10 + cutDigit);
                    const int32_t exp2div64 = fixed_point_decimal::exp10_to_exp2div64(currentExp10);

                    if (exp2div64 != prevExp2div64) {
                        z.set_exp10(currentExp10);
                        c = currentCenter;
                        c.set_exp10(currentExp10);
                        an.set_exp10(currentExp10);
                        bn.set_exp10(currentExp10);
                        one.set_exp10(currentExp10);
                        prevExp2div64 = exp2div64;
                    }
                }
            }


            const fixed_point_complex zExpected = nextCheckpoint.z.create_variant(currentExp10);
            blockResult.residual.set_exp10(currentExp10);
            fixed_point_complex::sub(blockResult.residual, z, zExpected);
            blockResult.fzgAn = static_cast<complex<dex>>(an);
            blockResult.an = std::move(an);
            blockResult.bn = std::move(bn);
        }

        template<FnListeners::FnLocatingMB2 FnLocatingMB2>
        static void processPartitions(const ParallelRenderState &state, std::vector<BlockResult> &blockResults,
                                      const fixed_point_complex &currentCenter,
                                      const std::vector<ReferenceCheckpoint> &checkpoints, const int32_t dcCurrExp10,
                                      const int32_t aimExp10, const bool burst,
                                      const std::vector<complex<dex>> &amplitudes, std::mutex &partitionPickerMutex,
                                      uint32_t &processedPartition, FnLocatingMB2 &&fnLocatingMB2) {
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

                processPartition(state, blockResult, currentCenter, checkpoints, amplitudes, partitionIndex,
                                 dcCurrExp10, aimExp10, burst);
            }
        }

        static void translateCenter(fixed_point_complex &c, fixed_point_complex &dc, const fixed_point_complex &t,
                                    const fixed_point_complex &u, const fixed_point_complex &lastCheckpoint,
                                    fixed_point_complex &temp) {
            fixed_point_complex::add(temp, t, lastCheckpoint);
            calcCenterOffset(dc, temp, u);
            fixed_point_complex::add(c, c, dc);
        }
        static void rebaseCheckpoints(const fixed_point_complex &dc, const std::vector<fixed_point_complex> &tt,
                                      const std::vector<fixed_point_complex> &ut,
                                      std::vector<ReferenceCheckpoint> &checkpoints, fixed_point_complex &temp) {
            for (uint32_t i = 1; i < checkpoints.size(); ++i) {
                auto &checkpoint = checkpoints[i];

                fixed_point_complex::mul(temp, dc, ut[i]);
                fixed_point_complex::add(checkpoint.z, checkpoint.z, tt[i]);
                fixed_point_complex::add(checkpoint.z, checkpoint.z, temp);
            }
        }

        static void calculateAmplitudes(complex<dex> &fzgAn, complex<dex> &fpgBn, std::vector<fixed_point_complex> &tt,
                                        std::vector<fixed_point_complex> &ut,
                                        const std::vector<BlockResult> &blockResults) {
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

        static int32_t getCutDigitCount(const complex<dex> &an) { return rff_math::log10Approx(an.norm_approx()); }
        static void prepareApproxAmplitudes(std::vector<complex<dex>> &amplitudes,
                                            const std::vector<BlockResult> &blockResults) {

            complex<dex> an = complex<dex>::ONE;
            for (uint32_t i = 0; i < static_cast<uint32_t>(blockResults.size()); ++i) {
                amplitudes[i] = an;
                an *= blockResults[i].fzgAn;
                an = an.try_normalized_value();
            }
        }

        static void setExp10(std::vector<BlockResult> &blockResults, fixed_point_complex &currentCenter,
                             std::vector<ReferenceCheckpoint> &checkpoints, fixed_point_complex &dc,
                             fixed_point_complex &temp, std::vector<fixed_point_complex> &tt,
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
                checkpoint.z.set_exp10(doubledExp10);
            }
            for (auto &blockResult: blockResults) {
                blockResult.an.set_exp10(doubledExp10);
                blockResult.bn.set_exp10(doubledExp10);
                blockResult.residual.set_exp10(doubledExp10);
            }
        }
        static bool checkAndUpdateHistory(std::array<int32_t, 10> &exp10History, const int32_t dcCurrExp10,
                                          const bool burst) {
            for (uint32_t i = 1; i < static_cast<uint32_t>(exp10History.size()); ++i) {
                exp10History[i - 1] = exp10History[i];
            }
            exp10History.back() = dcCurrExp10;
            if (exp10History.front() - dcCurrExp10 <= 1) {
                if (burst) {
                    vkh::logger::log_err("Failed to locate minibrot using 'Burst-locate'. Please uncheck the “Use "
                                         "Burst-locate” box and try again.");
                    return false;
                } else {
                    vkh::logger::log_err(
                            "Failed to locate minibrot. it might be a bug! please report this issue to developer.");
                    return false;
                }
            }
            return true;
        }
        template<FnListeners::FnLocatingMB2 FnLocatingMB2>
        static std::optional<MB2LocateResult> locateMinibrot(const ParallelRenderState &state,
                                                             const MB2RenderDataBase &data,
                                                             FnLocatingMB2 &&fnLocatingMB2, bool burst) {
            // multiply zoom by 2 and find center offset.
            // set the center to center + centerOffset.


            const MB2ReferenceBase *reference = data.getReference();

            if (!reference) {
                return std::nullopt;
            }


            const float logZoom = data.fractalSettings.general.logZoom;
            const int32_t refExp10 = Perturbator::logZoomToExp10(logZoom);
            float aimLogZoom = logZoom * 2;
            int32_t aimExp10 = Perturbator::logZoomToExp10(aimLogZoom);

            const uint32_t threads = data.fractalSettings.general.threads;
            std::vector<std::unique_ptr<std::jthread>> threadPool;
            threadPool.resize(threads);

            // copy checkpoints
            std::vector<ReferenceCheckpoint> checkpoints = reference->checkpoints;
            std::vector blockResults(checkpoints.size() - 1,
                                     BlockResult{.residual = fixed_point_complex(0.0, 0.0, refExp10),
                                                 .an = fixed_point_complex(0.0, 0.0, refExp10),
                                                 .bn = fixed_point_complex(0.0, 0.0, refExp10),
                                                 .fzgAn = complex<dex>::ONE});

            for (uint32_t i = 0; i < blockResults.size(); i++) {
                blockResults[i].fzgAn = checkpoints[i + 1].fzgAn;
            }

            std::vector<complex<dex>> approxAmplitudes(checkpoints.size() - 1);


            const dex dcMax = data.getPerturbator()->dcMax;

            complex<dex> fzgAn = complex<dex>::ONE;
            complex<dex> fpgBn = complex<dex>::ZERO;

            std::vector tt(checkpoints.size(), fixed_point_complex{0.0, 0.0, refExp10});
            std::vector ut(checkpoints.size(), fixed_point_complex{0.0, 0.0, refExp10});


            fixed_point_complex currentCenter = reference->center.create_variant(refExp10);
            fixed_point_complex dc(0.0, 0.0, refExp10);
            fixed_point_complex temp(0.0, 0.0, refExp10);

            calcCenterOffset(dc, checkpoints.back().z.create_variant(refExp10),
                             fixed_point_complex(reference->fpgBn, refExp10));
            fixed_point_complex::add(currentCenter, currentCenter, dc);

            dex dcd = static_cast<complex<dex>>(dc).norm_approx();
            complex<dex> mbScale = complex<dex>::ONE;

            std::array<int32_t, 10> exp10History{};

            if (dcMax < dcd) {
                vkh::logger::log_err("Center could not be found");
                return std::nullopt;
            }

            do {

                int32_t dcCurrExp10 = dcd.is_zero() ? aimExp10 : rff_math::log10Approx(dcd);
                if (!checkAndUpdateHistory(exp10History, dcCurrExp10, burst)) {
                    return std::nullopt;
                }

                prepareApproxAmplitudes(approxAmplitudes, blockResults);

                // ReSharper disable once CppTooWideScope
                std::mutex partitionPickerMutex;
                // ReSharper disable once CppTooWideScope
                uint32_t processedPartition = 0;

                for (uint32_t i = 0; i < threads; ++i) {
                    threadPool[i] = std::make_unique<std::jthread>(
                            [&state, &blockResults, &currentCenter, &checkpoints, dcCurrExp10, aimExp10, burst,
                             &approxAmplitudes, &partitionPickerMutex, &processedPartition, &fnLocatingMB2] {
                                processPartitions(state, blockResults, currentCenter, checkpoints, dcCurrExp10,
                                                  aimExp10, burst, approxAmplitudes, partitionPickerMutex,
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


                setExp10(blockResults, currentCenter, checkpoints, dc, temp, tt, ut, aimExp10);
                calculateAmplitudes(fzgAn, fpgBn, tt, ut, blockResults);
                translateCenter(currentCenter, dc, tt.back(), ut.back(), checkpoints.back().z, temp);
                rebaseCheckpoints(dc, tt, ut, checkpoints, temp);


                mbScale = fzgAn * fpgBn;
                if (mbScale.is_zero()) {
                    vkh::logger::log_err("minibrot size cannot be measured");
                    return std::nullopt;
                }

                aimLogZoom = static_cast<float>(rff_math::log10(mbScale.norm_approx()));
                aimExp10 = Perturbator::logZoomToExp10(aimLogZoom + MINIBROT_LOG_ZOOM_OFFSET);

                dcd = static_cast<complex<dex>>(dc).norm_approx();

            } while (rff_math::log10(dcd) > -aimLogZoom);

            const auto resultLogZoom = aimLogZoom + MINIBROT_LOG_ZOOM_OFFSET;

            return MB2LocateResult{.center = std::move(currentCenter), .logZoom = resultLogZoom};
        }
    };
} // namespace merutilm::rff2
