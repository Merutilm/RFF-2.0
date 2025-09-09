//
// Created by Merutilm on 2025-05-11.
//

#include "MPAPeriod.h"

#include <algorithm>
#include <memory>


namespace merutilm::rff2 {
    MPAPeriod::MPAPeriod(std::vector<uint64_t> &&tablePeriod, std::vector<bool> &&isArtificial, std::vector<uint64_t> &&tableElements) : tablePeriod(std::move(tablePeriod)), isArtificial(std::move(isArtificial)), tableElements(std::move(tableElements)){
    }


    std::vector<uint64_t> MPAPeriod::generatePeriodElements(const std::vector<uint64_t> &tablePeriod) {
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
        // period 11 : 11/3 =3.xxx (3 elements)                                                                              elements = 3
        // period 26 : 26/11=2.xxx (3*2 elements), 26%11 = 4, 4/3 = 1.xxx (1 element)                                        elements = 3*2+1=7
        // period 77 : 77/26=2.xxx (7*2 elements), 77%26 = 25,25/11= 2.xxx (3*2 elements), 25%11=4, 4/3 = 1.xxx (1 element), elements = 7*2+3*2+1=21
        // Stored elements to memory

        const size_t size = tablePeriod.size();
        auto tablePeriodElements = std::vector<uint64_t>(size, 0);

        for (int i = 0; i < size; ++i) {
            if (i == 0) {
                tablePeriodElements[i] = 1;
                continue;
            }
            uint64_t elements = 0;
            uint64_t remainder = tablePeriod[i];
            for (int j = i - 1; j >= 0; j--) {
                const uint64_t groupAmount = remainder / tablePeriod[j];
                remainder %= tablePeriod[j];
                elements += groupAmount * tablePeriodElements[j];
            }
            tablePeriodElements[i] = elements;
        }
        return tablePeriodElements;
    }

    MPAPeriod::Temp MPAPeriod::generateTablePeriod(const std::vector<uint64_t> &referencePeriod, const FrtMPAAttribute &mpaSettings) {
        // example
        // period : [3, 11, 26]
        //
        // -- : The space of R3A
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
        const uint64_t longestPeriod = referencePeriod[referencePeriod.size() - 1];


        std::vector<uint64_t> tablePeriod;
        std::vector<bool> isArtificial;

        uint64_t currentRefPeriod = minSkip;

        tablePeriod.push_back(currentRefPeriod);
        isArtificial.push_back(!std::ranges::binary_search(referencePeriod, currentRefPeriod));

        //first period is always minimum skip iteration when the longest period is larger than this,
        //and it is artificially-created period if generated period is not an element of generated period.

        for (uint64_t p: referencePeriod) {
            //Generate Period Array

            if (p >= minSkip && (p == longestPeriod && currentRefPeriod != longestPeriod || currentRefPeriod * maxMultiplier
                                 <= p)) {
                //If next valid period is "maxMultiplierBetweenLevel^2" times larger than "currentPeriod",
                //add currentPeriod * "maxMultiplierBetweenLevel" period
                //until the multiplier between level is lower than the square of "maxMultiplierBetweenLevel".
                //It is artificially-created period.

                while (currentRefPeriod >= minSkip && currentRefPeriod * maxMultiplier * maxMultiplier < p) {

                    tablePeriod.push_back(currentRefPeriod * maxMultiplier);
                    isArtificial.push_back(true);

                    currentRefPeriod *= maxMultiplier;
                }

                //Otherwise, add generated period to period array.

                tablePeriod.push_back(p);
                isArtificial.push_back(false);
                currentRefPeriod = p;
                                 }
        }

        return Temp{std::move(tablePeriod), std::move(isArtificial)};
    }

    std::unique_ptr<MPAPeriod> MPAPeriod::create(const std::vector<uint64_t> &referencePeriod,
                                                 const FrtMPAAttribute &mpaSettings) {
        auto [tablePeriod, isArtificial] = generateTablePeriod(referencePeriod, mpaSettings);
        auto tablePeriodElements = generatePeriodElements(tablePeriod);
        return std::make_unique<MPAPeriod>(std::move(tablePeriod), std::move(isArtificial), std::move(tablePeriodElements));
    }
}