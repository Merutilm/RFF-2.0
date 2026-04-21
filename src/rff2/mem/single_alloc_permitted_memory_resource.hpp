//
// Created by Merutilm on 2026-04-21.
//

#pragma once
#include <iostream>
#include <memory_resource>
namespace merutilm::rff2 {
    struct single_alloc_permitted_memory_resource final : std::pmr::memory_resource {

        memory_resource * resource;
        bool once_allocated = false;
        explicit single_alloc_permitted_memory_resource() : resource(std::pmr::get_default_resource()) {
        }

        void *do_allocate(std::size_t __bytes, std::size_t __alignment) override {
            if (once_allocated) {
                std::cerr << "2nd allocation is not allowed" << std::endl;
                throw std::bad_alloc();
            }
            once_allocated = true;
            return resource->allocate(__bytes, __alignment);
        };
        void do_deallocate(void *__p, std::size_t __bytes, std::size_t __alignment) override {
            resource->deallocate(__p, __bytes, __alignment);
        };
        bool do_is_equal(const memory_resource &__other) const noexcept override {
            return resource->is_equal(__other);
        }
    };
}