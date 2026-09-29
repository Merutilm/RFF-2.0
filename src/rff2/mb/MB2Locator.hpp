//
// Created by Merutilm on 2025-05-16.
// Sensitivity-Tapered Multiple Shooting (STMS) Algorithm by GPT-6 Astra on 2026-09-10
// Re-Implemented by Merutilm on 2026-09-13

#pragma once
#include "../parallel/ParallelRenderState.h"
#include "../settings/ExpLocatorSettings.hpp"
#include "MB2RenderData.hpp"

namespace merutilm::rff2 {
    struct MB2LocateResult {
        fixed_point_complex center;
        double logZoom{};
    };

    struct BlockResult {
        fixed_point_complex residual;
        fixed_point_complex an;
        fixed_point_complex bn;
        complex<dex> fzgAn;
    };

    struct ThreadTempCache {
        fixed_point_complex z;
        fixed_point_complex zExpected;
        fixed_point_complex c;
        std::array<fixed_point_decimal, fixed_point_complex::TEMPS_COUNT> temps;
    };

    enum class PartitionStatus : uint8_t { SUCCESS, INTERRUPTED, ERROR_BURST_Z_ESCAPED };

    class MB2Locator {
        static constexpr double MB2_LOG_ZOOM_OFFSET = 2.f;
        static constexpr uint32_t EXP10_HISTORY_LENGTH = 10;

        const ParallelRenderState &state;
        const MB2ReferenceBase &reference;
        const ExpLocatorSettings locSettings;
        const uint32_t threads;
        fixed_point_complex prevCenter;
        fixed_point_complex currCenter;
        fixed_point_complex temp;
        std::vector<ReferenceCheckpoint> checkpoints;
        std::vector<ThreadTempCache> threadTempCaches;
        std::vector<BlockResult> blockResults;
        std::vector<complex<dex>> approxAmplitudes;
        std::vector<PartitionStatus> status;
        FnListeners::FnLocatingMB2W fnLocatingMB2W;
    public:
        explicit MB2Locator(const ParallelRenderState &state, const MB2RenderDataBase &data, const ExpLocatorSettings &locSettings,
                            FnListeners::FnLocatingMB2W &&fnLocatingMB2W);

        std::optional<MB2LocateResult> locate();

        static fixed_point_complex calcCenterOffset(const MB2ReferenceBase &reference);

    private:

        static void calcCenterOffset(fixed_point_complex &result, const fixed_point_complex &z,
                                     const fixed_point_complex &bn, std::array<fixed_point_decimal, 6> &temps);

        PartitionStatus processPartition(uint32_t partitionIndex, int64_t dcCurrExp10, int64_t aimExp10,
                                         ThreadTempCache &cache);

        PartitionStatus processPartitions(int64_t dcCurrExp10, int64_t aimExp10, std::mutex &partitionPickerMutex,
                                          uint32_t &processedPartition, ThreadTempCache &cache);

        void translateCenter(fixed_point_complex &dc, const fixed_point_complex &t, const fixed_point_complex &u);

        void rebaseCheckpoints(const fixed_point_complex &dc, const std::vector<fixed_point_complex> &tt,
                               const std::vector<fixed_point_complex> &ut);

        void calculateAmplitudes(complex<dex> &fzgAn, complex<dex> &fpgBn, std::vector<fixed_point_complex> &tt,
                                 std::vector<fixed_point_complex> &ut);

        static int64_t getCutDigitCount(const complex<dex> &an);

        void prepareApproxAmplitudes();
        void reserveExp10(fixed_point_complex &dc, std::vector<fixed_point_complex> &tt,
                          std::vector<fixed_point_complex> &ut, int64_t aimExp10);

        void setExp10(fixed_point_complex &dc, std::vector<fixed_point_complex> &tt,
                      std::vector<fixed_point_complex> &ut, int64_t exp10);
        static bool checkAndUpdateHistory(std::array<int64_t, EXP10_HISTORY_LENGTH> &exp10History, int64_t dcCurrExp10, bool burst);

        void solvePartitionsParallel(int64_t aimExp10, int64_t dcCurrExp10);

        static void refreshInfos(const complex<dex> &fzgAn, const complex<dex> &fpgBn, const fixed_point_complex &dc,
                                 complex<dex> &mbScale, double &aimLogZoom, int64_t &aimExp10, dex &dcd);

        static bool shouldAbort(const ParallelRenderState &state, const std::vector<PartitionStatus> &status);

    };
} // namespace merutilm::rff2
