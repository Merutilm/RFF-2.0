//
// Created by Merutilm on 2025-05-04.
//

#pragma once

namespace merutilm::rff2 {
    enum class FrtMPACompressionMethod {
        /**
         * Do Not Compress. It is the fastest but allocates the most memory.
         */
        FASTEST,
        /**
         * Compresses using elements' count each period. Both speed and memory usage are average.
         */
        BALANCED,
        /**
         * Compresses all duplicate elements and speeds up MPA Table creation. It allocates the less memory, but the speed is also slow.
         */
        STRONGEST

    };
}