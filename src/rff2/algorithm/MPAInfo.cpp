//
// Created by Merutilm on 2025-05-11.
//

#include "MPAInfo.hpp"
#include <algorithm>

#include "../util/ArrayUtils.hpp"
#include "MPAIndexMapperUtils.hpp"
#include "srpc_module/SRPCUnit.hpp"


namespace merutilm::rff2 {
    MPAInfo::MPAInfo(const std::vector<itt::ref> &referencePeriod, const ReferenceCompressor &referenceCompressor,
                     const FrtMPASettings &mpaSettings) {

        generateTablePeriod(referencePeriod, mpaSettings);
        generateCountInfos();
        generatePulledMPACompressor(referenceCompressor);
        generateCompressionInfo();
        printDebugInfo();
    }


    void MPAInfo::generateCountInfos() {
        // index compression : [3, 11, 26, 77] // index compression : [3, 11, 26, 77]
        // startIteration : 1  4  7 12 15 18 23 27
        // index :          0  1  2  3  4  5  6  7

        // 3 6 9 12 15 18 21 24 -> 3 6 9 -> 1 4 7 (11/3 = 3.xxx)
        // 11 22 33 44 55 66 -> 11, 22 -> 1 12 (26/11 = 2.xxx)
        // 26 52 78 104 130 -> 26, 52 -> 1 27 (77/26 = 2.xxx)
        //
        // the remainder of [2]/[1] can also be divided by smaller period.
        // it can be recursive.
        //
        // skippable iteration counts
        // period 11 : 11/3 =3.xxx (3 elements) elements = 3,
        // period 26 : 26/11=2.xxx (3*2 elements), 26%11 = 4, 4/3 = 1.xxx (1 element) elements = 3*2+1=7
        // period 77 : 77/26=2.xxx (7*2 elements), 77%26 = 25,25/11= 2.xxx (3*2 elements), 25%11=4, 4/3 = 1.xxx (1
        // element), elements = 7*2+3*2+1=21
        //
        // period element counts
        // period 11 : 11/3 =3.xxx (3 elements) elements = 3 + 1(self) = 4,
        // period 26 : 26/11=2.xxx (4*2 elements), 26%11 = 4, 4/3 = 1.xxx (1 element) elements = 4*2+1+1(self)=10
        // period 77 : 77/26=2.xxx (10*2 elements), 77%26 = 25,25/11= 2.xxx (4*2 elements), 25%11=4, 4/3 = 1.xxx (1
        // element), elements = 10*2+4*2+1+1(self)=30
        //
        // Stored elements to memory

        const size_t size = tablePeriods.size();
        tableElementCounts = std::vector<itt::ref>(size, 0);
        skippableIterationCounts = std::vector<itt::ref>(size, 0);
        tableElementCounts[0] = 1;
        skippableIterationCounts[0] = 1;

        for (size_t i = 1; i < size; ++i) {
            itt::ref tableElement = 0;
            itt::ref skippableIterationCount = 0;
            itt::ref remainder = tablePeriods[i];
            for (size_t j = i; j > 0; --j) {
                const itt::ref groupAmount = remainder / tablePeriods[j - 1];
                remainder %= tablePeriods[j - 1];
                tableElement += groupAmount * tableElementCounts[j - 1]; // lower level pa
                skippableIterationCount += groupAmount * skippableIterationCounts[j - 1];
            }
            tableElementCounts[i] = tableElement + 1; // current level pa
            skippableIterationCounts[i] = skippableIterationCount;
        }
    }

