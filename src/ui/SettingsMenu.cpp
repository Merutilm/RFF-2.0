//
// Created by Merutilm on 2025-05-14.
//

#include "SettingsMenu.h"

#include <assert.h>
#include <functional>
#include <vector>
#include <string>
#include <windows.h>

#include "Constants.h"
#include "CallbackExplore.h"
#include "CallbackFile.h"
#include "CallbackFractal.h"
#include "CallbackRender.h"
#include "CallbackShader.h"
#include "CallbackVideo.h"
#include "RenderScene.h"
#include "../preset/render/RenderPresets.h"
#include "../preset/resolution/ResolutionPresets.h"
#include "../preset/calc/CalculationPresets.h"
#include "../preset/shader/bloom/BloomPresets.h"
#include "../preset/shader/color/ColorPresets.h"
#include "../preset/shader/fog/FogPresets.h"
#include "../preset/shader/palette/PalettePresets.h"
#include "../preset/shader/slope/SlopePresets.h"
#include "../preset/shader/stripe/StripePresets.h"


namespace merutilm::rff {
    SettingsMenu::SettingsMenu(const HMENU hMenubar) : menubar(hMenubar) {
        configure();
    }

    SettingsMenu::~SettingsMenu() {
        DestroyMenu(menubar);
        for (const auto &menu: childMenus) {
            DestroyMenu(menu);
        }
    }


    void SettingsMenu::configure() {
        HMENU currentMenu = nullptr;
        HMENU subMenu1 = nullptr;
        HMENU subMenu2 = nullptr;

        currentMenu = addChildMenu(menubar, "File");
        addChildItem(currentMenu, "Open Map", CallbackFile::OPEN_MAP);
        addChildItem(currentMenu, "Save Map", CallbackFile::SAVE_MAP);
        addChildItem(currentMenu, "Save Image", CallbackFile::SAVE_IMAGE);

        currentMenu = addChildMenu(menubar, "Fractal");
        addChildItem(currentMenu, "Reference", CallbackFractal::REFERENCE);
        addChildItem(currentMenu, "Iterations", CallbackFractal::ITERATIONS);
        addChildItem(currentMenu, "MPA", CallbackFractal::MPA);
        addChildCheckbox(currentMenu, "Automatic Iterations", CallbackFractal::AUTOMATIC_ITERATIONS);
        addChildCheckbox(currentMenu, "Absolute Iteration Mode", CallbackFractal::ABSOLUTE_ITERATION_MODE);

        currentMenu = addChildMenu(menubar, "Render");
        addChildItem(currentMenu, "Clarity", CallbackRender::SET_CLARITY);
        addChildCheckbox(currentMenu, "Anti-aliasing", CallbackRender::ANTIALIASING);
        currentMenu = addChildMenu(menubar, "Shader");
        addChildItem(currentMenu, "Palette", CallbackShader::PALETTE);
        addChildItem(currentMenu, "Stripe", CallbackShader::STRIPE);
        addChildItem(currentMenu, "Slope", CallbackShader::SLOPE);
        addChildItem(currentMenu, "Color", CallbackShader::COLOR);
        addChildItem(currentMenu, "Fog", CallbackShader::FOG);
        addChildItem(currentMenu, "Bloom", CallbackShader::BLOOM);
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
        addChildItem(currentMenu, "Data Settings", CallbackVideo::DATA_SETTINGS);
        addChildItem(currentMenu, "Animation Settings", CallbackVideo::ANIMATION_SETTINGS);
        addChildItem(currentMenu, "Export Settings", CallbackVideo::EXPORT_SETTINGS);
        addChildItem(currentMenu, "Generate Video Keyframe", CallbackVideo::GENERATE_VID_KEYFRAME);
        addChildItem(currentMenu, "Export Zooming Video", CallbackVideo::EXPORT_ZOOM_VID);
        currentMenu = addChildMenu(menubar, "Explore");
        addChildItem(currentMenu, "Recompute", CallbackExplore::RECOMPUTE);
        addChildItem(currentMenu, "Refresh Color", CallbackExplore::REFRESH_COLOR);
        addChildItem(currentMenu, "Reset", CallbackExplore::RESET);
        addChildItem(currentMenu, "Cancel", CallbackExplore::CANCEL_RENDER);
        addChildItem(currentMenu, "Find Center", CallbackExplore::FIND_CENTER);
        addChildItem(currentMenu, "Locate Minibrot", CallbackExplore::LOCATE_MINIBROT);
    }


    HMENU SettingsMenu::addChildMenu(const HMENU target, const std::string_view child) {
        return add(target, child, [](const SettingsMenu &, const RenderScene &) {
            //it is "MENU", The callback not required
        }, true, false, std::nullopt);
    }


    HMENU SettingsMenu::addChildItem(const HMENU target, const std::string_view child,
                                        const std::function<void
                                            (SettingsMenu &, RenderScene &)> &callback) {
        return add(target, child, callback, false, false, std::nullopt);
    }

    HMENU SettingsMenu::addChildCheckbox(const HMENU target, const std::string_view child,
                                            const std::function<bool*(RenderScene &)> &checkboxAction) {
        return add(target, child, [](SettingsMenu &, RenderScene &) {
                   }, false,
                   true, checkboxAction);
    }

    HMENU SettingsMenu::add(const HMENU target, const std::string_view child,
                               const std::function<void(SettingsMenu &, RenderScene &)> &
                               callback,
                               const bool hasChild, const bool hasCheckbox,
                               const std::optional<std::function<bool*(RenderScene &)> > &checkboxAction) {
        const HMENU hmenu = CreateMenu();

        if (hasChild) {
            AppendMenu(target, MF_POPUP, reinterpret_cast<UINT_PTR>(hmenu), child.data());
        } else {
            AppendMenu(target, 0, Constants::Win32::ID_MENUS + count++, child.data());
            callbacks.emplace_back(callback);
            hasCheckboxes.emplace_back(hasCheckbox);
            checkboxActions.emplace_back(checkboxAction);
        }

        childMenus.push_back(hmenu);
        return hmenu;
    }

    void SettingsMenu::executeAction(RenderScene &scene, const int menuID) {
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

    bool SettingsMenu::hasCheckbox(const int menuID) {
        if (const auto id = getIndex(menuID);
            checkIndex(id)
        ) {
            return hasCheckboxes[id];
        }
        return false;
    }


    void SettingsMenu::setCurrentActiveSettingsWindow(std::unique_ptr<SettingsWindow> &&scene) {
        currentActiveSettingsWindow = std::move(scene);
    }


    bool *SettingsMenu::getBool(RenderScene &scene, const int menuID) const {
        if (const auto id = getIndex(menuID);
            checkIndex(id)
        ) {
            return checkboxActions[id] == std::nullopt ? nullptr : (*checkboxActions[id])(scene);
        }
        return nullptr;
    }


    int SettingsMenu::getIndex(const int menuID) {
        return menuID - Constants::Win32::ID_MENUS;
    }

    bool SettingsMenu::checkIndex(const int index) const {
        return index >= 0 && index < callbacks.size();
    }
}