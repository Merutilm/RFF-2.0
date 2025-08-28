//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include <vector>
#include <windows.h>
#include <atomic>

#include "../../vulkan_helper/configurator/GraphicsPipelineConfigurator.hpp"
#include "../../vulkan_helper/handle/EngineHandler.hpp"
#include "../data/ApproxTableCache.h"
#include "../formula/MandelbrotPerturbator.h"
#include "../io/RFFDynamicMapBinary.h"
#include "../parallel/BackgroundThreads.h"
#include "../preset/Presets.h"
#include "../attr/Attribute.h"
#include "../vulkan/BoxBlurPipelineConfigurator.hpp"
#include "../vulkan/ColorPipelineConfigurator.hpp"
#include "../vulkan/IterationPalettePipelineConfigurator.hpp"
#include "../vulkan/SlopePipelineConfigurator.hpp"
#include "../vulkan/StripePipelineConfigurator.hpp"
#include "../vulkan/PresentPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class RenderScene final : public vkh::EngineHandler {
        HWND window;

        ParallelRenderState state;
        Attribute attr;

        uint16_t interactedMX = 0;
        uint16_t interactedMY = 0;

        uint64_t lastPeriod = 1;

        static constexpr uint32_t BLUR_MAX_WIDTH = 300;


        std::atomic<bool> recomputeRequested = false;
        std::atomic<bool> resizeRequested = false;
        std::atomic<bool> shaderRequested = false;
        std::atomic<bool> createImageRequested = false;
        std::string createImageRequestedFilename;

        std::atomic<bool> idleCompute = true;

        ApproxTableCache approxTableCache = ApproxTableCache();

        std::array<std::string, Constants::Status::LENGTH> *statusMessageRef = nullptr;
        std::unique_ptr<RFFDynamicMapBinary> currentMap = nullptr;
        std::unique_ptr<Matrix<double> > iterationMatrix = nullptr;
        std::unique_ptr<MandelbrotPerturbator> currentPerturbator = nullptr;


        std::vector<std::unique_ptr<vkh::PipelineConfigurator> > shaderPrograms = {};

        IterationPalettePipelineConfigurator *rendererIteration;
        StripePipelineConfigurator *rendererStripe;
        SlopePipelineConfigurator *rendererSlope;
        ColorPipelineConfigurator *rendererColor;
        BoxBlurPipelineConfigurator *rendererBoxBlur;
        PresentPipelineConfigurator *rendererPresent;


        uint16_t cwRequest = 0;
        uint16_t chRequest = 0;

        BackgroundThreads backgroundThreads = BackgroundThreads();

    public:
        explicit RenderScene(vkh::EngineRef engine, HWND window,
                             std::array<std::string, Constants::Status::LENGTH> *statusMessageRef);

        ~RenderScene() override;

        RenderScene(const RenderScene &) = delete;

        RenderScene &operator=(const RenderScene &) = delete;

        RenderScene(RenderScene &&) = delete;

        RenderScene &operator=(RenderScene &&) = delete;

        [[nodiscard]] HWND getWindowHandle() const { return window; }

        void resolveWindowResizeEnd();

        void render(uint32_t frameIndex, uint32_t imageIndex);

        void draw(uint32_t frameIndex, uint32_t imageIndex);


        void requestShader() {
            shaderRequested = true;
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

        [[nodiscard]] VkExtent2D getInternalRenderContextExtent() const {
            const auto &swapchain = *engine.getCore().getWindowContext(
                Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).swapchain;
            const auto [width, height] = swapchain.populateSwapchainExtent();
            const float multiplier = attr.render.clarityMultiplier;
            return {
                static_cast<uint32_t>(static_cast<float>(width) * multiplier), static_cast<uint32_t>(static_cast<float>(height) * multiplier)
            };
        }

        [[nodiscard]] VkExtent2D getExternalRenderContextExtent() const {
            const auto &swapchain = *engine.getCore().getWindowContext(
                Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).swapchain;
            return swapchain.populateSwapchainExtent();
        }


        static Attribute defaultSettings();

        static LRESULT CALLBACK renderSceneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        [[nodiscard]] std::array<dex, 2> offsetConversion(const Attribute &settings, int mx, int my) const;

        static dex getDivisor(const Attribute &settings);

        [[nodiscard]] uint16_t getClientWidth() const;

        [[nodiscard]] uint16_t getClientHeight() const;

        [[nodiscard]] uint16_t getIterationBufferWidth(const Attribute &settings) const;

        [[nodiscard]] uint16_t getIterationBufferHeight(const Attribute &settings) const;

        void applyCreateImage();

        void applyShaderAttr(const Attribute &attr) const;

        void applyResize();

        void overwriteMatrixFromMap() const;

        [[nodiscard]] uint16_t getMouseXOnIterationBuffer() const;

        [[nodiscard]] uint16_t getMouseYOnIterationBuffer() const;

        void recomputeThreaded();

        void beforeCompute(Attribute &settings) const;

        bool compute(const Attribute &settings);

        void afterCompute(bool success);

        void setStatusMessage(const int index, const std::string_view &message) const {
            (*statusMessageRef)[index] = std::string("  ").append(message);
        }

        [[nodiscard]] Attribute &getAttribute() {
            return attr;
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
            return shaderRequested;
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

        void init() override;

        void initRenderContext() const;

        void initShaderPrograms();

        void destroy() override;
    };


    template<typename P> requires std::is_base_of_v<Preset, P>
    void RenderScene::changePreset(P &preset) {
        if constexpr (std::is_base_of_v<Presets::CalculationPresets, P>) {
            attr.calc.mpaAttribute = preset.genMPA();
            attr.calc.referenceCompAttribute = preset.genReferenceCompression();
            requestRecompute();
        }
        if constexpr (std::is_base_of_v<Presets::RenderPresets, P>) {
            attr.render = preset.genRender();
            requestResize();
            requestRecompute();
        }
        if constexpr (std::is_base_of_v<Presets::ResolutionPresets, P>) {
            auto r = preset.genResolution();
            cwRequest = r[0];
            chRequest = r[1];
        }
        if constexpr (std::is_base_of_v<Presets::ShaderPreset, P>) {
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::PalettePreset, P>) {
                attr.shader.palette = preset.genPalette();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::StripePreset, P>) {
                attr.shader.stripe = preset.genStripe();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::SlopePreset, P>) {
                attr.shader.slope = preset.genSlope();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::ColorPreset, P>) {
                attr.shader.color = preset.genColor();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::FogPreset, P>) {
                attr.shader.fog = preset.genFog();
            }
            if constexpr (std::is_base_of_v<Presets::ShaderPresets::BloomPreset, P>) {
                attr.shader.bloom = preset.genBloom();
            }
            requestShader();
        }
    }
}