    void MPAInfo::generateTablePeriod(const std::vector<itt::ref> &referencePeriod, const FrtMPASettings &mpaSettings) {
        // example
        // period : [3, 11, 26]
        //
        // -- : The space of MPA
        //
        // It : 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 ... longestPeriod
        //
        // 03 : 00 01 02 03 01 02 03 01 02 03 -- -- 01 02 03 01 02 03 01 02 03 -- -- 01 02 03 -- ...
        // 11 : 00 01 02 03 04 05 06 07 08 09 10 11 01 02 03 04 05 06 07 08 09 10 11 -- -- -- -- ...
        // 26 : 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 ...
        //
        //
        // Skips
        //  1 + 2 |  1 + 10 |  1 + 25
        //  4 + 2 | 12 + 10 | 27 + 25
        //  7 + 2 | 27 + 10
        // 12 + 2
        // 15 + 2
        // 18 + 2
        // 23 + 2
        // 27 + 2
        // ...

        const int maxMultiplier = mpaSettings.maxMultiplierBetweenLevel;
        const int minSkip = mpaSettings.minSkipReference;
        const itt::ref longestPeriod = referencePeriod[referencePeriod.size() - 1];

        itt::ref currentRefPeriod = minSkip;

        tablePeriods = {};
        isArtificial = {};
        tablePeriods.push_back(currentRefPeriod);
        isArtificial.push_back(!std::ranges::binary_search(referencePeriod, currentRefPeriod));

        // first period is always minimum skip iteration when the longest period is larger than this,
        // and it is artificially-created period if generated period is not an element of generated period.

        for (itt::ref p: referencePeriod) {
            // Generate Period Array

            if (p >= minSkip &&
                (p == longestPeriod && currentRefPeriod != longestPeriod || currentRefPeriod * maxMultiplier <= p)) {
                // If next valid period is "maxMultiplierBetweenLevel^2" times larger than "currentPeriod",
                // add currentPeriod * "maxMultiplierBetweenLevel" period
                // until the multiplier between level is lower than the square of "maxMultiplierBetweenLevel".
                // It is artificially-created period.

                while (currentRefPeriod >= minSkip && currentRefPeriod * maxMultiplier * maxMultiplier < p) {

                    tablePeriods.push_back(currentRefPeriod * maxMultiplier);
                    isArtificial.push_back(true);

                    currentRefPeriod *= maxMultiplier;
                }

                // Otherwise, add generated period to period array.

                tablePeriods.push_back(p);
                isArtificial.push_back(false);
                currentRefPeriod = p;
            }
        }
    }

    void MPAInfo::generatePulledMPACompressor(const ReferenceCompressor &referenceCompressor) {
        size_t compressed = 0;
        for (const auto &unit: referenceCompressor) {
            const itt::ref start = unit.start;
            const itt::ref length = unit.range();
            const size_t index = ArrayUtils::binarySearch(tablePeriods, length + 1);

            // Check if the reference compressor is same as period.
            // However, The Computer doesn't know whether the compressor's length came from skipping to the periodic
            // point, or being cut off in the middle. So, Do check tableIndex too.

            if (const auto [pulledIndex, _] = MPAIndexMapperUtils::iterationToPulledTableIndexMapper(*this, start);
                index != UINT64_MAX && pulledIndex != UINT64_MAX && !isArtificial[index]) {
                const itt::ref skippableIterationCount = skippableIterationCounts[index];
                compressed += skippableIterationCount - 1;
                auto end = pulledIndex + skippableIterationCount - 1;
                pulledMPACompressor.emplace_back(pulledIndex + 1, end, end - compressed + 1);
            }
        }
    }


    void MPAInfo::generateCompressionInfo() {
        const size_t size = tablePeriods.size();
        compressedSkippableIterationCounts = std::vector<itt::ref>(size, 0);
        compressedSkippableIterationCounts[0] = 1;
        compressionUnitCounts = std::vector<size_t>(size, 0);

        for (size_t i = 1; i < size; ++i) {
            const itt::ref sic = skippableIterationCounts[i];
            itt::ref actualSic = sic;
            for (size_t j = 0; j < pulledMPACompressor.size() && sic > pulledMPACompressor[j].start; ++j) {
                const auto &comp = pulledMPACompressor[j];
                const size_t level = ArrayUtils::binarySearch(skippableIterationCounts, comp.range() + 1);
                assert(level != UINT64_MAX && level < i);
                actualSic -= skippableIterationCounts[level] - 1;
                ++compressionUnitCounts[i];
            }
            compressedSkippableIterationCounts[i] = actualSic;
        }
    }
    void MPAInfo::printDebugInfo() {
#ifndef NDEBUG
        std::cout << "======================= MPA GENERATION ===========================\n";
        for (int i = 0; i < tablePeriods.size(); ++i) {
            std::cout << "Period " << tablePeriods[i] << " Info"
                      << "\n\tSkippable Count : " << skippableIterationCounts[i]
                      << "\n\tTable Element Count : " << tableElementCounts[i]
                      << "\n\tArtificial : " << (isArtificial[i] ? "yes" : "no")
                      << "\n\tCompressed Skippable Iterations : " << compressedSkippableIterationCounts[i] << "\n\n";
        }

        std::cout << "=================== Pulled MPA Compressor Info ===================\n";
        for (int i = 0; i < pulledMPACompressor.size(); ++i) {
            std::cout << "- Compressor " << i << "\n\tCompressed Area : " << pulledMPACompressor[i].start << "-"
                      << pulledMPACompressor[i].end << "\n\tReferencing Area : 1-" << pulledMPACompressor[i].range()
                      << "\n\tSRPC Recovering to : " << pulledMPACompressor[i].recoverIndex << "\n\n";
        }
        std::cout << std::flush;

#endif
    }

} // namespace merutilm::rff2
