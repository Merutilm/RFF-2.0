//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    struct ExternShaderStorage final : Handler{
        BufferContext context;

        explicit ExternShaderStorage(BufferContext context) : context(std::move(context)) {
            ExternShaderStorage::init();
        }

        ~ExternShaderStorage() override {
            ExternShaderStorage::cleanup();
        }

        ExternShaderStorage(const ExternShaderStorage &) = delete;

        ExternShaderStorage operator=(const ExternShaderStorage &) = delete;

        ExternShaderStorage(ExternShaderStorage &&) = delete;

        ExternShaderStorage operator=(ExternShaderStorage &&) = delete;

    protected:
        void init() override {
            //no operation
        }

        void cleanup() override {
            //no operation
        }
    };


}
