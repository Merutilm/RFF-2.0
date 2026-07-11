//
// Created by Merutilm on 2026-05-10.
//

#pragma once

#include "Keybinding.hpp"
#include "Listener.hpp"
#include "PlatformMenuBase.hpp"

namespace merutilm::vkh {
    struct PlatformCheckboxMenuItemBase : PlatformMenuBase {
        std::vector<Keybinding> keybindings;
        std::function<bool()> checkboxStateGetter;
        Listener<bool> onClick;
        bool checked = false;

        explicit PlatformCheckboxMenuItemBase(std::string name, const PlatformMenuBase * parent);
        ~PlatformCheckboxMenuItemBase() override;
        PlatformCheckboxMenuItemBase(const PlatformCheckboxMenuItemBase &) = delete;
        PlatformCheckboxMenuItemBase &operator=(const PlatformCheckboxMenuItemBase &) = delete;
        PlatformCheckboxMenuItemBase(PlatformCheckboxMenuItemBase &&other) = delete;
        PlatformCheckboxMenuItemBase &operator=(PlatformCheckboxMenuItemBase &&other) = delete;
        void addKeybinding(Keybinding &&keybinding);

        template<typename F> requires std::is_invocable_r_v<bool, F>
        void setCheckboxStateGetter(F && func) {
            checkboxStateGetter = func;
        }

        void refreshChecked() {
            checked = checkboxStateGetter();
        }

        Listener<bool> &getOnClickListener();


    private:
        void init() override;
        void cleanup() override;
    };
} // namespace merutilm::vkh
