//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include <atomic>
#include <vector>

#include "../formula/MB2Perturbator.h"
#include "../formula/MB2RenderData.hpp"
#include "../io/RFFDynamicMapBinary.h"
#include "../parallel/BackgroundThreads.h"
#include "../preset/Presets.h"
#include "../settings/Settings.h"
#include "AppRenderManagerRequests.hpp"
#include "AppRenderer.hpp"
#include "SettingsWindow.hpp"
#include "vulkan_helper/engine/SharedResource.hpp"
#include "vulkan_helper/handle/EngineHandler.hpp"

namespace merutilm::rff2 {
    class AppRenderManager final : public vkh::EngineHandler {

        vkh::WindowContext &wc;
        vkh::SharedResource &sr;

        ParallelRenderState state;
        Settings settings;
        AppRenderManagerRequests requests;

        std::atomic<bool> idleCompute = true;

        std::unique_ptr<SettingsWindow> currentSettingsWindow;

        std::array<std::wstring, Constants::Status::LENGTH> *statusMessageRef = nullptr;
        std::unique_ptr<Matrix<double>> iterationMatrix = nullptr;

        std::unique_ptr<MB2RenderDataBase> renderData = nullptr;

        std::unique_ptr<AppRenderer> renderer = nullptr;

        BackgroundThreads backgroundThreads = BackgroundThreads();

    public:
        explicit AppRenderManager(vkh::Engine &engine, vkh::WindowContext &wc, vkh::SharedResource &sr,
                             std::array<std::wstring, Constants::Status::LENGTH> *statusMessageRef);

        ~AppRenderManager() override;

        AppRenderManager(const AppRenderManager &) = delete;

        AppRenderManager &operator=(const AppRenderManager &) = delete;

        AppRenderManager(AppRenderManager &&) = delete;

        AppRenderManager &operator=(AppRenderManager &&) = delete;

        void resolveWindowResizeEnd();

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

        void addMouseListeners();

        [[nodiscard]] std::array<dex, 2> offsetConversion(const Settings &s, int mx, int my) const;

        static dex getDivisor(const Settings &settings);

        [[nodiscard]] uint16_t getIterationBufferWidth(const Settings &s) const;

        [[nodiscard]] uint16_t getIterationBufferHeight(const Settings &s) const;

        void applyDefaultSettings();

        void applyCreateImage();

        void applyShaderAttr(const Settings &s) const;

        void applyResize();

        void refreshResizeParams();

        void initRenderer();

        void refreshRenderContext() const;

        void refreshSharedImgContext() const;

        void overwriteMatrixFromMap(const RFFDynamicMapBinary &map) const;

        [[nodiscard]] int16_t getMouseXOnIterationBuffer(int mx) const;

        [[nodiscard]] int16_t getMouseYOnIterationBuffer(int my) const;

        void recomputeThreaded();

        void beforeIterationFill() const;

        bool prepareRenderData(const std::chrono::time_point<std::chrono::high_resolution_clock> &start,
                              const Settings &s);
        bool fillIteration(const std::chrono::time_point<std::chrono::high_resolution_clock> &start,
                                 const Settings &s);

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

        [[nodiscard]] AppRenderManagerRequests &getRequests() {
            return requests;
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

        [[nodiscard]] bool isIdleCompute() const {
            return idleCompute;
        }


        [[nodiscard]] vkh::WindowContext &getWindowContext() const {
            return wc;
        }

        [[nodiscard]] vkh::SharedResource &getSharedResource() const {
            return sr;
        }


        template<typename P> requires std::is_base_of_v<Preset, P>
        void applyPreset(P &preset);

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
            wc.getWindow()->setRenderWindowSizeWithClientAdjustment(r[0], r[1]);
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
