//
// Created by Merutilm on 2025-05-16.
// Sensitivity-Tapered Multiple Shooting (STMS) Algorithm by GPT-6 Astra on 2026-09-10
// Re-Implemented by Merutilm on 2026-09-13

#include "MB2Locator.hpp"

#include "MB2Reference.h"
#include "MB2RenderData.hpp"
#include "Perturbator.h"


namespace merutilm::rff2 {

    fixed_point_complex MB2Locator::calcCenterOffset(const MB2ReferenceBase &reference) {
        const int exp10 = Perturbator::logZoomToExp10(reference.logZoom);
        fixed_point_complex off(0, 0, exp10);
        calcCenterOffset(off, reference.checkpoints.back().complex.create_variant(exp10),
                         reference.fpgBn.create_variant(exp10));
        return off;
    }

    void MB2Locator::calcCenterOffset(fixed_point_complex &result, const fixed_point_complex &z,
                                      const fixed_point_complex &bn) {
        fixed_point_complex::div(result, z, bn);
        fixed_point_complex::neg(result);
    }


    std::optional<MB2LocateResult>
    MB2Locator::locateMinibrot(const ParallelRenderState &state, const MB2RenderDataBase &data,
                               const std::function<void(uint64_t, int)> &actionWhileFindingMinibrotCenter) {
        // multiply zoom by 2 and find center offset.
        // set the center to center + centerOffset.


        const MB2ReferenceBase *reference = data.getReference();

        if (!reference) {
            return std::nullopt;
        }


        const float logZoom = data.fractalSettings.general.logZoom;
        const float doubledLogZoom = logZoom * 2;
        const int doubledExp10 = Perturbator::logZoomToExp10(doubledLogZoom);

        const uint32_t threads = std::min(data.fractalSettings.general.threads,
                                          static_cast<uint32_t>(reference->checkpoints.size() - 1));
        std::vector<std::unique_ptr<std::jthread>> threadPool;
        threadPool.resize(threads);

        // copy checkpoints
        std::vector<ReferenceCheckpoint> checkpoints = reference->checkpoints;

        for (auto &checkpoint: checkpoints) {
            checkpoint.complex = checkpoint.complex.create_variant(doubledExp10);
        }


        std::vector blockResults(threads, BlockResult{.residual = fixed_point_complex(0, 0, doubledExp10),
                                                      .an = fixed_point_complex(0, 0, doubledExp10),
                                                      .bn = fixed_point_complex(0, 0, doubledExp10)});

        const fixed_point_complex srcCenter = reference->center.create_variant(doubledExp10);
        fixed_point_complex dc(0, 0, doubledExp10);

        fixed_point_complex currentCenter = srcCenter;
        const fixed_point_complex one(1.0, 0.0, doubledExp10);

        calcCenterOffset(dc, checkpoints.back().complex, reference->fpgBn.create_variant(doubledExp10));

        const dex dcMax = data.getPerturbator()->dcMax;
        const dex doubledZoomDcMax = dcMax * dcMax;
        complex<dex> fzgAn = complex<dex>::ONE;
        complex<dex> fpgBn = complex<dex>::ZERO;


        std::vector tt(checkpoints.size(), fixed_point_complex{0, 0, doubledExp10});
        std::vector ut(checkpoints.size(), fixed_point_complex{0, 0, doubledExp10});

        for (uint32_t repetition = 0; static_cast<complex<dex>>(dc).norm_approx() > doubledZoomDcMax; ++repetition) {

            if (dcMax < static_cast<complex<dex>>(dc).norm_approx()) {
                vkh::logger::log_err("The center could not be found");
                return std::nullopt;
            }

            fixed_point_complex::add(currentCenter, currentCenter, dc);

            for (uint32_t i = 0; i < threads; ++i) {

                threadPool[i] = std::make_unique<std::jthread>(
                        [&state, &checkpoints, &currentCenter, doubledExp10, &blockResults, &one, i,
                         &actionWhileFindingMinibrotCenter, repetition, threads] {
                            assert(i + 1 < checkpoints.size());

                            const ReferenceCheckpoint &currentCheckpoint = checkpoints[i];
                            const ReferenceCheckpoint &nextCheckpoint = checkpoints[i + 1];

                            const uint64_t startIteration = currentCheckpoint.refIteration;
                            const uint64_t endIteration = nextCheckpoint.refIteration;


                            // clone z
                            fixed_point_complex z = currentCheckpoint.complex;
                            fixed_point_complex an(1, 0, doubledExp10);
                            fixed_point_complex bn(0, 0, doubledExp10);

                            complex<dex> fzgAnTemp = complex<dex>::ONE;

                            for (uint64_t iteration = startIteration; iteration < endIteration; ++iteration) {

                                if (state.interruptRequested() &&
                                    iteration % Constants::Fractal::PARALLEL_OPERATION_INTERRUPT_CHECK_INTERVAL)
                                    return;

#ifndef NDEBUG
                                actionWhileFindingMinibrotCenter(iteration, repetition);
#else
                                if (i == 0)
                                    actionWhileFindingMinibrotCenter((iteration - 1) * threads, repetition);
#endif


                                fixed_point_complex::mul(an, an, z);
                                fixed_point_complex::dbl(an, an);

                                if (iteration > 0) {
                                    fzgAnTemp = (fzgAnTemp * 2 * static_cast<complex<dex>>(z)).try_normalized_value();
                                }
                                fixed_point_complex::mul(bn, bn, z);
                                fixed_point_complex::dbl(bn, bn);
                                fixed_point_complex::add(bn, bn, one);

                                fixed_point_complex::sqr(z, z);
                                fixed_point_complex::add(z, z, currentCenter);
                            }

                            BlockResult &blockResult = blockResults[i];
                            fixed_point_complex::sub(blockResult.residual, z, nextCheckpoint.complex);

                            blockResult.an = std::move(an);
                            blockResult.bn = std::move(bn);
                            blockResult.fzgAn = fzgAnTemp;
                        });

#ifndef NDEBUG
                if (threadPool[i]->joinable()) {
                    threadPool[i]->join();
                }
#endif
            }

            for (auto &thread: threadPool) {
                if (thread->joinable()) {
                    thread->join();
                }
                thread = nullptr;
            }

            if (state.interruptRequested())
                return std::nullopt;



            fzgAn = complex<dex>::ONE;

            tt[0] = fixed_point_complex(0, 0, doubledExp10);
            ut[0] = fixed_point_complex(0, 0, doubledExp10);

            for (uint32_t i = 0; i < threads; ++i) {
                const auto &blockResult = blockResults[i];

                fzgAn = (fzgAn * blockResult.fzgAn).try_normalized_value();

                fixed_point_complex::mul(tt[i + 1], tt[i], blockResult.an);
                fixed_point_complex::add(tt[i + 1], tt[i + 1], blockResult.residual);

                fixed_point_complex::mul(ut[i + 1], ut[i], blockResult.an);
                fixed_point_complex::add(ut[i + 1], ut[i + 1], blockResult.bn);
            }

            fixed_point_complex temp(0, 0, doubledExp10);

            fixed_point_complex::add(temp, tt.back(), checkpoints.back().complex);
            calcCenterOffset(dc, temp, ut.back());

            for (uint32_t i = 1; i < checkpoints.size(); ++i) {
                auto &checkpoint = checkpoints[i];

                fixed_point_complex::mul(temp, dc, ut[i]);
                fixed_point_complex::add(checkpoint.complex, checkpoint.complex, tt[i]);
                fixed_point_complex::add(checkpoint.complex, checkpoint.complex, temp);
            }

            fpgBn = static_cast<complex<dex>>(ut.back());
        }

        const float resultLogZoom = rff_math::log10((fzgAn * fpgBn).norm_approx()) + MINIBROT_LOG_ZOOM_OFFSET;

        return MB2LocateResult{std::move(currentCenter), resultLogZoom};
    }
} // namespace merutilm::rff2
