//
// Created by Merutilm on 9/14/26.
//

#pragma once
#include "../calc/iteration_types.hpp"
#include <type_traits>

namespace merutilm::rff2::FnListeners {


    using FnRefCalcW = std::function<void(itt::ref)>;
    template<typename F>
    concept FnRefCalc = std::is_convertible_v<F, FnRefCalcW>;

    using FnSeriesApproxW = std::function<void(float)>;
    template<typename F>
    concept FnSeriesApprox = std::is_convertible_v<F, FnSeriesApproxW>;

    using FnCreatingTableW = std::function<void(float)>;
    template<typename F>
    concept FnCreatingTable = std::is_convertible_v<F, FnCreatingTableW>;

    using FnLocatingMB2W = std::function<void(int64_t, uint32_t, uint32_t)>;
    template<typename F>
    concept FnLocatingMB2 = std::is_convertible_v<F, FnLocatingMB2W>;

} // namespace merutilm::rff2::FnListeners
