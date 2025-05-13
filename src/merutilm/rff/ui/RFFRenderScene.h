//
// Created by Merutilm on 2025-05-07.
//

#pragma once

#include "../settings/Settings.h"
#include <windows.h>

#include "RFFScene.h"

#include "../data/Matrix.h"
#include "../formula/LightMandelbrotPerturbator.h"
#include "../opengl/GLRendererIteration.h"
#include "../parallel/ParallelRenderState.h"
#include "../value/PointDouble.h"


class RFFRenderScene final : private RFFScene{

    ParallelRenderState referenceRenderState;
    Settings settings;
    int mouseX = 0;
    int mouseY = 0;
    bool recomputeRequested = false;
    bool resizeRequested = false;
    bool colorRequested = false;
    uint64_t lastPeriod = 1;
    bool canBeDisplayed = false;
    bool isComputing = false;

    std::array<std::string, RFFConstants::Status::LENGTH> *statusMessageRef = nullptr;

    std::unique_ptr<Matrix<double>> iterationMatrix = nullptr;
    std::unique_ptr<LightMandelbrotPerturbator> currentPerturbator = nullptr;
    std::unique_ptr<GLRendererIteration> rendererIteration = nullptr;


public:
    RFFRenderScene();

    ~RFFRenderScene() override = default;

    RFFRenderScene(const RFFRenderScene&) = delete;
    RFFRenderScene& operator=(const RFFRenderScene&) = delete;
    RFFRenderScene(RFFRenderScene&&) noexcept = delete;
    RFFRenderScene& operator=(RFFRenderScene&&) = delete;

private:

    static Settings initSettings();

public:
    void runAction(UINT message, WPARAM wParam);

    PointDouble offsetConversion(const Settings &settings, int mx, int my) const;

    static double getDivisor(const Settings &settings);

    int getClientWidth() const;

    int getClientHeight() const;

    int getIterationBufferWidth(const Settings &settings) const;

    int getIterationBufferHeight(const Settings &settings) const;

    void configure(HWND wnd, HDC hdc, HGLRC context, std::array<std::string, RFFConstants::Status::LENGTH> *statusMessageRef);

    void renderGL() override;

    void requestColor();

    void requestResize();

    void requestRecompute();

    void applyColor(const Settings &settings) const;

    void applyResize() const;

    void applyComputationalSettings();

    int getMouseXOnIterationBuffer() const;

    int getMouseYOnIterationBuffer() const;

    void recompute();

    void compute();

    void compute0(const Settings &settings);

    void setStatusMessage(int index, const std::string_view &message) const;

    Settings &getSettings();

};
