//
// Created by Merutilm on 2025-05-04.
//

#pragma once


namespace merutilm::rff2 {
    enum class FrtMPASelectionMethod {
        /**
        * Checks the lowest-level MPA first, increases the level until not valid.
        */
        LOWEST,
        /**
         * Checks the highest-level MPA first. Decreases the level if not valid.
         */
        HIGHEST
    };
}