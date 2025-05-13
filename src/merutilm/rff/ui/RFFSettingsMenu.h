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
    std::vector<std::function<void(RFFSettingsMenu&, RFFRenderScene&)>> callbacks = {};
    std::unique_ptr<RFFSettingsWindow> currentActiveSettingsWindow = nullptr;

    explicit RFFSettingsMenu(HMENU hMenubar);

    ~RFFSettingsMenu();

    RFFSettingsMenu(const RFFSettingsMenu&) = delete;

    RFFSettingsMenu& operator=(const RFFSettingsMenu&) = delete;

    RFFSettingsMenu(RFFSettingsMenu&&) = delete;

    RFFSettingsMenu& operator=(RFFSettingsMenu&&) = delete;

    void configure();

    HMENU addChildMenu(HMENU target, std::string_view child);

    HMENU addChildItem(HMENU target, std::string_view child, const std::function<void(RFFSettingsMenu&, RFFRenderScene &)> &callback);

    HMENU add(HMENU target, std::string_view child, const std::function<void(RFFSettingsMenu&, RFFRenderScene &)> &callback, bool hasChild);

    void executeAction(RFFRenderScene &scene, WPARAM wparam);

    void setCurrentActiveSettingsWindow(std::unique_ptr<RFFSettingsWindow> &&scene);
};
