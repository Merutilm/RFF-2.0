//
// Created by Merutilm on 9/14/26.
//

#pragma once
#include <cstdint>
#include <type_traits>

namespace merutilm::rff2::FnListeners {


    using FnRefCalcW = std::function<void(uint64_t)>;
    template<typename F>
    concept FnRefCalc = std::is_convertible_v<F, FnRefCalcW>;

    using FnSeriesApproxW = std::function<void(uint64_t, float)>;
    template<typename F>
    concept FnSeriesApprox = std::is_convertible_v<F, FnSeriesApproxW>;

    using FnCreatingTableW = std::function<void(uint64_t, float)>;
    template<typename F>
    concept FnCreatingTable = std::is_convertible_v<F, FnCreatingTableW>;

    using FnLocatingMB2W = std::function<void(int32_t, uint32_t, uint32_t)>;
    template<typename F>
    concept FnLocatingMB2 = std::is_convertible_v<F, FnLocatingMB2W>;

} // namespace merutilm::rff2::FnListeners
