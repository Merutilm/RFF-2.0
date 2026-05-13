//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include <vector>
#include <atomic>

#include "../formula/MB2Perturbator.h"
#include "../formula/MB2RenderData.hpp"
#include "../io/RFFDynamicMapBinary.h"
#include "../parallel/BackgroundThreads.h"
#include "../preset/Presets.h"
#include "../settings/Settings.h"
#include "AppRenderer.hpp"
#include "RenderSceneRequests.hpp"
#include "SettingsWindow.hpp"
#include "vulkan_helper/engine/SharedResource.hpp"
#include "vulkan_helper/handle/EngineHandler.hpp"

namespace merutilm::rff2 {
    class AppRenderManager final : public vkh::EngineHandler {

        vkh::WindowContext &wc;
        vkh::SharedResource &sr;
        ParallelRenderState state;
        Settings settings;

        uint16_t interactedMX = 0;
        uint16_t interactedMY = 0;

        RenderSceneRequests requests;

        std::atomic<bool> idleCompute = true;

        std::unique_ptr<SettingsWindow> currentSettingsWindow;

        std::array<std::wstring, Constants::Status::LENGTH> *statusMessageRef = nullptr;
        std::unique_ptr<Matrix<double>> iterationMatrix = nullptr;

        std::unique_ptr<MB2RenderDataBase> renderData = nullptr;

        std::unique_ptr<AppRenderer> renderer = nullptr;

        bool wndFPSRequest = false;
        uint16_t wndCWRequest = 0;
        uint16_t wndCHRequest = 0;

        BackgroundThreads backgroundThreads = BackgroundThreads();

    public:
        explicit AppRenderManager(vkh::Engine &engine, vkh::WindowContext &wc, vkh::SharedResource &sr,
                             std::array<std::wstring, Constants::Status::LENGTH> *statusMessageRef);

        ~AppRenderManager() override;

        AppRenderManager(const AppRenderManager &) = delete;

        AppRenderManager &operator=(const AppRenderManager &) = delete;

        AppRenderManager(AppRenderManager &&) = delete;

        AppRenderManager &operator=(AppRenderManager &&) = delete;



        void resolveWindowResizeEnd() const;

        void render();

        [[nodiscard]] VkExtent2D getInternalImageExtent() const {
            const auto &swapchain = wc.getSwapchain();
            const auto [width, height] = swapchain.populateSwapchainExtent();
            const float multiplier = settings.render.clarityMultiplier;
            return {
                static_cast<uint32_t>(static_cast<float>(width) * multiplier),
                static_cast<uint32_t>(static_cast<float>(height) * multiplier)
            };
        }

        [[nodiscard]] VkExtent2D getBlurredImageExtent() const {
            const VkExtent2D blurredExtent = getInternalImageExtent();
            if (const float rat = Constants::Fractal::GAUSSIAN_MAX_WIDTH / static_cast<float>(blurredExtent.width);
                rat < 1) {
                return {
                    Constants::Fractal::GAUSSIAN_MAX_WIDTH,
                    static_cast<uint32_t>(static_cast<float>(blurredExtent.height) * rat)
                };
            }
            return blurredExtent;
        }

        [[nodiscard]] VkExtent2D getSwapchainRenderContextExtent() const {
            const auto &swapchain = wc.getSwapchain();
            return swapchain.populateSwapchainExtent();
        }


        static Settings genDefaultAttr();

        void addListeners();

        [[nodiscard]] std::array<dex, 2> offsetConversion(const Settings &settings, int mx, int my) const;

        static dex getDivisor(const Settings &settings);

        [[nodiscard]] uint16_t getRenderWindowWidth() const;

        [[nodiscard]] uint16_t getRenderWindowHeight() const;

        [[nodiscard]] uint16_t getIterationBufferWidth(const Settings &settings) const;

        [[nodiscard]] uint16_t getIterationBufferHeight(const Settings &settings) const;

        void applyDefaultAttr();

        void applyCreateImage();

        void applyShaderAttr(const Settings &settings) const;

        void refreshResizeParams();

        void initRenderer();

        void refreshRenderContext() const;

        void applyResize();

        void refreshSharedImgContext() const;

        void overwriteMatrixFromMap(const RFFDynamicMapBinary &map) const;

