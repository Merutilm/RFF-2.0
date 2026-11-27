//
// Created by Merutilm on 11/28/26.
//

#pragma once
#include "../calc/templates.hpp"
#include "../algorithm/MPAInfo.hpp"
#include "../algorithm/PA.h"
#include "ApproxTableBuffer.h"
#include "HNREReferenceData.hpp"

namespace merutilm::rff2 {
    template<Number Num>
    class HNREMPAView;

    template<Number Num>
    struct HNREMPAData {
        const HNREReferenceData<Num> *targetHNRE = nullptr;
        ApproxTableBuffer *buffer = nullptr;
        MPAInfo info;

        // buffer offset
        size_t tableOffset = 0;
        size_t mapperOffset = 0;
        size_t tableSize = 0;
        size_t mapperSize = 0;

        PA<Num> &interpretPA(const size_t index) const {
            return buffer->mpaTable.interpret<PA<Num>>(tableOffset + index);
        }

        MPAIndexMapper &interpretMapper(const size_t index) const {
            return buffer->flattenIndexMapper.interpret<MPAIndexMapper>(mapperOffset + index);
        }

        std::span<PA<Num>> interpretPA(const size_t index, const size_t len) const {
            assert(len < tableSize);
            return buffer->mpaTable.interpret<PA<Num>>(tableOffset + index, len);
        }
        std::span<MPAIndexMapper> interpretMapper(const size_t index, const size_t len) const {
            assert(len < mapperSize);
            return buffer->flattenIndexMapper.interpret<MPAIndexMapper>(mapperOffset + index, len);
        }

        [[nodiscard]] HNREMPAView<Num> makeMPAView() const;
    };
}