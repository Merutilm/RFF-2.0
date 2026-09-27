//
// Created by Merutilm on 9/26/26.
// Sensitivity-Tapered Multiple Shooting (STMS) Algorithm by GPT-6 Astra on 2026-09-10
// Re-Implemented by Merutilm on 2026-09-13


#include "MB2Locator.hpp"

namespace merutilm::rff2 {

    MB2Locator::MB2Locator(const ParallelRenderState &state, const MB2RenderDataBase &data, const ExpLocatorSettings &locSettings,
                           FnListeners::FnLocatingMB2W &&fnLocatingMB2W) :
        state(state), locSettings(locSettings), threads(data.fractalSettings.general.threads),
        refLogZoom(data.fractalSettings.general.logZoom), refDcMax(data.getPerturbator()->dcMax),
        refFpgBn(data.getReference()->fpgBn), currentCenter(data.fractalSettings.reference.center),
        checkpoints(data.getReference()->checkpoints), fnLocatingMB2W(std::move(fnLocatingMB2W)) {}

    fixed_point_complex MB2Locator::calcCenterOffset(const MB2ReferenceBase &reference) {
        const int64_t exp10 = Perturbator::logZoomToExp10(reference.logZoom);
        fixed_point_complex off(0.0, 0.0, exp10);
        calcCenterOffset(off, reference.checkpoints.back().z.create_variant(exp10),
                         fixed_point_complex(reference.fpgBn, exp10));
        return off;
    }

    void MB2Locator::calcCenterOffset(fixed_point_complex &result, const fixed_point_complex &z,
                                      const fixed_point_complex &bn) {
        fixed_point_complex::div(result, z, bn);
        fixed_point_complex::neg(result);
    }

    PartitionStatus MB2Locator::processPartition(const uint32_t partitionIndex, const int64_t dcCurrExp10,
                                                 const int64_t aimExp10) {

        const ReferenceCheckpoint &currentCheckpoint = checkpoints[partitionIndex];
        const ReferenceCheckpoint &nextCheckpoint = checkpoints[partitionIndex + 1];

        const uint64_t startIteration = currentCheckpoint.refIteration;
        const uint64_t endIteration = nextCheckpoint.refIteration;


        const int64_t cutDigitCount = getCutDigitCount(approxAmplitudes[partitionIndex]);
        const int64_t srcExp10 = aimExp10 / 2;
        const int64_t exp10Decrement = std::max(static_cast<int64_t>(1), srcExp10 - dcCurrExp10);

        // magic number 3 and 64 is experimental, appropriate value is unknown.
        // magic number 4 in latter is double-step behind newton precision.
        const int64_t exp10 = locSettings.burst ? std::max(srcExp10 - exp10Decrement * 3 + cutDigitCount - 64, aimExp10)
                                    : std::max(srcExp10 - exp10Decrement * 4, aimExp10);

        fixed_point_complex z = currentCheckpoint.z.create_variant(exp10);
        fixed_point_complex an(1, 0, exp10);
        fixed_point_complex bn(0, 0, exp10);
        fixed_point_complex c = currentCenter.create_variant(exp10);

        int64_t currentExp10 = exp10;
        int64_t prevExp2div64 = 0;

        // An, Bn generation
        for (uint64_t iteration = startIteration; iteration < endIteration; ++iteration) {

            if (state.interruptRequested() && iteration % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL)
                return PartitionStatus::INTERRUPTED;

            if (iteration > 0) {
                fixed_point_complex::mul(an, an, z);
                fixed_point_complex::dbl(an, an);
            }

            fixed_point_complex::mul(bn, bn, z);
            fixed_point_complex::dbl(bn, bn);
            bn.add_one();

            if (locSettings.crvp) {
                knownOrbit[iteration] = z;
            }
            fixed_point_complex::sqr(z, z);
            fixed_point_complex::add(z, z, c);


            // the code below is currently not working for specific location, i dont know why

            if (locSettings.burst) {

                if (static_cast<complex<dex>>(z).norm_approx() > dex(1e8)) {
                    return PartitionStatus::ERROR_BURST_Z_ESCAPED;
                }

                const int64_t cutDigit = getCutDigitCount(static_cast<complex<dex>>(an));
                currentExp10 = std::min(static_cast<int64_t>(-1), exp10 + cutDigit);
                const int64_t exp2div64 = fixed_point_decimal::exp10_to_exp2div64(currentExp10);

                if (exp2div64 != prevExp2div64) {
                    z.set_exp10(currentExp10);
                    c = currentCenter;
                    c.set_exp10(currentExp10);
                    an.set_exp10(currentExp10);
                    bn.set_exp10(currentExp10);
                    prevExp2div64 = exp2div64;
                }
            }
        }

        auto &blockResult = blockResults[partitionIndex];
        const fixed_point_complex zExpected = nextCheckpoint.z.create_variant(currentExp10);
        blockResult.residual.set_exp10(currentExp10);
        fixed_point_complex::sub(blockResult.residual, z, zExpected);
        blockResult.fzgAn = static_cast<complex<dex>>(an);
        blockResult.an = std::move(an);
        blockResult.bn = std::move(bn);
        return PartitionStatus::SUCCESS;
    }

