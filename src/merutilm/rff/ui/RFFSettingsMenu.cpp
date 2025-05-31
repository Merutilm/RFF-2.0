//
// Created by Merutilm on 2025-05-14.
//

#include "RFFSettingsMenu.h"

#include <assert.h>
#include <functional>
#include <vector>
#include <string>
#include <windows.h>

#include "RFFCallbackExplore.h"
#include "RFFCallbackFile.h"
#include "RFFCallbackFractal.h"
#include "RFFCallbackRender.h"
#include "RFFCallbackShader.h"
#include "RFFRenderScene.h"
#include "../preset/render/RenderPresets.h"
#include "../preset/resolution/ResolutionPresets.h"
#include "../preset/calc/CalculationPresets.h"
#include "../preset/shader/bloom/BloomPresets.h"
#include "../preset/shader/color/ColorPresets.h"
#include "../preset/shader/fog/FogPresets.h"
#include "../preset/shader/palette/PalettePresets.h"
#include "../preset/shader/slope/SlopePresets.h"
#include "../preset/shader/stripe/StripePresets.h"


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
    HMENU subMenu1 = nullptr;
    HMENU subMenu2 = nullptr;

    currentMenu = addChildMenu(menubar, "File");
    addChildItem(currentMenu, "Open Map", RFFCallbackFile::OPEN_MAP);
    addChildItem(currentMenu, "Save Map", RFFCallbackFile::SAVE_MAP);
    addChildItem(currentMenu, "Save Image", RFFCallbackFile::SAVE_IMAGE);

    currentMenu = addChildMenu(menubar, "Fractal");
    addChildItem(currentMenu, "Reference", RFFCallbackFractal::REFERENCE);
    addChildItem(currentMenu, "Iterations", RFFCallbackFractal::ITERATIONS);
    addChildItem(currentMenu, "MPA", RFFCallbackFractal::MPA);
    addChildCheckbox(currentMenu, "Automatic Iterations", RFFCallbackFractal::AUTOMATIC_ITERATIONS);
    addChildCheckbox(currentMenu, "Absolute Iteration Mode", RFFCallbackFractal::ABSOLUTE_ITERATION_MODE);

    currentMenu = addChildMenu(menubar, "Render");
    addChildItem(currentMenu, "Clarity", RFFCallbackRender::SET_CLARITY);
    addChildCheckbox(currentMenu, "Anti-aliasing", RFFCallbackRender::ANTIALIASING);
    currentMenu = addChildMenu(menubar, "Shader");
    addChildItem(currentMenu, "Palette", RFFCallbackShader::PALETTE);
    addChildItem(currentMenu, "Stripe", RFFCallbackShader::STRIPE);
    addChildItem(currentMenu, "Slope", RFFCallbackShader::SLOPE);
    addChildItem(currentMenu, "Color", RFFCallbackShader::COLOR);
    addChildItem(currentMenu, "Fog", RFFCallbackShader::FOG);
    addChildItem(currentMenu, "Bloom", RFFCallbackShader::BLOOM);
    currentMenu = addChildMenu(menubar, "Preset");
    subMenu1 = addChildMenu(currentMenu, "Calculation");
    addPresetExecutor(subMenu1, CalculationPresets::UltraFast());
    addPresetExecutor(subMenu1, CalculationPresets::Fast());
    addPresetExecutor(subMenu1, CalculationPresets::Normal());
    addPresetExecutor(subMenu1, CalculationPresets::Best());
    addPresetExecutor(subMenu1, CalculationPresets::UltraBest());
    addPresetExecutor(subMenu1, CalculationPresets::Stable());
    addPresetExecutor(subMenu1, CalculationPresets::MoreStable());
    addPresetExecutor(subMenu1, CalculationPresets::UltraStable());
    subMenu1 = addChildMenu(currentMenu, "Render");
    addPresetExecutor(subMenu1, RenderPresets::Potato());
    addPresetExecutor(subMenu1, RenderPresets::Low());
    addPresetExecutor(subMenu1, RenderPresets::Medium());
    addPresetExecutor(subMenu1, RenderPresets::High());
    addPresetExecutor(subMenu1, RenderPresets::Ultra());
    addPresetExecutor(subMenu1, RenderPresets::Extreme());
    subMenu1 = addChildMenu(currentMenu, "Resolution");
    addPresetExecutor(subMenu1, ResolutionPresets::L1());
    addPresetExecutor(subMenu1, ResolutionPresets::L2());
    addPresetExecutor(subMenu1, ResolutionPresets::L3());
    addPresetExecutor(subMenu1, ResolutionPresets::L4());
    addPresetExecutor(subMenu1, ResolutionPresets::L5());
    subMenu1 = addChildMenu(currentMenu, "Shader");
    subMenu2 = addChildMenu(subMenu1, "Palette");
    addPresetExecutor(subMenu2, PalettePresets::LongRandom64());
    addPresetExecutor(subMenu2, PalettePresets::Rainbow());
    subMenu2 = addChildMenu(subMenu1, "Stripe");
    addPresetExecutor(subMenu2, StripePresets::SlowAnimated());
    subMenu2 = addChildMenu(subMenu1, "Slope");
    addPresetExecutor(subMenu2, SlopePresets::Normal());
    subMenu2 = addChildMenu(subMenu1, "Color");
    addPresetExecutor(subMenu2, ColorPresets::WeakContrast());
    subMenu2 = addChildMenu(subMenu1, "Fog");
    addPresetExecutor(subMenu2, FogPresets::Medium());
    subMenu2 = addChildMenu(subMenu1, "Bloom");
    addPresetExecutor(subMenu2, BloomPresets::Normal());
    currentMenu = addChildMenu(menubar, "Video");
    currentMenu = addChildMenu(menubar, "Explore");
    addChildItem(currentMenu, "Recompute", RFFCallbackExplore::RECOMPUTE);
    addChildItem(currentMenu, "Refresh Color", RFFCallbackExplore::REFRESH_COLOR);
    addChildItem(currentMenu, "Reset", RFFCallbackExplore::RESET);
    addChildItem(currentMenu, "Cancel", RFFCallbackExplore::CANCEL_RENDER);
    addChildItem(currentMenu, "Find Center", RFFCallbackExplore::FIND_CENTER);
    addChildItem(currentMenu, "Locate Minibrot", RFFCallbackExplore::LOCATE_MINIBROT);
}


