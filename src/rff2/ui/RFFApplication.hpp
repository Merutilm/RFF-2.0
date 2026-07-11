//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include <atomic>

#include "../formula/MB2Perturbator.h"
#include "../formula/MB2RenderData.hpp"
#include "../io/RFFDynamicMapBinary.h"
#include "../parallel/BackgroundThreads.h"
#include "../preset/Presets.h"
#include "../settings/Settings.h"
#include "AppRenderManagerRequests.hpp"
#include "AppRenderer.hpp"
#include "CursorManager.hpp"
#include "SettingsWindow.hpp"
#include "vulkan_helper/Application.hpp"

namespace merutilm::rff2 {
    class RFFApplication final : public vkh::Application {

        ParallelRenderState state = {};
        Settings settings;
        AppRenderManagerRequests requests = {};
        AppRenderer *renderer = nullptr;

        std::atomic<bool> idleCompute = true;

        std::array<std::string, Constants::Status::LENGTH> statusMessages = {};
        std::unique_ptr<Matrix<double>> iterationMatrix = nullptr;

        std::unique_ptr<MB2RenderDataBase> renderData = nullptr;

        std::unique_ptr<CursorManager> cursorManager = nullptr;

        BackgroundThreads backgroundThreads = BackgroundThreads();

    public:
        explicit RFFApplication(const vkh::WindowInitializerSettings &wic) : Application(wic), settings(genDefaultAttr()) {

        }

        ~RFFApplication() override = default;

        RFFApplication(const RFFApplication &) = delete;

        RFFApplication &operator=(const RFFApplication &) = delete;

        RFFApplication(RFFApplication &&) = delete;

        RFFApplication &operator=(RFFApplication &&) = delete;

        void resolveWindowResize(uint32_t w, uint32_t h);

        void update() override;


        static Settings genDefaultAttr();

        [[nodiscard]] complex<dex> offsetConversion(const Settings &s, int mx, int my) const;

        static dex getDivisor(const Settings &settings);

        [[nodiscard]] uint16_t calcIterationBufferWidth(const Settings &s) const;

        [[nodiscard]] uint16_t calcIterationBufferHeight(const Settings &s) const;
        uint16_t getIterationBufferWidth() const;
        uint16_t getIterationBufferHeight() const;


        void addListeners() override;

        void applyDefaultSettings();

        void applyCreateImage();

        void applyShaderSettings(const Settings &s) const;

        void refreshResizeParams(VkExtent2D swapchainExtent);

        void registerRenderers() override;


        void refreshSharedImgContexts(VkExtent2D extent) override;

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


        void setStatusMessage(const int index, const std::string_view &message) {
            statusMessages[index] = std::string("  ").append(message);
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

        // void setCurrentSettingsWindows(std::unique_ptr<SettingsWindow> window) {
        //     currentSettingsWindow = std::move(window);
        // }

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
            return *rootWindowContext;
        }


        template<typename P> requires std::is_base_of_v<Preset, P>
        void applyPreset(P &preset);

    protected:

        void onStart() override;

        void onQuit() override;

        void renderImGui() override;

    };


    template<typename P> requires std::is_base_of_v<Preset, P>
    void RFFApplication::applyPreset(P &preset) {
        if constexpr (std::is_base_of_v<Presets::CalculationPreset, P>) {
            settings.fractal.reference.sync = preset.genRefSync();
            settings.fractal.mpa = preset.genMPA();
            settings.fractal.reference.compression = preset.genRefComp();
            requests.requestRecompute();
        }
        if constexpr (std::is_base_of_v<Presets::RenderPreset, P>) {
            settings.render = preset.genRender();
            int w;
            int h;
            glfwGetWindowSize(rootWindowContext->getWindow()->getWindow(), &w, &h);
            resolveWindowResize(static_cast<uint32_t>(w), static_cast<uint32_t>(h));
            requests.requestRecompute();
        }
        if constexpr (std::is_base_of_v<Presets::ResolutionPreset, P>) {
            auto r = preset.genResolution();
            glfwSetWindowSize(rootWindowContext->getWindow()->getWindow(), r[0], r[1]);
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