    PartitionStatus MB2Locator::processPartitions(const int64_t dcCurrExp10, const int64_t aimExp10,
                                                  std::mutex &partitionPickerMutex, uint32_t &processedPartition) {
        while (true) {
            uint32_t partitionIndex = 0;
            {
                std::scoped_lock lock(partitionPickerMutex);
                partitionIndex = processedPartition++;

                if (partitionIndex >= checkpoints.size() - 1) {
                    return PartitionStatus::SUCCESS;
                }

                fnLocatingMB2W(dcCurrExp10, partitionIndex, static_cast<uint32_t>(checkpoints.size() - 1));
            }

            const PartitionStatus result = processPartition(partitionIndex, dcCurrExp10, aimExp10);

            if (result != PartitionStatus::SUCCESS)
                return result;
        }
    }

    void MB2Locator::translateCenter(fixed_point_complex &dc, const fixed_point_complex &t,
                                     const fixed_point_complex &u) {
        fixed_point_complex::add(temp, t, checkpoints.back().z);
        calcCenterOffset(dc, temp, u);
        fixed_point_complex::add(currentCenter, currentCenter, dc);
    }

    void MB2Locator::rebaseCheckpoints(const fixed_point_complex &dc, const std::vector<fixed_point_complex> &tt,
                                       const std::vector<fixed_point_complex> &ut) {
        for (uint32_t i = 1; i < checkpoints.size(); ++i) {
            auto &checkpoint = checkpoints[i];

            fixed_point_complex::mul(temp, dc, ut[i]);
            fixed_point_complex::add(checkpoint.z, checkpoint.z, tt[i]);
            fixed_point_complex::add(checkpoint.z, checkpoint.z, temp);
        }
    }

