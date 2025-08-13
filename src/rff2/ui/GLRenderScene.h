//
// Created by Merutilm on 2025-05-07.
//

#pragma once

#include "../settings/Settings.h"
#include <windows.h>

#include "WGLScene.h"
#include "../data/ApproxTableCache.h"

#include "../data/Matrix.h"
#include "../formula/LightMandelbrotPerturbator.h"
#include "../io/RFFDynamicMapBinary.h"
#include "../mrthy/DeepPA.h"
#include "../opengl/GLMultipassRenderer.h"
#include "../opengl/GLRendererAntialiasing.h"
#include "../opengl/GLRendererBloom.h"
#include "../opengl/GLRendererColor.h"
#include "../opengl/GLRendererFog.h"
#include "../opengl/GLRendererIteration.h"
#include "../opengl/GLRendererSlope.h"
#include "../opengl/GLRendererStripe.h"
#include "../parallel/BackgroundThreads.h"
#include "../parallel/ParallelRenderState.h"
#include "../preset/Presets.h"


namespace merutilm::rff2 {
    class GLRenderScene final : public WGLScene {

        ParallelRenderState state;
        Settings settings;

        uint16_t interactedMX = 0;
        uint16_t interactedMY = 0;

        uint64_t lastPeriod = 1;


        std::atomic<bool> recomputeRequested = false;
        std::atomic<bool> resizeRequested = false;
        std::atomic<bool> colorRequested = false;
        std::atomic<bool> createImageRequested = false;
        std::string createImageRequestedFilename = "";

        std::atomic<bool> idleCompute = true;

        ApproxTableCache approxTableCache = ApproxTableCache();

        std::array<std::string, Constants::Status::LENGTH> *statusMessageRef = nullptr;
        std::unique_ptr<RFFDynamicMapBinary> currentMap = nullptr;
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

        uint16_t cwRequest = 0;
        uint16_t chRequest = 0;

        BackgroundThreads backgroundThreads = BackgroundThreads();

    public:
        explicit GLRenderScene();

        ~GLRenderScene() override;

        GLRenderScene(const GLRenderScene &) = delete;

        GLRenderScene &operator=(const GLRenderScene &) = delete;

        GLRenderScene(GLRenderScene &&) noexcept = delete;

        GLRenderScene &operator=(GLRenderScene &&) = delete;

        static Settings defaultSettings();

        void runAction(UINT message, WPARAM wParam);

        [[nodiscard]] std::array<dex, 2> offsetConversion(const Settings &settings, int mx, int my) const;

        static dex getDivisor(const Settings &settings);

        [[nodiscard]] uint16_t getClientWidth() const;

        [[nodiscard]] uint16_t getClientHeight() const;

        [[nodiscard]] uint16_t getIterationBufferWidth(const Settings &settings) const;

        [[nodiscard]] uint16_t getIterationBufferHeight(const Settings &settings) const;

        void configure(HWND wnd, HDC hdc, HGLRC context,
                       std::array<std::string, Constants::Status::LENGTH> *statusMessageRef);

        void renderGL() override;

        void requestColor();

        void requestResize();

        void requestRecompute();

        void requestCreateImage(std::string_view = "");

        void applyCreateImage();

        void applyColor(const Settings &settings) const;

        void applyResize();

        void overwriteMatrixFromMap() const;

        uint16_t getMouseXOnIterationBuffer() const;

        uint16_t getMouseYOnIterationBuffer() const;

        void recomputeThreaded();

        void beforeCompute(Settings &settings) const;

        bool compute(const Settings &settings);

        void afterCompute(bool success);

        void setStatusMessage(int index, const std::string_view &message) const;

        Settings &getSettings();

        ParallelRenderState &getState();

        MandelbrotPerturbator *getCurrentPerturbator() const;

        void setCurrentPerturbator(std::unique_ptr<MandelbrotPerturbator> perturbator);

        ApproxTableCache &getApproxTableCache();

        BackgroundThreads &getBackgroundThreads();

        RFFDynamicMapBinary &getCurrentMap() const;

        void setCurrentMap(const RFFDynamicMapBinary &map);

        bool isRecomputeRequested() const;

        bool isResizeRequested() const;

        bool isColorRequested() const;

        bool isCreateImageRequested() const;

        bool isIdleCompute() const;

        int getCWRequest() const;

        int getCHRequest() const;

        void clientResizeRequestSolved();

        template<typename P> requires std::is_base_of_v<Preset, P>
        void changePreset(P &preset);
    };


    template<typename P> requires std::is_base_of_v<Preset, P>
    void GLRenderScene::changePreset(P &preset) {
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
}