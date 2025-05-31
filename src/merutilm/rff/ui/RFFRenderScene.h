//
// Created by Merutilm on 2025-05-07.
//

#pragma once

#include "../settings/Settings.h"
#include <windows.h>

#include "RFFScene.h"
#include "../data/ApproxTableCache.h"

#include "../data/Matrix.h"
#include "../formula/LightMandelbrotPerturbator.h"
#include "../io/RFFMap.h"
#include "../mrthy/DeepPA.h"
#include "../opengl/GLMultipassRenderer.h"
#include "../opengl/GLRendererAntialiasing.h"
#include "../opengl/GLRendererBloom.h"
#include "../opengl/GLRendererColor.h"
#include "../opengl/GLRendererFog.h"
#include "../opengl/GLRendererIteration.h"
#include "../opengl/GLRendererSlope.h"
#include "../opengl/GLRendererStripe.h"
#include "../parallel/ParallelRenderState.h"
#include "../preset/Presets.h"


class RFFRenderScene final : private RFFScene {
    ParallelRenderState state;
    Settings settings;
    int mouseX = 0;
    int mouseY = 0;

    uint64_t lastPeriod = 1;

    ApproxTableCache approxTableCache = ApproxTableCache();

    std::atomic<bool> recomputeRequested = false;
    std::atomic<bool> resizeRequested = false;
    std::atomic<bool> colorRequested = false;
    std::atomic<bool> createImageRequested = false;

    std::array<std::string, RFF::Status::LENGTH> *statusMessageRef = nullptr;
    std::unique_ptr<RFFMap> currentMap = nullptr;
    std::unique_ptr<Matrix<double> > iterationMatrix = nullptr;
    std::unique_ptr<MandelbrotPerturbator> currentPerturbator = nullptr;

    std::unique_ptr<GLMultipassRenderer> renderer = nullptr;
    std::unique_ptr<GLRendererIteration> rendererIteration = nullptr;
    std::unique_ptr<GLRendererStripe> rendererStripe = nullptr;
    std::unique_ptr<GLRendererSlope> rendererSlope = nullptr;
    std::unique_ptr<GLRendererColor> rendererColorFilter = nullptr;
    std::unique_ptr<GLRendererFog> rendererFog = nullptr;
    std::unique_ptr<GLRendererBloom> rendererBloom = nullptr;
    std::unique_ptr<GLRendererAntialiasing> rendererAntialiasing = nullptr;

public:

    int cwRequest = 0;
    int chRequest = 0;

    RFFRenderScene();

    ~RFFRenderScene() override;

    RFFRenderScene(const RFFRenderScene &) = delete;

    RFFRenderScene &operator=(const RFFRenderScene &) = delete;

    RFFRenderScene(RFFRenderScene &&) noexcept = delete;

    RFFRenderScene &operator=(RFFRenderScene &&) = delete;

private:
    static Settings initSettings();

public:
    void runAction(UINT message, WPARAM wParam);

    std::array<double_exp, 2> offsetConversion(const Settings &settings, int mx, int my) const;

    static double_exp getDivisor(const Settings &settings);

    int getClientWidth() const;

    int getClientHeight() const;

    int getIterationBufferWidth(const Settings &settings) const;

    int getIterationBufferHeight(const Settings &settings) const;

    void configure(HWND wnd, HDC hdc, HGLRC context,
                   std::array<std::string, RFF::Status::LENGTH> *statusMessageRef);

    void renderGL() override;

    void requestColor();

    void requestResize();

    void requestRecompute();

    void requestCreateImage();

    void applyCreateImage() const;

    void applyColor(const Settings &settings) const;

    void applyResize();

    int getMouseXOnIterationBuffer() const;

    int getMouseYOnIterationBuffer() const;

    void recompute();

    void beforeCompute(Settings &settings) const;

    bool compute(const Settings &settings);

    void afterCompute(bool success);

    void setStatusMessage(int index, const std::string_view &message) const;

    Settings &getSettings();

    ParallelRenderState &getState();

    MandelbrotPerturbator *getCurrentPerturbator() const;

    void setCurrentPerturbator(std::unique_ptr<MandelbrotPerturbator> perturbator);

    ApproxTableCache &getApproxTableCache();

    template<typename P> requires std::is_base_of_v<Preset, P>
    void changePreset(P &preset);
};


template<typename P> requires std::is_base_of_v<Preset, P>
void RFFRenderScene::changePreset(P &preset) {
    if constexpr (std::is_base_of_v<Presets::CalculationPresets, P>) {
        settings.calculationSettings.mpaSettings = preset.mpaSettings();
        settings.calculationSettings.referenceCompressionSettings = preset.referenceCompressionSettings();
        requestRecompute();
    }
    if constexpr (std::is_base_of_v<Presets::RenderPresets, P>) {
        settings.renderSettings = preset.renderSettings();
        requestResize();
        requestRecompute();
    }
    if constexpr (std::is_base_of_v<Presets::ResolutionPresets, P>) {
        auto r = preset.getResolution();
        cwRequest = r[0];
        chRequest = r[1];
    }
    if constexpr (std::is_base_of_v<Presets::ShaderPreset, P>) {
        if constexpr (std::is_base_of_v<Presets::ShaderPresets::PalettePreset, P>) {
            settings.shaderSettings.paletteSettings = preset.paletteSettings();
        }
        if constexpr (std::is_base_of_v<Presets::ShaderPresets::StripePreset, P>) {
            settings.shaderSettings.stripeSettings = preset.stripeSettings();
        }
        if constexpr (std::is_base_of_v<Presets::ShaderPresets::SlopePreset, P>) {
            settings.shaderSettings.slopeSettings = preset.slopeSettings();
        }
        if constexpr (std::is_base_of_v<Presets::ShaderPresets::ColorPreset, P>) {
            settings.shaderSettings.colorSettings = preset.colorSettings();
        }
        if constexpr (std::is_base_of_v<Presets::ShaderPresets::FogPreset, P>) {
            settings.shaderSettings.fogSettings = preset.fogSettings();
        }
        if constexpr (std::is_base_of_v<Presets::ShaderPresets::BloomPreset, P>) {
            settings.shaderSettings.bloomSettings = preset.bloomSettings();
        }
        requestColor();
    }
}
