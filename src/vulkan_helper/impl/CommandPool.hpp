//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class CommandPool final : public CoreHandler {
        VkCommandPool commandPool = {};

    public:
        explicit CommandPool(const Core &core);

        ~CommandPool() override;

        CommandPool(const CommandPool &) = delete;

        CommandPool &operator=(const CommandPool &) = delete;

        CommandPool(CommandPool &&) = delete;

        CommandPool &operator=(CommandPool &&) = delete;

        [[nodiscard]] VkCommandPool getCommandPoolHandle() const {
            return commandPool;
        }

    private:
        void init() override;

        void destroy() override;
    };
}
