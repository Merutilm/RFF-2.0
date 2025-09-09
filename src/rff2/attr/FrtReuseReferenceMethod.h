//
// Created by Merutilm on 2025-05-04.
//

#pragma once

namespace merutilm::rff2 {
    enum class FrtReuseReferenceMethod {
        /**
        * Reuse current reference
        */
        CURRENT_REFERENCE,
        /**
         * Get the centered reference using its period and reuse this.
         */
        CENTERED_REFERENCE,
        /**
         * Do not reuse reference and recalculate reference every perturbator.
         */
        DISABLED
    };
}
