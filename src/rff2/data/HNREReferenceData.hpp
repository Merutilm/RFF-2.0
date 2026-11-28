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
        std::vector<itt::ref> generatedPeriods;
        complex<dex> dcTransform;
        complex<dex> dzTransform;
        fixed_point_complex transformedC{};


        HNREDataBase() = default;

        virtual ~HNREDataBase() = default;

        explicit HNREDataBase(std::vector<itt::ref> &&generatedPeriods) :
            generatedPeriods(std::move(generatedPeriods)) {}

        virtual itt::ref longestPeriod() const = 0;
    };

    // HNRE (Hierarchical Nested Reference Estimation) Info
    template<Number Num>
    struct HNREReferenceData : HNREDataBase, SRPCData<itt::ref, complex<Num>> {

        Num scale;

        HNREReferenceData() = default;

        explicit HNREReferenceData(SRPCData<itt::ref, complex<Num>> &&data, fixed_point_complex &z,
                                   fixed_point_complex &c, fixed_point_complex &fzgAn, const complex<dex> &fpgBn,
                                   const dex transformedDcMax, const int64_t exp10,
                                   std::array<fixed_point_decimal, fixed_point_util::TEMPS_COUNT> &temps,
                                   std::vector<itt::ref> &&generatedPeriods) :
            HNREDataBase(std::move(generatedPeriods)), SRPCData<itt::ref, complex<Num>>(std::move(data)) {

            mbNucleusOff.set_exp10(exp10);
            mbNucleus.set_exp10(exp10);
            transformedC.set_exp10(exp10);
            fzgAn.set_exp10(exp10);
            z.set_exp10(exp10);
            c.set_exp10(exp10);

            for (auto &temp: temps) {
                temp.set_exp10(exp10, false);
            }

            auto fzgAn0 = static_cast<complex<dex>>(fzgAn);
            dcTransform = fzgAn0 * fpgBn;
            dzTransform = fzgAn0;

            ReferenceUtils::calcCenterOffset(mbNucleusOff, z, fixed_point_complex(fpgBn.re, fpgBn.im, exp10), temps);

            fixed_point_complex::mul(transformedC, fzgAn, z, temps);
            fixed_point_complex::add(mbNucleus, c, mbNucleusOff);
            scale = Num(std::max(transformedDcMax, estimateDcMaxScale()));
        }

        [[nodiscard]] const std::vector<complex<Num>> &getCompressedArray() const {
            return SRPCData<itt::ref, complex<Num>>::compressedArray;
        }
        [[nodiscard]] const ReferenceCompressor &getCompressor() const {
            return SRPCData<itt::ref, complex<Num>>::compressor;
        }

        [[nodiscard]] itt::ref longestPeriod() const override { return generatedPeriods.back(); }


        [[nodiscard]] HNREReferenceView<Num> makeReferenceView() const;

    private:
        [[nodiscard]] dex estimateDcMaxScale() const {
            return dex(32.0) / dcTransform.norm();
        }
    };
} // namespace merutilm::rff2
