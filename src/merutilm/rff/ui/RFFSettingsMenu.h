//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>
#include <string_view>
#include <vector>
#include <windef.h>

#include "RFFRenderScene.h"
#include "RFFSettingsWindow.h"

struct RFFSettingsMenu {
    HMENU menubar;
    int count = 0;
    std::vector<HMENU> childMenus = {};
    std::vector<std::function<void(RFFSettingsMenu &, RFFRenderScene &)> > callbacks = {};
    std::vector<bool> hasCheckboxes = {};
    std::vector<std::optional<std::function<bool*(RFFRenderScene &)> > > checkboxActions = {};
    std::unique_ptr<RFFSettingsWindow> currentActiveSettingsWindow = nullptr;

    explicit RFFSettingsMenu(HMENU hMenubar);

    ~RFFSettingsMenu();

    bool hasCheckbox(int menuID);

    RFFSettingsMenu(const RFFSettingsMenu &) = delete;

    RFFSettingsMenu &operator=(const RFFSettingsMenu &) = delete;

    RFFSettingsMenu(RFFSettingsMenu &&) = delete;

    RFFSettingsMenu &operator=(RFFSettingsMenu &&) = delete;

    void configure();

    HMENU addChildMenu(HMENU target, std::string_view child);

    HMENU addChildItem(HMENU target, std::string_view child,
                       const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> &callback);

    HMENU addChildCheckbox(HMENU target, std::string_view child,
                           const std::function<bool*(RFFRenderScene &)>
                           &checkboxAction);

    HMENU add(HMENU target, std::string_view child,
              const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> &callback, bool
              hasChild, bool hasCheckbox, const std::optional<std::function<bool*(RFFRenderScene &)>> &checkboxAction);

    void executeAction(RFFRenderScene &scene, int menuID);

    void setCurrentActiveSettingsWindow(std::unique_ptr<RFFSettingsWindow> &&scene);

    bool* getBool(RFFRenderScene &scene, int menuID) const;

    static int getIndex(int menuID);

    bool checkIndex(int index) const;
};
