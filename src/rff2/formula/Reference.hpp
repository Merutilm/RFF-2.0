//
// Created by Merutilm on 2026-04-16.
//

#pragma once

namespace merutilm::rff2 {

    struct Reference {


        virtual ~Reference() = default;

        /**
         * @return the array length of reference.
         */
        [[nodiscard]] virtual size_t length() const = 0;



        enum class CreationResult {
            SUCCESS, FAILED, TERMINATED
        };

    };
}
