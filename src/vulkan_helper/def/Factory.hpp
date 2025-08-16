//
// Created by Merutilm on 2025-08-16.
//

#pragma once
#include <memory>

namespace merutilm::vkh {
    class Factory {
        template<typename T>
        struct UniquePointerTypeGetter {
            using Type = T;
        };

        template<typename T>
        struct UniquePointerTypeGetter<std::unique_ptr<T> > {
            using Type = T;
        };

    public:
        template<typename Ptr, typename... Args>
        static Ptr create(Args &&... args) {
            using T = typename UniquePointerTypeGetter<Ptr>::Type;
            return std::make_unique<T>(std::forward<Args>(args)...);
        }
    };
}