        [[nodiscard]] uint16_t getMouseXOnIterationBuffer(int mx) const;

        [[nodiscard]] uint16_t getMouseYOnIterationBuffer(int my) const;

        void recomputeThreaded();

        void beforeIterationFill() const;

        bool prepareRenderData(const std::chrono::time_point<std::chrono::high_resolution_clock> &start,
                              const Settings &settings);
        bool fillIteration(const std::chrono::time_point<std::chrono::high_resolution_clock> &start,
                                 const Settings &settings);

        void afterCompute(bool success);

        void setStatusMessage(const int index, const std::wstring_view &message) const {
            (*statusMessageRef)[index] = std::wstring(L"  ").append(message);
        }

        [[nodiscard]] Settings &getSettings() {
            return settings;
        }

        [[nodiscard]] ParallelRenderState &getState() {
            return state;
        }

        [[nodiscard]] MB2RenderDataBase *getCurrentRenderData() const {
            return renderData.get();
        }

        void setCurrentSettingsWindows(std::unique_ptr<SettingsWindow> window) {
            currentSettingsWindow = std::move(window);
        }

        void setCurrentPerturbator(std::unique_ptr<MB2RenderDataBase> data) {
            renderData = std::move(data);
        }

        [[nodiscard]] BackgroundThreads &getBackgroundThreads() {
            return backgroundThreads;
        }

        [[nodiscard]] RFFDynamicMapBinary generateMap() const {
            return {renderData->fractalSettings.general.logZoom, renderData->getReference()->longestPeriod(), renderData->fractalSettings.perturb.maxIteration, *iterationMatrix};
        }

        [[nodiscard]] RenderSceneRequests &getRequests() {
            return requests;
        }

        [[nodiscard]] bool isFPSRequested() const {
            return wndFPSRequest;
        }

        [[nodiscard]] bool isIdleCompute() const {
            return idleCompute;
        }


        [[nodiscard]] int getWndCWRequest() const {
            return wndCWRequest;
        }

        [[nodiscard]] int getWndCHRequest() const {
            return wndCHRequest;
        }

        [[nodiscard]] vkh::WindowContext &getWindowContext() const {
            return wc;
        }

        [[nodiscard]] vkh::SharedResource &getSharedResource() const {
            return sr;
        }

        void wndRequestFPS() {
            wndFPSRequest = true;
        }

        void wndRequestClientSize(const uint16_t width, const uint16_t height) {
            wndCWRequest = width;
            wndCHRequest = height;
        }

        void wndClientSizeRequestSolved() {
            wndCWRequest = 0;
            wndCHRequest = 0;
        }

        void wndFPSRequestSolved() {
            wndFPSRequest = false;
        }



        template<typename P> requires std::is_base_of_v<Preset, P>
        void applyPreset(P &preset);

    private:

        void init() override;

        void attachRenderContext() const;

        void cleanup() override;
    };


    template<typename P> requires std::is_base_of_v<Preset, P>
    void AppRenderManager::applyPreset(P &preset) {
        if constexpr (std::is_base_of_v<Presets::CalculationPreset, P>) {
            settings.fractal.reference.sync = preset.genRefSync();
            settings.fractal.mpa = preset.genMPA();
            settings.fractal.reference.compression = preset.genRefComp();
            requests.requestRecompute();
        }
        if constexpr (std::is_base_of_v<Presets::RenderPreset, P>) {
            settings.render = preset.genRender();
            requests.requestResize();
            requests.requestRecompute();
        }
        if constexpr (std::is_base_of_v<Presets::ResolutionPreset, P>) {
            auto r = preset.genResolution();
            wndRequestClientSize(r[0], r[1]);
        }
        if constexpr (std::is_base_of_v<Presets::ShaderPreset, P>) {
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::PalettePreset, P>) {
                settings.shader.palette = preset.genPalette();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::StripePreset, P>) {
                settings.shader.stripe = preset.genStripe();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::SlopePreset, P>) {
                settings.shader.slope = preset.genSlope();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::ColorPreset, P>) {
                settings.shader.color = preset.genColor();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::FogPreset, P>) {
                settings.shader.fog = preset.genFog();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::BloomPreset, P>) {
                settings.shader.bloom = preset.genBloom();
            }
            requests.requestShader();
        }
    }
}
