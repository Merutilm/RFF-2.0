//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>
#include <string_view>
#include <vector>
#include <windef.h>

#include "RenderScene.hpp"
#include "SettingsWindow.hpp"

namespace merutilm::rff2 {
    struct SettingsMenu {
        HMENU menubar;
        int count = 0;
        std::vector<HMENU> childMenus = {};
        std::vector<std::function<void(SettingsMenu &, RenderScene &)> > callbacks = {};
        std::vector<bool> hasCheckboxes = {};
        std::vector<std::optional<std::function<bool*(RenderScene &, bool)> > > checkboxActions = {};
        std::unique_ptr<SettingsWindow> currentActiveSettingsWindow = nullptr;

        explicit SettingsMenu(HMENU hMenubar);

        ~SettingsMenu();

        bool hasCheckbox(int menuID);

        SettingsMenu(const SettingsMenu &) = delete;

        SettingsMenu &operator=(const SettingsMenu &) = delete;

        SettingsMenu(SettingsMenu &&) = delete;

        SettingsMenu &operator=(SettingsMenu &&) = delete;

        void configure();

        HMENU addChildMenu(HMENU target, std::string_view child);

        HMENU addChildItem(HMENU target, std::string_view child,
                           const std::function<void(SettingsMenu &, RenderScene &)> &callback);

        HMENU addChildCheckbox(HMENU target, std::string_view child,
                               const std::function<bool*(RenderScene &, bool)>
                               &checkboxAction);

        template<typename P> requires std::is_base_of_v<Preset, P>
        HMENU addPresetExecutor(HMENU target, P preset);

        HMENU add(HMENU target, std::string_view child,
                  const std::function<void(SettingsMenu &, RenderScene &)> &callback, bool
                  hasChild, bool hasCheckbox, const std::optional<std::function<bool *(RenderScene &, bool)>> &checkboxAction);

        void executeAction(RenderScene &scene, int menuID);

        void setCurrentActiveSettingsWindow(std::unique_ptr<SettingsWindow> &&scene);

        bool *getBool(RenderScene &scene, int menuID, bool executeMode) const;

        static int getIndex(int menuID);

        bool checkIndex(int index) const;
    };

    template<typename P> requires std::is_base_of_v<Preset, P>
    HMENU SettingsMenu::addPresetExecutor(HMENU target, const P preset) {
        return add(target, preset.getName(), [p1 = std::move(preset)](SettingsMenu &, RenderScene &scene) {
            scene.changePreset(p1);
        }, false, false, std::nullopt);
    }
}