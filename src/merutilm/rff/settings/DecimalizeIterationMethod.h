//
// Created by Merutilm on 2025-05-04.
//

#pragma once

namespace merutilm::rff {
    enum class DecimalizeIterationMethod {
        /**
         * Do Not Use Decimal Iterations.
         */
        NONE,
        /**
         * Use triangle inequality once.
         */
        LINEAR,
        /**
         * Calculates <b>Sqrt(Linear)</b>.
         */
        SQUARE_ROOT,
        /**
         * Calculates <b>Log(Linear + 1)</b>.
         */
        LOG,
        /**
         * Calculates <b>Log(Log(Linear + 1) + 1)</b>.
         */
        LOG_LOG
    };
}