    void MB2Locator::calculateAmplitudes(complex<dex> &fzgAn, complex<dex> &fpgBn, std::vector<fixed_point_complex> &tt,
                                         std::vector<fixed_point_complex> &ut) const {
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

    int64_t MB2Locator::getCutDigitCount(const complex<dex> &an) { return rff_math::log10Approx(an.norm_approx()); }
    void MB2Locator::prepareApproxAmplitudes() {
        complex<dex> an = complex<dex>::ONE;
        for (uint32_t i = 0; i < static_cast<uint32_t>(blockResults.size()); ++i) {
            approxAmplitudes[i] = an;
            an *= blockResults[i].fzgAn;
            an = an.try_normalized_value();
        }
    }

    void MB2Locator::setExp10(fixed_point_complex &dc, std::vector<fixed_point_complex> &tt,
                              std::vector<fixed_point_complex> &ut, const int64_t exp10) {

        currentCenter.set_exp10(exp10);
        dc.set_exp10(exp10);
        temp.set_exp10(exp10);

        for (auto &tt0: tt) {
            tt0.set_exp10(exp10);
        }
        for (auto &ut0: ut) {
            ut0.set_exp10(exp10);
        }
        for (auto &checkpoint: checkpoints) {
            checkpoint.z.set_exp10(exp10);
        }
        for (auto &blockResult: blockResults) {
            blockResult.an.set_exp10(exp10);
            blockResult.bn.set_exp10(exp10);
            blockResult.residual.set_exp10(exp10);
        }
    }

    bool MB2Locator::checkAndUpdateHistory(std::array<int64_t, EXP10_HISTORY_LENGTH> &exp10History, const int64_t dcCurrExp10,
                                           const bool burst) {
        for (uint32_t i = 1; i < static_cast<uint32_t>(exp10History.size()); ++i) {
            exp10History[i - 1] = exp10History[i];
        }
        exp10History.back() = dcCurrExp10;
        if (exp10History.front() - dcCurrExp10 <= 1 || dcCurrExp10 - exp10History[exp10History.size() - 2] > 10) {
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

    void MB2Locator::solvePartitionsParallel(const int64_t aimExp10, const int64_t dcCurrExp10) {


        std::vector<std::unique_ptr<std::jthread>> threadPool(threads);
        // ReSharper disable once CppTooWideScope
        std::mutex partitionPickerMutex;
        // ReSharper disable once CppTooWideScope
        uint32_t processedPartition = 0;


        for (uint32_t i = 0; i < threadPool.size(); ++i) {
            threadPool[i] = std::make_unique<std::jthread>(
                    [this, dcCurrExp10, aimExp10, &partitionPickerMutex, &processedPartition, i] {
                        status[i] = processPartitions(dcCurrExp10, aimExp10, partitionPickerMutex, processedPartition);
                    });
        }

        // wait for complete
        for (const auto &thread: threadPool) {
            if (thread->joinable()) {
                thread->join();
            }
        }
    }
    void MB2Locator::refreshInfos(const complex<dex> &fzgAn, const complex<dex> &fpgBn, const fixed_point_complex &dc,
                                  complex<dex> &mbScale, double &aimLogZoom, int64_t &aimExp10, dex &dcd) {
        mbScale = fzgAn * fpgBn;
        aimLogZoom = rff_math::log10(mbScale.norm_approx());
        aimExp10 = Perturbator::logZoomToExp10(aimLogZoom + MB2_LOG_ZOOM_OFFSET);
        dcd = static_cast<complex<dex>>(dc).norm_approx();
    }

    bool MB2Locator::shouldAbort(const ParallelRenderState &state, const std::vector<PartitionStatus> &status) {
        if (state.interruptRequested()) {
            return true;
        }

        return std::ranges::any_of(status, [](const PartitionStatus &s) {
            return s != PartitionStatus::SUCCESS;
        });
    }

    std::optional<MB2LocateResult> MB2Locator::locate() {

        const int64_t refExp10 = Perturbator::logZoomToExp10(refLogZoom);
        double aimLogZoom = refLogZoom * 2;
        int64_t aimExp10 = Perturbator::logZoomToExp10(aimLogZoom + MB2_LOG_ZOOM_OFFSET);

        // copy checkpoints
        blockResults.resize(checkpoints.size() - 1, BlockResult{
                                                            .residual = fixed_point_complex(0.0, 0.0, refExp10),
                                                            .fzgAn = complex<dex>::ONE,
                                                    });

        for (uint32_t i = 0; i < blockResults.size(); i++) {
            blockResults[i].fzgAn = checkpoints[i + 1].fzgAn;
        }


        if (locSettings.crvp) knownOrbit.resize(checkpoints.back().refIteration);
        approxAmplitudes.resize(checkpoints.size() - 1);
        status.resize(threads);

        complex<dex> fzgAn = complex<dex>::ONE;
        complex<dex> fpgBn = complex<dex>::ZERO;

        std::vector tt(checkpoints.size(), fixed_point_complex{0.0, 0.0, refExp10});
        std::vector ut(checkpoints.size(), fixed_point_complex{0.0, 0.0, refExp10});

        currentCenter.set_exp10(refExp10);
        temp.set_exp10(refExp10);


        fixed_point_complex dc(0.0, 0.0, refExp10);

        calcCenterOffset(dc, checkpoints.back().z.create_variant(refExp10), fixed_point_complex(refFpgBn, refExp10));
        fixed_point_complex::add(currentCenter, currentCenter, dc);

        dex dcd = static_cast<complex<dex>>(dc).norm_approx();
        complex<dex> mbScale = complex<dex>::ONE;

        std::array<int64_t, EXP10_HISTORY_LENGTH> exp10History{};

        if (refDcMax < dcd) {
            vkh::logger::log_err("Center could not be found");
            return std::nullopt;
        }

        do {

            const int64_t dcCurrExp10 = dcd.is_zero() ? aimExp10 : rff_math::log10Approx(dcd);
            if (!checkAndUpdateHistory(exp10History, dcCurrExp10, locSettings.burst)) {
                return std::nullopt;
            }

            prepareApproxAmplitudes();
            solvePartitionsParallel(aimExp10, dcCurrExp10);
            if (shouldAbort(state, status))
                return std::nullopt;
            setExp10(dc, tt, ut, aimExp10);
            calculateAmplitudes(fzgAn, fpgBn, tt, ut);
            translateCenter(dc, tt.back(), ut.back());
            rebaseCheckpoints(dc, tt, ut);
            refreshInfos(fzgAn, fpgBn, dc, mbScale, aimLogZoom, aimExp10, dcd);

            if (mbScale.is_zero()) {
                vkh::logger::log_err("minibrot size cannot be measured");
                return std::nullopt;
            }

        } while (rff_math::log10(dcd) > -aimLogZoom);

        const auto resultLogZoom = aimLogZoom + MB2_LOG_ZOOM_OFFSET;

        return MB2LocateResult{.center = std::move(currentCenter), .logZoom = resultLogZoom};
    }
} // namespace merutilm::rff2
