//
// Created by Merutilm on 2026-05-10.
//

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "vulkan_helper/handle/Handler.hpp"



namespace merutilm::vkh {
    struct PlatformMenuBase : Handler {


        const std::string name;
        const PlatformMenuBase * const parent;
        std::vector<std::unique_ptr<PlatformMenuBase>> items;

        explicit PlatformMenuBase(std::string name, const PlatformMenuBase *parent);
        ~PlatformMenuBase() override;
        PlatformMenuBase(const PlatformMenuBase &) = delete;
        PlatformMenuBase &operator=(const PlatformMenuBase &) = delete;
        PlatformMenuBase(PlatformMenuBase &&other) = delete;
        PlatformMenuBase &operator=(PlatformMenuBase &&other) = delete;

        [[nodiscard]] size_t getItemCount() const {
            return items.size();
        }

    private:
        void init() override;
        void cleanup() override;
    };
} // namespace merutilm::vkh