HMENU RFFSettingsMenu::addChildMenu(const HMENU target, const std::string_view child) {
    return add(target, child, [](const RFFSettingsMenu &, const RFFRenderScene &) {
        //it is "MENU", The callback not required
    }, true, false, std::nullopt);
}


HMENU RFFSettingsMenu::addChildItem(const HMENU target, const std::string_view child,
                                    const std::function<void
                                        (RFFSettingsMenu &, RFFRenderScene &)> &callback) {
    return add(target, child, callback, false, false, std::nullopt);
}

HMENU RFFSettingsMenu::addChildCheckbox(const HMENU target, const std::string_view child,
                                        const std::function<bool*(RFFRenderScene &)> &checkboxAction) {
    return add(target, child, [](RFFSettingsMenu &, RFFRenderScene &) {
               }, false,
               true, checkboxAction);
}

HMENU RFFSettingsMenu::add(const HMENU target, const std::string_view child,
                           const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> &
                           callback,
                           const bool hasChild, const bool hasCheckbox,
                           const std::optional<std::function<bool*(RFFRenderScene &)> > &checkboxAction) {
    const HMENU hmenu = CreateMenu();

    if (hasChild) {
        AppendMenu(target, MF_POPUP, reinterpret_cast<UINT_PTR>(hmenu), child.data());
    } else {
        AppendMenu(target, 0, RFF::Win32::ID_MENUS + count++, child.data());
        callbacks.emplace_back(callback);
        hasCheckboxes.emplace_back(hasCheckbox);
        checkboxActions.emplace_back(checkboxAction);
    }

    childMenus.push_back(hmenu);
    return hmenu;
}

void RFFSettingsMenu::executeAction(RFFRenderScene &scene, const int menuID) {
    if (currentActiveSettingsWindow != nullptr) {
        MessageBox(currentActiveSettingsWindow->getWindow(),
                   "Failed to open settings. Close the previous settings window.", "Warning", MB_OK | MB_ICONWARNING);
        return;
    }
    if (const auto id = getIndex(menuID);
        checkIndex(id)
    ) {
        callbacks[id](*this, scene);
    }
}

bool RFFSettingsMenu::hasCheckbox(const int menuID) {
    if (const auto id = getIndex(menuID);
        checkIndex(id)
    ) {
        return hasCheckboxes[id];
    }
    return false;
}


void RFFSettingsMenu::setCurrentActiveSettingsWindow(std::unique_ptr<RFFSettingsWindow> &&scene) {
    currentActiveSettingsWindow = std::move(scene);
}


bool *RFFSettingsMenu::getBool(RFFRenderScene &scene, const int menuID) const {
    if (const auto id = getIndex(menuID);
        checkIndex(id)
    ) {
        return checkboxActions[id] == std::nullopt ? nullptr : (*checkboxActions[id])(scene);
    }
    return nullptr;
}


int RFFSettingsMenu::getIndex(const int menuID) {
    return menuID - RFF::Win32::ID_MENUS;
}

bool RFFSettingsMenu::checkIndex(const int index) const {
    return index >= 0 && index < callbacks.size();
}
