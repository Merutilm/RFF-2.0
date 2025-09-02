//
// Created by Merutilm on 2025-08-16.
//

#pragma once
#include "vkh_base.hpp"

namespace merutilm::vkh {
    class factory {
        template<typename T>
        struct unique_pointer_type_getter {
            using type = T;
        };

        template<typename T>
        struct unique_pointer_type_getter<std::unique_ptr<T> > {
            using type = T;
        };

    public:
        template<typename Ptr, typename... Args>
        static Ptr create(Args &&... args) {
            using T = typename unique_pointer_type_getter<Ptr>::type;
            return std::make_unique<T>(std::forward<Args>(args)...);
        }
    };
}
