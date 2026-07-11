//
// Created by Merutilm on 2025-05-14.
//

#pragma once

#ifdef _WIN32
#include "../preset/calc/CalculationPresets.h"
#include "../preset/render/RenderPresets.h"
#include "../preset/resolution/ResolutionPresets.h"
#include "../preset/shader/bloom/ShdBloomPresets.h"
#include "../preset/shader/color/ShdColorPresets.h"
#include "../preset/shader/fog/ShdFogPresets.h"
#include "../preset/shader/palette/ShdPalettePresets.h"
#include "../preset/shader/slope/ShdSlopePresets.h"
#include "../preset/shader/stripe/ShdStripePresets.h"
#include "RFFApplication.hpp"
#include "CallbackExplore.hpp"
#include "CallbackFile.hpp"
#include "CallbackFractal.hpp"
#include "CallbackRender.hpp"
#include "CallbackShader.hpp"
#include "CallbackVideo.hpp"

namespace merutilm::rff2::SettingsMenuGenerator {

    template<typename P>
        requires std::is_base_of_v<Preset, P>
    void addPresetExecutor(vkh::PlatformWindowBase &window, AppRenderManager &appRenderManager,
                           vkh::PlatformMenuBase *subMenu, P preset) {
        window.addMenuItemWithListener(*subMenu, preset.getName(),
                                       [preset, &appRenderManager] { appRenderManager.applyPreset(preset); });
    }

