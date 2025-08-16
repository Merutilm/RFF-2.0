//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include <vector>
#include <windows.h>
#include <atomic>

#include "../../vulkan_helper/configurator/PipelineConfigurator.hpp"
#include "../../vulkan_helper/handle/EngineHandler.hpp"
#include "../data/ApproxTableCache.h"
#include "../formula/MandelbrotPerturbator.h"
#include "../io/RFFDynamicMapBinary.h"
#include "../parallel/BackgroundThreads.h"
#include "../preset/Presets.h"
#include "../settings/Settings.h"
#include "../vulkan/IterationPalettePipelineConfigurator.hpp"
#include "../vulkan/StripePipelineConfigurator.hpp"

namespace merutilm::rff2 {

    class RenderScene final {

        vkh::Engine &engine;
        HWND window;

        ParallelRenderState state;
        Settings settings;

        uint16_t interactedMX = 0;
        uint16_t interactedMY = 0;

        uint64_t lastPeriod = 1;


        std::atomic<bool> recomputeRequested = false;
        std::atomic<bool> resizeRequested = false;
        std::atomic<bool> colorRequested = false;
        std::atomic<bool> createImageRequested = false;
        std::string createImageRequestedFilename;

        std::atomic<bool> idleCompute = true;

        ApproxTableCache approxTableCache = ApproxTableCache();

        std::array<std::string, Constants::Status::LENGTH> *statusMessageRef = nullptr;
        std::unique_ptr<RFFDynamicMapBinary> currentMap = nullptr;
        std::unique_ptr<Matrix<double> > iterationMatrix = nullptr;
        std::unique_ptr<MandelbrotPerturbator> currentPerturbator = nullptr;
        
        std::vector<std::unique_ptr<vkh::PipelineConfigurator>> shaderPrograms = {};
        IterationPalettePipelineConfigurator * rendererIteration;
        StripePipelineConfigurator * rendererStripe;



        uint16_t cwRequest = 0;
        uint16_t chRequest = 0;

        BackgroundThreads backgroundThreads = BackgroundThreads();

    public:
        explicit RenderScene(vkh::Engine& engine, HWND window, std::array<std::string, Constants::Status::LENGTH> *statusMessageRef);

        ~RenderScene();

        RenderScene(const RenderScene &) = delete;

        RenderScene &operator=(const RenderScene &) = delete;

        RenderScene(RenderScene &&) = delete;

        RenderScene &operator=(RenderScene &&) = delete;

        [[nodiscard]] HWND getWindowHandle() const { return window; }

        void render(VkCommandBuffer cbh, uint32_t frameIndex, const VkExtent2D &extent);


        void requestColor() {
            colorRequested = true;
        }

        void requestResize() {
            resizeRequested = true;
        }

        void requestRecompute() {
            recomputeRequested = true;
        }

        void requestCreateImage(const std::string_view filename = "") {
            createImageRequested = true;
            createImageRequestedFilename = filename;
        };


        static Settings defaultSettings();

        static LRESULT CALLBACK renderSceneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        [[nodiscard]] std::array<dex, 2> offsetConversion(const Settings &settings, int mx, int my) const;

        static dex getDivisor(const Settings &settings);

        [[nodiscard]] uint16_t getClientWidth() const;

        [[nodiscard]] uint16_t getClientHeight() const;

        [[nodiscard]] uint16_t getIterationBufferWidth(const Settings &settings) const;

        [[nodiscard]] uint16_t getIterationBufferHeight(const Settings &settings) const;

        void applyCreateImage();

        void applyColor(const Settings &settings) const;

        void applyResize();

        void overwriteMatrixFromMap() const;

        [[nodiscard]] uint16_t getMouseXOnIterationBuffer() const;

        [[nodiscard]] uint16_t getMouseYOnIterationBuffer() const;

        void recomputeThreaded();

        void beforeCompute(Settings &settings) const;

        bool compute(const Settings &settings);

        void afterCompute(bool success);

        void setStatusMessage(const int index, const std::string_view &message) const {
            (*statusMessageRef)[index] = std::string("  ").append(message);
        }

        [[nodiscard]] Settings &getSettings() {
            return settings;
        }

        [[nodiscard]] ParallelRenderState &getState() {
            return state;
        }

        [[nodiscard]] MandelbrotPerturbator *getCurrentPerturbator() const {
            return currentPerturbator.get();
        }

        void setCurrentPerturbator(std::unique_ptr<MandelbrotPerturbator> perturbator) {
            currentPerturbator = std::move(perturbator);
        }

        [[nodiscard]] ApproxTableCache &getApproxTableCache() {
            return approxTableCache;
        }

        [[nodiscard]] BackgroundThreads &getBackgroundThreads() {
            return backgroundThreads;
        }

        [[nodiscard]] RFFDynamicMapBinary &getCurrentMap() const {
            return *currentMap;
        }

        void setCurrentMap(const RFFDynamicMapBinary &map) {
            currentMap = std::make_unique<RFFDynamicMapBinary>(map);
        }

        [[nodiscard]] const std::vector<std::unique_ptr<vkh::PipelineConfigurator>> &getShaderPrograms() { return shaderPrograms; }
        
        [[nodiscard]] bool isRecomputeRequested() const {
            return recomputeRequested;
        }

        [[nodiscard]] bool isCreateImageRequested() const {
            return createImageRequested;
        }

        [[nodiscard]] bool isResizeRequested() const {
            return resizeRequested;
        }

        [[nodiscard]] bool isColorRequested() const {
            return colorRequested;
        }

        [[nodiscard]] bool isIdleCompute() const {
            return idleCompute;
        }

        [[nodiscard]] int getCWRequest() const {
            return cwRequest;
        }

        [[nodiscard]] int getCHRequest() const {
            return chRequest;
        }

        void clientResizeRequestSolved() {
            cwRequest = 0;
            chRequest = 0;
        }


        template<typename P> requires std::is_base_of_v<Preset, P>
        void changePreset(P &preset);
        
    private:

        void runAction(UINT msg, WPARAM wparam, LPARAM lparam);

        void init();

        void initRenderContext() const;

        void initShaderPrograms();

        void destroy();
    };


    template<typename P> requires std::is_base_of_v<Preset, P>
    void RenderScene::changePreset(P &preset) {
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
