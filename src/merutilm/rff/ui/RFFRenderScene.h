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


class RFFRenderScene final : private RFFScene {
    ParallelRenderState referenceRenderState;
    Settings settings;
    int mouseX = 0;
    int mouseY = 0;
    uint64_t lastPeriod = 1;

    ApproxTableCache approxTableCache = ApproxTableCache();

    std::atomic<bool> recomputeRequested = false;
    std::atomic<bool> resizeRequested = false;
    std::atomic<bool> colorRequested = false;
    std::atomic<bool> clarityRequested = false;
    std::atomic<bool> isComputing = false;

    std::atomic<bool> canDisplayed = false;

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

    void requestClarity();

    void requestRecompute();

    void applyColor(const Settings &settings) const;

    void applyResize() const;

    void applyClarity() const;

    void applyComputationalSettings();

    int getMouseXOnIterationBuffer() const;

    int getMouseYOnIterationBuffer() const;

    void recompute();

    void beforeCompute(const Settings &settings);

    void compute(const Settings &settings);

    void afterCompute() const;

    void setStatusMessage(int index, const std::string_view &message) const;

    Settings &getSettings();

    ParallelRenderState &getState();

    MandelbrotPerturbator *getCurrentPerturbator() const;

    void setCurrentPerturbator(std::unique_ptr<MandelbrotPerturbator> perturbator);

    ApproxTableCache &getApproxTableCache();
};
