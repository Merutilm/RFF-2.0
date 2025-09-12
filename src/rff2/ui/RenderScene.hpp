//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include <vector>
#include <windows.h>
#include <atomic>

#include "RenderSceneRequests.hpp"
#include "RenderSceneRenderer.hpp"
#include "../../vulkan_helper/handle/EngineHandler.hpp"
#include "../data/ApproxTableCache.h"
#include "../formula/MandelbrotPerturbator.h"
#include "../io/RFFDynamicMapBinary.h"
#include "../parallel/BackgroundThreads.h"
#include "../preset/Presets.h"
#include "../attr/Attribute.h"

namespace merutilm::rff2 {
    class RenderScene final : public vkh::EngineHandler {

        vkh::WindowContextRef wc;
        ParallelRenderState state;
        Attribute attr;

        uint16_t interactedMX = 0;
        uint16_t interactedMY = 0;

        uint64_t lastMaxIteration = 0;
        float lastLogZoom = 0;
        uint64_t lastPeriod = 1;

        RenderSceneRequests requests;

        std::atomic<bool> idleCompute = true;

        ApproxTableCache approxTableCache = ApproxTableCache();

        std::array<std::wstring, Constants::Status::LENGTH> *statusMessageRef = nullptr;
        std::unique_ptr<Matrix<double>> iterationMatrix = nullptr;

        std::unique_ptr<MandelbrotPerturbator> currentPerturbator = nullptr;

        std::unique_ptr<RenderSceneRenderer> renderer = nullptr;

        bool wndFPSRequest = false;
        uint16_t wndCWRequest = 0;
        uint16_t wndCHRequest = 0;

        BackgroundThreads backgroundThreads = BackgroundThreads();

    public:
        explicit RenderScene(vkh::EngineRef engine, vkh::WindowContextRef wc,
                             std::array<std::wstring, Constants::Status::LENGTH> *statusMessageRef);

        ~RenderScene() override;

        RenderScene(const RenderScene &) = delete;

        RenderScene &operator=(const RenderScene &) = delete;

        RenderScene(RenderScene &&) = delete;

        RenderScene &operator=(RenderScene &&) = delete;



        void resolveWindowResizeEnd() const;

        void render();

        [[nodiscard]] VkExtent2D getInternalImageExtent() const {
            const auto &swapchain = wc.getSwapchain();
            const auto [width, height] = swapchain.populateSwapchainExtent();
            const float multiplier = attr.render.clarityMultiplier;
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


        static Attribute genDefaultAttr();

        static LRESULT CALLBACK renderSceneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        [[nodiscard]] std::array<dex, 2> offsetConversion(const Attribute &settings, int mx, int my) const;

        static dex getDivisor(const Attribute &settings);

        [[nodiscard]] uint16_t getClientWidth() const;

        [[nodiscard]] uint16_t getClientHeight() const;

        [[nodiscard]] uint16_t getIterationBufferWidth(const Attribute &settings) const;

        [[nodiscard]] uint16_t getIterationBufferHeight(const Attribute &settings) const;

        void applyDefaultAttr();

        void applyCreateImage();

        void applyShaderAttr(const Attribute &attr) const;

        void refreshResizeParams();

        void initRenderer();

        void refreshRenderContext() const;

        void applyResize();

        void refreshSharedImgContext() const;

        void overwriteMatrixFromMap(const RFFDynamicMapBinary &map) const;

        [[nodiscard]] uint16_t getMouseXOnIterationBuffer() const;

        [[nodiscard]] uint16_t getMouseYOnIterationBuffer() const;

        void recomputeThreaded();

        void beforeCompute(Attribute &attr) const;

        bool compute(const Attribute &attr);

        void afterCompute(bool success);

        void setStatusMessage(const int index, const std::wstring_view &message) const {
            (*statusMessageRef)[index] = std::wstring(L"  ").append(message);
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

        [[nodiscard]] RFFDynamicMapBinary generateMap() const {
            return RFFDynamicMapBinary(lastLogZoom, lastPeriod, lastMaxIteration, *iterationMatrix);
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

        [[nodiscard]] vkh::WindowContextRef getWindowContext() const {
            return wc;
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
        void changePreset(P &preset);

    private:
        void runAction(UINT msg, WPARAM wparam, LPARAM lparam);

        void init() override;

        void attachRenderContext() const;

        void destroy() override;
    };


    template<typename P> requires std::is_base_of_v<Preset, P>
    void RenderScene::changePreset(P &preset) {
        if constexpr (std::is_base_of_v<Presets::CalculationPreset, P>) {
            attr.fractal.mpaAttribute = preset.genMPA();
            attr.fractal.referenceCompAttribute = preset.genReferenceCompression();
            requests.requestRecompute();
        }
        if constexpr (std::is_base_of_v<Presets::RenderPreset, P>) {
            attr.render = preset.genRender();
            requests.requestResize();
            requests.requestRecompute();
        }
        if constexpr (std::is_base_of_v<Presets::ResolutionPreset, P>) {
            auto r = preset.genResolution();
            wndRequestClientSize(r[0], r[1]);
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
            requests.requestShader();
        }
    }
}