    inline void configure(vkh::PlatformWindowBase &window, AppRenderManager &appRenderManager) {
        vkh::PlatformMenuBase *currentMenu = nullptr;
        vkh::PlatformMenuBase *subMenu1 = nullptr;
        vkh::PlatformMenuBase *subMenu2 = nullptr;

        currentMenu = &window.addMenu(*window.menubar, L"File");
        window.addMenuItemWithListener(*currentMenu, L"Save Map", CallbackFile::fnSaveMap(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Save Image", CallbackFile::fnSaveImage(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Save Location", CallbackFile::fnSaveLocation(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Load Map", CallbackFile::fnLoadMap(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Load Location", CallbackFile::fnLoadLocation(appRenderManager));

        currentMenu = &window.addMenu(*window.menubar, L"Fractal");
        window.addMenuItemWithListener(*currentMenu, L"Reference", CallbackFractal::fnReference(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Iterations", CallbackFractal::fnIterations(appRenderManager));
#ifdef ENABLE_SERIES_APPROXIMATION
        window.addMenuItemWithListener(*currentMenu, L"Series Approximation", CallbackFractal::fnSa(appRenderManager));
#endif
        window.addMenuItemWithListener(*currentMenu, L"MP-Approximation", CallbackFractal::fnMpa(appRenderManager));
        window.addCheckboxMenuItemWithListener(*currentMenu, L"Automatic Iterations",
                                               CallbackFractal::fnGetterAutomaticIterations(appRenderManager),
                                               CallbackFractal::fnAutomaticIterations(appRenderManager));
        window.addCheckboxMenuItemWithListener(*currentMenu, L"Absolute Iteration Mode",
                                               CallbackFractal::fnGetterAbsoluteIterationMode(appRenderManager),
                                               CallbackFractal::fnAbsoluteIterationMode(appRenderManager));

        currentMenu = &window.addMenu(*window.menubar, L"Render");
        window.addMenuItemWithListener(*currentMenu, L"Set Render Properties",
                                       CallbackRender::fnSetRenderProperties(appRenderManager));
        window.addCheckboxMenuItemWithListener(*currentMenu, L"Linear Interpolation",
                                               CallbackRender::fnGetterLinearInterpolation(appRenderManager),
                                               CallbackRender::fnLinearInterpolation(appRenderManager));
        currentMenu = &window.addMenu(*window.menubar, L"Shader");
        window.addMenuItemWithListener(*currentMenu, L"Palette", CallbackShader::fnPalette(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Stripe", CallbackShader::fnStripe(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Slope", CallbackShader::fnSlope(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Color", CallbackShader::fnColor(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Fog", CallbackShader::fnFog(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Bloom", CallbackShader::fnBloom(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Load KFR Color",
                                       CallbackShader::fnLoadKfrPalette(appRenderManager));

        currentMenu = &window.addMenu(*window.menubar, L"Preset");
        subMenu1 = &window.addMenu(*currentMenu, L"Calculation");
        addPresetExecutor(window, appRenderManager, subMenu1, CalculationPresets::UltraFast());
        addPresetExecutor(window, appRenderManager, subMenu1, CalculationPresets::Fast());
        addPresetExecutor(window, appRenderManager, subMenu1, CalculationPresets::Normal());
        addPresetExecutor(window, appRenderManager, subMenu1, CalculationPresets::Best());
        addPresetExecutor(window, appRenderManager, subMenu1, CalculationPresets::UltraBest());
        addPresetExecutor(window, appRenderManager, subMenu1, CalculationPresets::Stable());
        addPresetExecutor(window, appRenderManager, subMenu1, CalculationPresets::MoreStable());
        addPresetExecutor(window, appRenderManager, subMenu1, CalculationPresets::UltraStable());
        subMenu1 = &window.addMenu(*currentMenu, L"Render");
        addPresetExecutor(window, appRenderManager, subMenu1, RenderPresets::Potato());
        addPresetExecutor(window, appRenderManager, subMenu1, RenderPresets::Low());
        addPresetExecutor(window, appRenderManager, subMenu1, RenderPresets::Medium());
        addPresetExecutor(window, appRenderManager, subMenu1, RenderPresets::High());
        addPresetExecutor(window, appRenderManager, subMenu1, RenderPresets::Ultra());
        addPresetExecutor(window, appRenderManager, subMenu1, RenderPresets::Extreme());
        subMenu1 = &window.addMenu(*currentMenu, L"Resolution");
        addPresetExecutor(window, appRenderManager, subMenu1, ResolutionPresets::L1());
        addPresetExecutor(window, appRenderManager, subMenu1, ResolutionPresets::L2());
        addPresetExecutor(window, appRenderManager, subMenu1, ResolutionPresets::L3());
        addPresetExecutor(window, appRenderManager, subMenu1, ResolutionPresets::L4());
        addPresetExecutor(window, appRenderManager, subMenu1, ResolutionPresets::L5());
        subMenu1 = &window.addMenu(*currentMenu, L"Shader");
        subMenu2 = &window.addMenu(*subMenu1, L"Palette");
        addPresetExecutor(window, appRenderManager, subMenu2, ShdPalettePresets::Classic1());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdPalettePresets::Classic2());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdPalettePresets::Azure());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdPalettePresets::Cinematic());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdPalettePresets::Desert());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdPalettePresets::Flame());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdPalettePresets::LongRandom64());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdPalettePresets::LongRainbow7());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdPalettePresets::Rainbow());
        subMenu2 = &window.addMenu(*subMenu1, L"Stripe");
        addPresetExecutor(window, appRenderManager, subMenu2, ShdStripePresets::Disabled());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdStripePresets::SlowAnimated());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdStripePresets::FastAnimated());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdStripePresets::Smooth());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdStripePresets::SmoothTranslucent());
        subMenu2 = &window.addMenu(*subMenu1, L"Slope");
        addPresetExecutor(window, appRenderManager, subMenu2, ShdSlopePresets::Disabled());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdSlopePresets::NoReflection());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdSlopePresets::Reflective());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdSlopePresets::Translucent());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdSlopePresets::Reversed());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdSlopePresets::Micro());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdSlopePresets::Nano());
        subMenu2 = &window.addMenu(*subMenu1, L"Color");
        addPresetExecutor(window, appRenderManager, subMenu2, ShdColorPresets::Disabled());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdColorPresets::WeakContrast());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdColorPresets::HighContrast());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdColorPresets::Dull());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdColorPresets::Vivid());
        subMenu2 = &window.addMenu(*subMenu1, L"Fog");
        addPresetExecutor(window, appRenderManager, subMenu2, ShdFogPresets::Disabled());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdFogPresets::Low());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdFogPresets::Medium());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdFogPresets::High());
        addPresetExecutor(window, appRenderManager, subMenu2, ShdFogPresets::Ultra());
        subMenu2 = &window.addMenu(*subMenu1, L"Bloom");
        addPresetExecutor(window, appRenderManager, subMenu2, BloomPresets::Disabled());
        addPresetExecutor(window, appRenderManager, subMenu2, BloomPresets::Highlighted());
        addPresetExecutor(window, appRenderManager, subMenu2, BloomPresets::HighlightedStrong());
        addPresetExecutor(window, appRenderManager, subMenu2, BloomPresets::Weak());
        addPresetExecutor(window, appRenderManager, subMenu2, BloomPresets::Normal());
        addPresetExecutor(window, appRenderManager, subMenu2, BloomPresets::Strong());
        currentMenu = &window.addMenu(*window.menubar, L"Video");
        window.addMenuItemWithListener(*currentMenu, L"Data Settings", CallbackVideo::fnDataSettings(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Animation Settings",
                                       CallbackVideo::fnAnimationSettings(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Export Settings",
                                       CallbackVideo::fnExportSettings(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Generate Video Keyframe",
                                       CallbackVideo::fnGenerateVidKeyframes(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Export Zooming Video",
                                       CallbackVideo::fnExportZoomVideo(appRenderManager));
        currentMenu = &window.addMenu(*window.menubar, L"Explore");
        window.addMenuItemWithListener(*currentMenu, L"Recompute", CallbackExplore::fnRecompute(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Reset", CallbackExplore::fnReset(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Cancel", CallbackExplore::fnCancelRender(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Find Center", CallbackExplore::fnFindCenter(appRenderManager));
        window.addMenuItemWithListener(*currentMenu, L"Locate Minibrot",
                                       CallbackExplore::fnLocateMinibrot(appRenderManager));
    }


} // namespace merutilm::rff2::SettingsMenuGenerator
#endif