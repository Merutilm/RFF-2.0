//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class CommandPoolImpl final : public CoreHandler {
        VkCommandPool commandPool = {};

    public:
        explicit CommandPoolImpl(CoreRef core);

        ~CommandPoolImpl() override;

        CommandPoolImpl(const CommandPoolImpl &) = delete;

        CommandPoolImpl &operator=(const CommandPoolImpl &) = delete;

        CommandPoolImpl(CommandPoolImpl &&) = delete;

        CommandPoolImpl &operator=(CommandPoolImpl &&) = delete;

        [[nodiscard]] VkCommandPool getCommandPoolHandle() const {
            return commandPool;
        }

    private:
        void init() override;

        void destroy() override;
    };

    using CommandPool = std::unique_ptr<CommandPoolImpl>;
    using CommandPoolPtr = CommandPoolImpl *;
    using CommandPoolRef = CommandPoolImpl &;
}
