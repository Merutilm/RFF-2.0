//
// Created by Merutilm on 2025-07-13.
//

#pragma once

#include "Handler.hpp"
#include "../impl/Engine.hpp"

namespace merutilm::vkh {
    struct WindowContextHandler : public Handler {
        WindowContextRef wc;

        explicit WindowContextHandler(WindowContextRef wc) : wc(wc) {};

        ~WindowContextHandler() override = default;

        WindowContextHandler(const WindowContextHandler &) = delete;

        WindowContextHandler &operator=(const WindowContextHandler &) = delete;

        WindowContextHandler(WindowContextHandler &&) = delete;

        WindowContextHandler &operator=(WindowContextHandler &&) = delete;

    };
}
