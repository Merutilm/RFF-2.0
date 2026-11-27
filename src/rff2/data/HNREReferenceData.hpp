//
// Created by Merutilm on 11/26/26.
//

#pragma once
#include "../algorithm/ReferenceUtils.hpp"
#include "../algorithm/srpc_module/SRPCData.hpp"
#include "../calc/fixed_point_complex.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    class HNREReferenceView;

    struct HNREDataBase {
        fixed_point_complex mbNucleus{};
        fixed_point_complex mbNucleusOff{};
        fixed_point_complex transformedC{};
        std::vector<itt::ref> generatedPeriods;

        HNREDataBase() = default;

        explicit HNREDataBase(std::vector<itt::ref> &&generatedPeriods) :
            generatedPeriods(std::move(generatedPeriods)) {}
    };

    // HNRE (Hierarchical Nested Reference Estimation) Info
    template<Number Num>
    struct HNREReferenceData : HNREDataBase, SRPCData<itt::ref, complex<Num>> {

        complex<dex> transform;

        HNREReferenceData() = default;

        explicit HNREReferenceData(SRPCData<itt::ref, complex<Num>> &&data, fixed_point_complex &z,
                                   const fixed_point_complex &c, fixed_point_complex &fzgAn,
                                   const complex<dex> &fpgBn, const int64_t exp10,
                                   std::array<fixed_point_decimal, fixed_point_util::TEMPS_COUNT> &temps,
                                   std::vector<itt::ref> &&generatedPeriods) :
            HNREDataBase(std::move(generatedPeriods)), SRPCData<itt::ref, complex<Num>>(std::move(data)) {

            mbNucleusOff.set_exp10(exp10);
            mbNucleus.set_exp10(exp10);
            transformedC.set_exp10(exp10);
            fzgAn.set_exp10(exp10);
            z.set_exp10(exp10);

            for (auto &temp: temps) {
                temp.set_exp10(exp10, false);
            }

            transform = static_cast<complex<dex>>(fzgAn) * fpgBn;

            ReferenceUtils::calcCenterOffset(mbNucleusOff, z, fixed_point_complex(fpgBn.re, fpgBn.im, exp10), temps);

            fixed_point_complex::mul(transformedC, fzgAn, z, temps);

            // std::cout << "[DEBUG]: " << transformedC << std::endl;

            fixed_point_complex::add(mbNucleus, c, mbNucleusOff);
            std::cout << mbNucleus << std::endl;
        }

        [[nodiscard]] const std::vector<complex<Num>> &getCompressedArray() const {
            return SRPCData<itt::ref, complex<Num>>::compressedArray;
        }
        [[nodiscard]] const ReferenceCompressor &getCompressor() const {
            return SRPCData<itt::ref, complex<Num>>::compressor;
        }

        [[nodiscard]] itt::ref longestPeriod() const { return generatedPeriods.back(); }


        [[nodiscard]] HNREReferenceView<Num> makeReferenceView() const;
    };
} // namespace merutilm::rff2
