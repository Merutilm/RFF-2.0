//
// Created by Merutilm on 2025-05-14.
//

#include "SettingsMenu.hpp"

#include <assert.h>
#include <functional>
#include <vector>
#include <string>
#include <windows.h>

#include "../constants/Constants.hpp"
#include "CallbackExplore.hpp"
#include "CallbackFile.hpp"
#include "CallbackFractal.hpp"
#include "CallbackRender.hpp"
#include "CallbackShader.hpp"
#include "CallbackVideo.hpp"
#include "RenderScene.hpp"
#include "../preset/render/RenderPresets.h"
#include "../preset/resolution/ResolutionPresets.h"
#include "../preset/calc/CalculationPresets.h"
#include "../preset/shader/bloom/BloomPresets.h"
#include "../preset/shader/color/ColorPresets.h"
#include "../preset/shader/fog/FogPresets.h"
#include "../preset/shader/palette/PalettePresets.h"
#include "../preset/shader/slope/SlopePresets.h"
#include "../preset/shader/stripe/StripePresets.h"


namespace merutilm::rff2 {
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
        addChildItem(currentMenu, "Save Map", CallbackFile::SAVE_MAP);
        addChildItem(currentMenu, "Save Image", CallbackFile::SAVE_IMAGE);
        addChildItem(currentMenu, "Save Location", CallbackFile::SAVE_LOCATION);
        addChildItem(currentMenu, "Load Map", CallbackFile::LOAD_MAP);
        addChildItem(currentMenu, "Load Location", CallbackFile::LOAD_LOCATION);

        currentMenu = addChildMenu(menubar, "Fractal");
        addChildItem(currentMenu, "Reference", CallbackFractal::REFERENCE);
        addChildItem(currentMenu, "Iterations", CallbackFractal::ITERATIONS);
        addChildItem(currentMenu, "MP-Approximation", CallbackFractal::MPA);
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
        addChildItem(currentMenu, "Load KFR Color", CallbackShader::LOAD_KFR_PALETTE);

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
        addPresetExecutor(subMenu2, PalettePresets::Classic1());
        addPresetExecutor(subMenu2, PalettePresets::Classic2());
        addPresetExecutor(subMenu2, PalettePresets::Azure());
        addPresetExecutor(subMenu2, PalettePresets::Cinematic());
        addPresetExecutor(subMenu2, PalettePresets::Desert());
        addPresetExecutor(subMenu2, PalettePresets::Flame());
        addPresetExecutor(subMenu2, PalettePresets::LongRandom64());
        addPresetExecutor(subMenu2, PalettePresets::LongRainbow7());
        addPresetExecutor(subMenu2, PalettePresets::Rainbow());
        subMenu2 = addChildMenu(subMenu1, "Stripe");
        addPresetExecutor(subMenu2, StripePresets::Disabled());
        addPresetExecutor(subMenu2, StripePresets::SlowAnimated());
        addPresetExecutor(subMenu2, StripePresets::FastAnimated());
        addPresetExecutor(subMenu2, StripePresets::Smooth());
        addPresetExecutor(subMenu2, StripePresets::SmoothTranslucent());
        subMenu2 = addChildMenu(subMenu1, "Slope");
        addPresetExecutor(subMenu2, SlopePresets::Disabled());
        addPresetExecutor(subMenu2, SlopePresets::NoReflection());
        addPresetExecutor(subMenu2, SlopePresets::Reflective());
        addPresetExecutor(subMenu2, SlopePresets::Translucent());
        addPresetExecutor(subMenu2, SlopePresets::Reversed());
        addPresetExecutor(subMenu2, SlopePresets::Micro());
        addPresetExecutor(subMenu2, SlopePresets::Nano());
        subMenu2 = addChildMenu(subMenu1, "Color");
        addPresetExecutor(subMenu2, ColorPresets::Disabled());
        addPresetExecutor(subMenu2, ColorPresets::WeakContrast());
        addPresetExecutor(subMenu2, ColorPresets::HighContrast());
        addPresetExecutor(subMenu2, ColorPresets::Dull());
        addPresetExecutor(subMenu2, ColorPresets::Vivid());
        subMenu2 = addChildMenu(subMenu1, "Fog");
        addPresetExecutor(subMenu2, FogPresets::Disabled());
        addPresetExecutor(subMenu2, FogPresets::Low());
        addPresetExecutor(subMenu2, FogPresets::Medium());
        addPresetExecutor(subMenu2, FogPresets::High());
        addPresetExecutor(subMenu2, FogPresets::Ultra());
        subMenu2 = addChildMenu(subMenu1, "Bloom");
        addPresetExecutor(subMenu2, BloomPresets::Disabled());
        addPresetExecutor(subMenu2, BloomPresets::Highlighted());
        addPresetExecutor(subMenu2, BloomPresets::HighlightedStrong());
        addPresetExecutor(subMenu2, BloomPresets::Weak());
        addPresetExecutor(subMenu2, BloomPresets::Normal());
        addPresetExecutor(subMenu2, BloomPresets::Strong());
        currentMenu = addChildMenu(menubar, "Video");
        addChildItem(currentMenu, "Data Settings", CallbackVideo::DATA_SETTINGS);
        addChildItem(currentMenu, "Animation Settings", CallbackVideo::ANIMATION_SETTINGS);
        addChildItem(currentMenu, "Export Settings", CallbackVideo::EXPORT_SETTINGS);
        addChildItem(currentMenu, "Generate Video Keyframe", CallbackVideo::GENERATE_VID_KEYFRAME);
        addChildItem(currentMenu, "Export Zooming Video", CallbackVideo::EXPORT_ZOOM_VID);
        currentMenu = addChildMenu(menubar, "Explore");
        addChildItem(currentMenu, "Recompute", CallbackExplore::RECOMPUTE);
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