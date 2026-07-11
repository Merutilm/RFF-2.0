//
// Created by Merutilm on 2026-05-10.
//

#pragma once

#include "Keybinding.hpp"
#include "Listener.hpp"
#include "PlatformMenuBase.hpp"

namespace merutilm::vkh {
    struct PlatformMenuItemBase : PlatformMenuBase {

        std::vector<Keybinding> keybindings;
        Listener<> onClick;

        explicit PlatformMenuItemBase(std::string name, const PlatformMenuBase * parent);
        ~PlatformMenuItemBase() override;
        PlatformMenuItemBase(const PlatformMenuItemBase &) = delete;
        PlatformMenuItemBase &operator=(const PlatformMenuItemBase &) = delete;
        PlatformMenuItemBase(PlatformMenuItemBase &&other) = delete;
        PlatformMenuItemBase &operator=(PlatformMenuItemBase &&other) = delete;
        void addKeybinding(Keybinding &&keybinding);
        Listener<> &getOnClickListener();

    private:
        void init() override;
        void cleanup() override;
    };
} // namespace merutilm::vkh
