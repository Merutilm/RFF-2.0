//
// Created by Merutilm on 2025-05-14.
//

#include "RFFSettingsMenu.h"

#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

#include "RFFCallbackFile.h"
#include "RFFCallbackFractal.h"
#include "RFFRenderScene.h"


RFFSettingsMenu::RFFSettingsMenu(const HMENU hMenubar) : menubar(hMenubar) {
    configure();
}

RFFSettingsMenu::~RFFSettingsMenu() {
    DestroyMenu(menubar);
    for (const auto &menu: childMenus) {
        DestroyMenu(menu);
    }
}


void RFFSettingsMenu::configure() {
    HMENU currentMenu = nullptr;

    currentMenu = addChildMenu(menubar, "File");
    addChildItem(currentMenu, "Open Map", RFFCallbackFile::OPEN_MAP);
    addChildItem(currentMenu, "Save Map", RFFCallbackFile::SAVE_MAP);
    addChildItem(currentMenu, "Save Image", RFFCallbackFile::SAVE_IMAGE);

    currentMenu = addChildMenu(menubar, "Fractal");
    addChildItem(currentMenu, "Reference", RFFCallbackFractal::REFERENCE);
    addChildItem(currentMenu, "Iterations", RFFCallbackFractal::ITERATIONS);
    addChildItem(currentMenu, "MPA", RFFCallbackFractal::MPA);
    addChildItem(currentMenu, "Automatic Iterations", RFFCallbackFractal::AUTOMATIC_ITERATIONS);
    addChildItem(currentMenu, "Absolute Iteration Mode", RFFCallbackFractal::ABSOLUTE_ITERATION_MODE);

    currentMenu = addChildMenu(menubar, "Image");
    currentMenu = addChildMenu(menubar, "Shader");
    currentMenu = addChildMenu(menubar, "Preset");
    currentMenu = addChildMenu(menubar, "Video");
    currentMenu = addChildMenu(menubar, "Explore");
}


HMENU RFFSettingsMenu::addChildMenu(const HMENU target, const std::string_view child) {
    return add(target, child, [](const RFFSettingsMenu&, const RFFRenderScene &) {
        //it is "MENU", The callback not required
    }, true);
}

HMENU RFFSettingsMenu::addChildItem(const HMENU target, const std::string_view child,
                                    const std::function<void(RFFSettingsMenu&, RFFRenderScene &)> &callback) {
    return add(target, child, callback, false);
}

HMENU RFFSettingsMenu::add(const HMENU target, const std::string_view child,
                           const std::function<void(RFFSettingsMenu&, RFFRenderScene &)> &callback, const bool hasChild) {
    const HMENU hmenu = CreateMenu();

    if (hasChild) {
        AppendMenu(target, MF_POPUP, reinterpret_cast<UINT_PTR>(hmenu), child.data());
    } else {
        AppendMenu(target, 0, RFFConstants::Win32::ID_MENUS + count++, child.data());
        callbacks.emplace_back(callback);
    }

    childMenus.push_back(hmenu);
    return hmenu;
}

void RFFSettingsMenu::executeAction(RFFRenderScene &scene, const WPARAM wparam) {
    if (currentActiveSettingsWindow != nullptr) {
        MessageBox(currentActiveSettingsWindow->getWindow(), "Failed to open settings. Close the previous settings window.", "Warning", MB_OK | MB_ICONWARNING);
        return;
    }
    if (const auto id = LOWORD(wparam) - RFFConstants::Win32::ID_MENUS;
        id >= 0 && id < callbacks.size()
    ) {
        callbacks[id](*this, scene);
    }
}

void RFFSettingsMenu::setCurrentActiveSettingsWindow(std::unique_ptr<RFFSettingsWindow> &&scene) {
    currentActiveSettingsWindow = std::move(scene);
}
