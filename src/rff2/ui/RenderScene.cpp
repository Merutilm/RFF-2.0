//
// Created by Merutilm on 2025-08-08.
//

#include "RenderScene.hpp"

#include "CallbackExplore.hpp"
#include "IOUtilities.h"
#include "../../vulkan_helper/executor/RenderPassFullscreenRecorder.hpp"
#include "../vulkan/RCC1.hpp"
#include "../vulkan/GPCIterationPalette.hpp"
#include "../calc/dex_exp.h"
#include "../formula/DeepMandelbrotPerturbator.h"
#include "../formula/LightMandelbrotPerturbator.h"
#include "../locator/MandelbrotLocator.h"
#include "../parallel/ParallelArrayDispatcher.h"
#include "../parallel/ParallelDispatcher.h"
#include "../preset/calc/CalculationPresets.h"
#include "../preset/render/RenderPresets.h"
#include "../preset/shader/bloom/ShdBloomPresets.h"
#include "../preset/shader/color/ShdColorPresets.h"
#include "../preset/shader/fog/ShdFogPresets.h"
#include "../preset/shader/palette/ShdPalettePresets.h"
#include "../preset/shader/slope/ShdSlopePresets.h"
#include "../preset/shader/stripe/ShdStripePresets.h"
#include "../vulkan/RCC4.hpp"
#include "../vulkan/RCCDownsampleForBlur.hpp"
#include "../vulkan/RCC3.hpp"
#include "../vulkan/RCC5.hpp"
#include "../vulkan/RCCPresent.hpp"
#include "../vulkan/RCCStatic2Image.hpp"
#include "../vulkan/SharedDescriptorTemplate.hpp"
#include "../vulkan/SharedImageContextIndices.hpp"
#include "opencv2/opencv.hpp"


namespace merutilm::rff2 {
    RenderScene::RenderScene(vkh::EngineRef engine, vkh::WindowContextRef wc,
                             std::array<std::wstring, Constants::Status::LENGTH> *
                             statusMessageRef) : EngineHandler(
                                                     engine),
                                                 wc(wc), attr(genDefaultAttr()),
                                                 statusMessageRef(statusMessageRef) {
        RenderScene::init();
    }

    RenderScene::~RenderScene() {
        RenderScene::destroy();
    }

    void RenderScene::init() {
        refreshSharedImgContext();
        attachRenderContext();
        initRenderer();
        refreshRenderContext();
        refreshResizeParams();
        applyShaderAttr(attr);
        wndRequestFPS();
        requests.requestRecompute();
    }


    void RenderScene::attachRenderContext() const {
        const auto swapchainImageContextGetter = [this] {
            auto &swapchain = wc.getSwapchain();
            return vkh::ImageContext::fromSwapchain(wc.core, swapchain);
        };
        wc.attachRenderContext<RCC0>(wc.core,
                                     [this] { return getInternalImageExtent(); },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCC1>(wc.core,
                                     [this] { return getInternalImageExtent(); },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCC2>(wc.core,
                                     [this] { return getInternalImageExtent(); },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCCDownsampleForBlur>(wc.core,
                                                     [this] { return getBlurredImageExtent(); },
                                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCC3>(wc.core,
                                     [this] { return getInternalImageExtent(); },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCC4>(wc.core,
                                     [this] { return getInternalImageExtent(); },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCC5>(wc.core,
                                     [this] { return getInternalImageExtent(); },
                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCCPresent>(wc.core,
                                           [this] { return getSwapchainRenderContextExtent(); },
                                           swapchainImageContextGetter);
    }

    void RenderScene::resolveWindowResizeEnd() const {
        if (wc.getWindow().isUnrenderable()) {
            return;
        }
        wc.core.getLogicalDevice().waitDeviceIdle();

        vkh::SwapchainRef swapchain = wc.getSwapchain();
        swapchain.recreate();
    }


    void RenderScene::render() {
        if (requests.defaultAttrRequested) {
            applyDefaultAttr();
            requests.defaultAttrRequested.exchange(false);
            backgroundThreads.notifyAll();
        }
        if (requests.shaderRequested) {
            applyShaderAttr(attr);
            requests.shaderRequested.exchange(false);
            backgroundThreads.notifyAll();
        }

        if (requests.resizeRequested) {
            state.cancel();
            applyResize();
            requests.resizeRequested.exchange(false);
            backgroundThreads.notifyAll();
        }

        if (requests.recomputeRequested) {
            idleCompute = false;
            requests.recomputeRequested.exchange(false);
            recomputeThreaded();
            //it is threaded, not idle
        }

        if (requests.createImageRequested) {
            applyCreateImage();
            requests.createImageRequested.exchange(false);
            backgroundThreads.notifyAll();
        }


        renderer->execute();
    }


    Attribute RenderScene::genDefaultAttr() {
        return Attribute{
            .fractal = FractalAttribute{
                .center = fp_complex("-0.85",
                                     "0",
                                     //"-1.29255707077531686131098415679305324693162987219277534742408945445699102528813182208390942132824552642640105852802031375797639923173781472397893283277669022615909880587638643429120957543820179919830492623879949932",
                                     //"-1.7433380976879299408417853435676017785972000052524291128107561584529660103218876836645852866195456038569337053542405",
                                     // "0.438169590583770312890168860021043433478705507119371935117854030759551072299659171256225012539071884716681573917133522314360175105572598172732723792994562397110248396170036793222839041625954944698185617470725880129",
                                     //"-0.00000180836819716880795128873613161993554089471597685393367018109950768833467685704762711890797154859214327088989719746641",
                                     Perturbator::logZoomToExp10(2)),
                .logZoom = 2, //186.47, //85.190033f,
                .maxIteration = 300,
                .bailout = 2,
                .decimalizeIterationMethod = FrtDecimalizeIterationMethod::LOG_LOG,
                .mpaAttribute = CalculationPresets::UltraFast().genMPA(),
                .referenceCompAttribute = CalculationPresets::UltraFast().genReferenceCompression(),
                .reuseReferenceMethod = FrtReuseReferenceMethod::DISABLED,
                .autoMaxIteration = true,
                .autoIterationMultiplier = 100,
                .absoluteIterationMode = false
            },
            .render = RenderPresets::High().genRender(),
            .shader = {
                .palette = ShdPalettePresets::LongRandom64().genPalette(),
                .stripe = ShdStripePresets::SlowAnimated().genStripe(),
                .slope = ShdSlopePresets::Translucent().genSlope(),
                .color = ShdColorPresets::WeakContrast().genColor(),
                .fog = ShdFogPresets::Medium().genFog(),
                .bloom = BloomPresets::Normal().genBloom()
            },
            .video = {
                .data = {
                    .defaultZoomIncrement = 2,
                    .isStatic = false
                },
                .animation = {
                    .overZoom = 2,
                    .showText = true,
                    .mps = 1
                },
                .exportation = {
                    .fps = 60,
                    .bitrate = 9000
                }
            }
        };
    }

    LRESULT RenderScene::renderSceneProc(const HWND hwnd, const UINT msg, const WPARAM wparam, const LPARAM lparam) {
        RenderScene &scene = *reinterpret_cast<RenderScene *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        scene.runAction(msg, wparam, lparam);
        return DefWindowProcW(hwnd, msg, wparam, lparam);
    }

    void RenderScene::runAction(const UINT msg, const WPARAM wparam, const LPARAM) {
        switch (msg) {
            case WM_LBUTTONDOWN: {
                SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                interactedMX = getMouseXOnIterationBuffer();
                interactedMY = getMouseYOnIterationBuffer();
                break;
            }
            case WM_LBUTTONUP: {
                SetCursor(LoadCursor(nullptr, IDC_CROSS));
                interactedMX = 0;
                interactedMY = 0;
            }
            case WM_MOUSEMOVE: {
                const uint16_t x = getMouseXOnIterationBuffer();
                const uint16_t y = getMouseYOnIterationBuffer();
                if (wparam == MK_LBUTTON && interactedMX > 0 && interactedMY > 0) {
                    SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                    const auto dx = static_cast<int16_t>(interactedMX - x);
                    const auto dy = static_cast<int16_t>(interactedMY - y);
                    const float m = attr.render.clarityMultiplier;
                    const float logZoom = attr.fractal.logZoom;
                    fp_complex &center = attr.fractal.center;
                    center = center.addCenterDouble(dex::value(static_cast<float>(dx) / m) / getDivisor(attr),
                                                    dex::value(static_cast<float>(dy) / m) / getDivisor(attr),
                                                    Perturbator::logZoomToExp10(logZoom));
                    interactedMX = x;
                    interactedMY = y;
                    requests.requestRecompute();
                } else {
                    SetCursor(LoadCursor(nullptr, IDC_CROSS));
                    if (renderer->iterationStagingBufferContext == nullptr) {
                        return;
                    }

                    if (auto it = static_cast<uint64_t>((*renderer->iterationStagingBufferContext)(x, y)); it != 0) {
                        setStatusMessage(Constants::Status::ITERATION_STATUS,
                                         std::format(L"I : {} ({}, {})", it, x, y));
                    }
                }
                break;
            }
            case WM_MOUSEWHEEL: {
                const int value = GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? 1 : -1;
                constexpr float increment = Constants::Fractal::ZOOM_INTERVAL;

                attr.fractal.logZoom = std::max(Constants::Fractal::ZOOM_MIN,
                                                attr.fractal.logZoom);
                if (value == 1) {
                    const std::array<dex, 2> offset = offsetConversion(attr, getMouseXOnIterationBuffer(),
                                                                       getMouseYOnIterationBuffer());
                    const double mzi = 1.0 / pow(10, Constants::Fractal::ZOOM_INTERVAL);
                    float &logZoom = attr.fractal.logZoom;
                    logZoom += increment;
                    attr.fractal.center = attr.fractal.center.addCenterDouble(
                        offset[0] * (1 - mzi),
                        offset[1] * (1 - mzi),
                        Perturbator::logZoomToExp10(logZoom));
                }
                if (value == -1) {
                    const std::array<dex, 2> offset = offsetConversion(attr, getMouseXOnIterationBuffer(),
                                                                       getMouseYOnIterationBuffer());
                    const double mzo = 1.0 / pow(10, -Constants::Fractal::ZOOM_INTERVAL);
                    float &logZoom = attr.fractal.logZoom;
                    logZoom -= increment;
                    attr.fractal.center = attr.fractal.center.addCenterDouble(
                        offset[0] * (1 - mzo),
                        offset[1] * (1 - mzo),
                        Perturbator::logZoomToExp10(logZoom));
                }


                requests.requestRecompute();
                break;
            }
            default: {
                //noop
            }
        }
    }

    std::array<dex, 2> RenderScene::offsetConversion(const Attribute &settings, const int mx, const int my) const {
        using namespace Constants::Fractal;
        const double ox = static_cast<double>(mx) - static_cast<double>(getIterationBufferWidth(settings)) / 2.0;
        const double oy = static_cast<double>(my) - static_cast<double>(getIterationBufferHeight(settings)) / 2.0;

        return {
            dex::value(std::abs(ox) < INTENTIONAL_ERROR_OFFSET_MIN_PIX ? INTENTIONAL_ERROR_OFFSET_MIN_PIX : ox) /
            getDivisor(settings)
            / settings.render.clarityMultiplier,
            dex::value(std::abs(oy) < INTENTIONAL_ERROR_OFFSET_MIN_PIX ? INTENTIONAL_ERROR_OFFSET_MIN_PIX : oy) /
            getDivisor(settings)
            / settings.render.clarityMultiplier
        };
    }

    dex RenderScene::getDivisor(const Attribute &settings) {
        dex v = dex::ZERO;
        dex_exp::exp10(&v, settings.fractal.logZoom);
        return v;
    }

    uint16_t RenderScene::getClientWidth() const {
        RECT rect;
        GetClientRect(wc.getWindow().getWindowHandle(), &rect);
        return static_cast<uint16_t>(rect.right - rect.left);
    }

    uint16_t RenderScene::getClientHeight() const {
        RECT rect;
        GetClientRect(wc.getWindow().getWindowHandle(), &rect);
        return static_cast<uint16_t>(rect.bottom - rect.top);
    }

    uint16_t RenderScene::getIterationBufferWidth(const Attribute &settings) const {
        const float multiplier = settings.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(getClientWidth()) * multiplier);
    }

    uint16_t RenderScene::getIterationBufferHeight(const Attribute &settings) const {
        const float multiplier = settings.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(getClientHeight()) * multiplier);
    }

    void RenderScene::applyDefaultAttr() {
        wc.core.getLogicalDevice().waitDeviceIdle();
        attr = genDefaultAttr();
    }


    void RenderScene::applyCreateImage() {
        const uint32_t frameIndex = renderer->getFrameIndex();
        wc.getSyncObject().getFence(frameIndex).wait();

        if (requests.createImageRequestedFilename.empty()) {
            requests.createImageRequestedFilename = IOUtilities::ioFileDialog(
                L"Save image", Constants::Extension::DESC_IMAGE,
                IOUtilities::SAVE_FILE,
                Constants::Extension::IMAGE)->string();
        }
        const auto &imgCtx = wc.getSharedImageContext().getImageContextMF(
            SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY)[frameIndex];

        vkh::BufferContext bufCtx = vkh::BufferContext::createContext(wc.core, {
                                                                          .size = imgCtx.capacity,
                                                                          .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                          .properties =
                                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                      });
        vkh::BufferContext::mapMemory(wc.core, bufCtx);
        //NEW COMMAND BUFFER
        {
            const auto executor = vkh::ScopedNewCommandBufferExecutor(wc.core, wc.getCommandPool());
            vkh::BarrierUtils::cmdImageMemoryBarrier(executor.getCommandBufferHandle(), imgCtx.image,
                                                     VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 0, 1,
                                                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT);
            vkh::BufferImageContextUtils::cmdCopyImageToBuffer(executor.getCommandBufferHandle(), imgCtx, bufCtx);
        }
        vkh::BufferContext::unmapMemory(wc.core, bufCtx);

        auto img = cv::Mat(static_cast<int>(imgCtx.extent.height), static_cast<int>(imgCtx.extent.width), CV_16UC4,
                           bufCtx.mappedMemory);
        cv::cvtColor(img, img, cv::COLOR_RGBA2BGRA);
        cv::imwrite(requests.createImageRequestedFilename, img);
        vkh::BufferContext::destroyContext(wc.core, bufCtx);
    }

    void RenderScene::applyShaderAttr(const Attribute &attr) const {
        wc.core.getLogicalDevice().waitDeviceIdle();
        renderer->rendererIteration->setPalette(attr.shader.palette);
        renderer->rendererStripe->setStripe(attr.shader.stripe);
        renderer->rendererSlope->setSlope(attr.shader.slope);
        renderer->rendererColor->setColor(attr.shader.color);
        renderer->rendererFog->setFog(attr.shader.fog);
        renderer->rendererBloom->setBloom(attr.shader.bloom);
        renderer->rendererLinearInterpolation->setLinearInterpolation(attr.render.linearInterpolation);
        renderer->rendererBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG, attr.shader.fog.radius);
        renderer->rendererBoxBlur->
                setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM, attr.shader.bloom.radius);
    }

    void RenderScene::refreshResizeParams() {
        const uint16_t iw = getIterationBufferWidth(attr);
        const uint16_t ih = getIterationBufferHeight(attr);
        const auto &[dWidth, dHeight] = getBlurredImageExtent();
        const auto &[sWidth, sHeight] = getSwapchainRenderContextExtent();

        renderer->rendererDownsampleForBlur->setRescaledResolution(0, {dWidth, dHeight});
        renderer->rendererDownsampleForBlur->setRescaledResolution(1, {dWidth, dHeight});
        renderer->rendererPresent->setRescaledResolution({sWidth, sHeight});
        renderer->rendererIteration->resetIterationBuffer(iw, ih);
        iterationMatrix = std::make_unique<Matrix<double> >(iw, ih);
        renderer->iterationStagingBufferContext = std::make_unique<GraphicsMatrixBuffer<double> >(
            wc.core, iw, ih, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    }

    void RenderScene::initRenderer() {
        wc.core.getLogicalDevice().waitDeviceIdle();
        renderer = std::make_unique<RenderSceneRenderer>(engine, wc.getAttachmentIndex());
    }


    void RenderScene::applyResize() {
        wc.core.getLogicalDevice().waitDeviceIdle();
        refreshSharedImgContext();
        refreshRenderContext();
        refreshResizeParams();
    }

    void RenderScene::refreshRenderContext() const {
        for (auto &context: wc.getRenderContexts()) {
            context->recreate();
        }

        for (const auto &sp: renderer->configurators) {
            sp->renderContextRefreshed();
        }
    }


    void RenderScene::refreshSharedImgContext() const {
        using namespace SharedImageContextIndices;
        auto &sharedImg = wc.getSharedImageContext();
        sharedImg.cleanupContexts();
        auto iiiGetter = [](const VkExtent2D extent, const VkFormat format, const VkImageUsageFlags usage) {
            return vkh::ImageInitInfo{
                .imageType = VK_IMAGE_TYPE_2D,
                .imageViewType = VK_IMAGE_VIEW_TYPE_2D,
                .imageFormat = format,
                .extent = {extent.width, extent.height, 1},
                .useMipmap = VK_FALSE,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .imageTiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = usage,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            };
        };

        const auto internalImageExtent = getInternalImageExtent();
        const auto blurredImageExtent = getBlurredImageExtent();

        sharedImg.appendMultiframeImageContext(MF_MAIN_RENDER_IMAGE_PRIMARY,
                                               iiiGetter(internalImageExtent, VK_FORMAT_R16G16B16A16_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_SAMPLED_BIT));
        sharedImg.appendMultiframeImageContext(MF_MAIN_RENDER_IMAGE_SECONDARY,
                                               iiiGetter(internalImageExtent, VK_FORMAT_R16G16B16A16_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                                         VK_IMAGE_USAGE_SAMPLED_BIT));
        sharedImg.appendMultiframeImageContext(MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY,
                                               iiiGetter(blurredImageExtent, VK_FORMAT_R8G8B8A8_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_SAMPLED_BIT |
                                                         VK_IMAGE_USAGE_STORAGE_BIT));
        sharedImg.appendMultiframeImageContext(MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY,
                                               iiiGetter(blurredImageExtent, VK_FORMAT_R8G8B8A8_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_SAMPLED_BIT |
                                                         VK_IMAGE_USAGE_STORAGE_BIT));
    }

    void RenderScene::overwriteMatrixFromMap(const RFFDynamicMapBinary &map) const {
        wc.core.getLogicalDevice().waitDeviceIdle();
        const uint32_t iw = getIterationBufferWidth(attr);
        const uint32_t ih = getIterationBufferHeight(attr);
        if (iw != map.getMatrix().getWidth() || ih != map.getMatrix().getHeight()) {
            vkh::logger::log_err("Map size mismatch, {}x{} required but provided {}x{}", iw, ih,
                                 map.getMatrix().getWidth(), map.getMatrix().getHeight());
            return;
        }

        renderer->rendererIteration->setMaxIteration(static_cast<double>(map.getMaxIteration()));
        renderer->iterationStagingBufferContext->fill(map.getMatrix().getCanvas());
    }

    uint16_t RenderScene::getMouseXOnIterationBuffer() const {
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(wc.getWindow().getWindowHandle(), &cursor);
        const float multiplier = attr.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(cursor.x) * multiplier);
    }

    uint16_t RenderScene::getMouseYOnIterationBuffer() const {
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(wc.getWindow().getWindowHandle(), &cursor);
        const float multiplier = attr.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(getIterationBufferHeight(attr)) - static_cast<float>(cursor.y) *
                                     multiplier);
    }

    void RenderScene::recomputeThreaded() {
        state.createThread([this](const std::stop_token &) {
            Attribute settings = this->attr; //clone the attr
            beforeCompute(settings);
            const bool success = compute(settings);
            afterCompute(success);
        });
    }

    void RenderScene::beforeCompute(Attribute &attr) const {
        attr.fractal.maxIteration = attr.fractal.autoMaxIteration
                                        ? lastPeriod * attr.fractal.
                                          autoIterationMultiplier
                                        : this->attr.fractal.maxIteration;
        renderer->rendererIteration->setMaxIteration(static_cast<double>(attr.fractal.maxIteration));
    }

    bool RenderScene::compute(const Attribute &attr) {
        auto start = std::chrono::high_resolution_clock::now();
        const uint16_t w = getIterationBufferWidth(attr);
        const uint16_t h = getIterationBufferHeight(attr);
        uint32_t len = uint32_t(w) * h;

        if (state.interruptRequested()) return false;

        auto &calc = attr.fractal;

        const float logZoom = calc.logZoom;

        if (state.interruptRequested()) return false;

        setStatusMessage(Constants::Status::ZOOM_STATUS,
                         std::format(L"Z : {:.06f}E{:d}", pow(10, fmod(logZoom, 1)), static_cast<int>(logZoom)));

        const std::array<dex, 2> offset = offsetConversion(attr, 0, 0);
        dex dcMax = dex::ZERO;
        dex_trigonometric::hypot_approx(&dcMax, offset[0], offset[1]);
        const auto refreshInterval = Utilities::getRefreshInterval(logZoom);
        std::function actionPerRefCalcIteration = [refreshInterval, this, &start](const uint64_t p) {
            if (p % refreshInterval == 0) {
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format(std::locale(), L"P : {:L}", p));
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
            }
        };
        std::function actionPerCreatingTableIteration = [refreshInterval, this, &start
                ](const uint64_t p, const double i) {
            if (p % refreshInterval == 0) {
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format(L"A : {:.3f}%", i * 100));
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
            }
        };


        if (state.interruptRequested()) return false;
        switch (calc.reuseReferenceMethod) {
                using enum FrtReuseReferenceMethod;
            case CURRENT_REFERENCE: {
                if (auto p = dynamic_cast<DeepMandelbrotPerturbator *>(currentPerturbator.get())) {
                    currentPerturbator = p->reuse(calc, currentPerturbator->getDcMaxAsDoubleExp(), approxTableCache);
                }
                if (auto p = dynamic_cast<LightMandelbrotPerturbator *>(currentPerturbator.get())) {
                    currentPerturbator = p->reuse(calc, static_cast<double>(currentPerturbator->getDcMaxAsDoubleExp()),
                                                  approxTableCache);
                }
                break;
            }
            case CENTERED_REFERENCE: {
                uint64_t period = currentPerturbator->getReference()->longestPeriod();
                auto center = MandelbrotLocator::locateMinibrot(state, currentPerturbator.get(), approxTableCache,
                                                                CallbackExplore::getActionWhileFindingMinibrotCenter(
                                                                    *this, logZoom, period),
                                                                CallbackExplore::getActionWhileCreatingTable(
                                                                    *this, logZoom),
                                                                CallbackExplore::getActionWhileFindingZoom(*this)
                );
                if (center == nullptr) return false;

                FractalAttribute refCalc = calc;
                refCalc.center = center->perturbator->calc.center;
                refCalc.logZoom = center->perturbator->calc.logZoom;
                int refExp10 = Perturbator::logZoomToExp10(refCalc.logZoom);

                if (refCalc.logZoom > Constants::Fractal::ZOOM_DEADLINE) {
                    currentPerturbator = std::make_unique<DeepMandelbrotPerturbator>(
                                state, refCalc, center->perturbator->getDcMaxAsDoubleExp(),
                                refExp10,
                                period, approxTableCache, std::move(actionPerRefCalcIteration),
                                std::move(actionPerCreatingTableIteration))
                            ->reuse(calc, dcMax, approxTableCache);
                } else {
                    currentPerturbator = std::make_unique<LightMandelbrotPerturbator>(state, refCalc,
                                static_cast<double>(center->perturbator->getDcMaxAsDoubleExp()),
                                refExp10, period, approxTableCache, std::move(actionPerRefCalcIteration),
                                std::move(actionPerCreatingTableIteration))
                            ->reuse(calc, static_cast<double>(dcMax), approxTableCache);
                }
                break;
            }
            case DISABLED: {
                int exp10 = Perturbator::logZoomToExp10(logZoom);
                if (logZoom > Constants::Fractal::ZOOM_DEADLINE) {
                    currentPerturbator = std::make_unique<DeepMandelbrotPerturbator>(
                        state, calc, dcMax, exp10,
                        0, approxTableCache, std::move(actionPerRefCalcIteration),
                        std::move(actionPerCreatingTableIteration));
                } else {
                    currentPerturbator = std::make_unique<LightMandelbrotPerturbator>(
                        state, calc, static_cast<double>(dcMax), exp10,
                        0, approxTableCache, std::move(actionPerRefCalcIteration),
                        std::move(actionPerCreatingTableIteration));
                }
                break;
            }
            default: {
                //noop
            }
        }

        const MandelbrotReference *reference = currentPerturbator->getReference();
        if (reference == Constants::NullPointer::PROCESS_TERMINATED_REFERENCE || state.interruptRequested())
            return false;

        lastLogZoom = calc.logZoom;
        lastMaxIteration = calc.maxIteration;
        lastPeriod = reference->longestPeriod();
        size_t refLength = reference->length();
        size_t mpaLen;
        if (const auto t = dynamic_cast<LightMandelbrotPerturbator *>(currentPerturbator.get())) {
            mpaLen = t->getTable().getLength();
        }
        if (const auto t = dynamic_cast<DeepMandelbrotPerturbator *>(currentPerturbator.get())) {
            mpaLen = t->getTable().getLength();
        }

        setStatusMessage(Constants::Status::PERIOD_STATUS,
                         std::format(L"P : {:L} ({:L}, {:L})", lastPeriod, refLength, mpaLen));
        if (state.interruptRequested()) return false;


        std::atomic renderPixelsCount = 0;

        auto rendered = std::vector<bool>(len);

        auto previewer = ParallelArrayDispatcher<double>(
            state, *iterationMatrix, attr.render.threads,
            [attr, this, &renderPixelsCount, &rendered](const uint16_t x, const uint16_t y,
                                                        const uint16_t xRes, const uint16_t yRes, float,
                                                        float, const uint32_t i, double) {
                rendered[i] = true;
                const auto dc = offsetConversion(attr, x, y);
                const double iteration = currentPerturbator->iterate(dc[0], dc[1]);
                renderer->iterationStagingBufferContext->set(x, y, iteration);

                auto my = static_cast<int16_t>(y + 1);
                while (my < yRes && !rendered[my * xRes + x]) {
                    renderer->iterationStagingBufferContext->set(x, my, iteration);
                    ++my;
                }

                ++renderPixelsCount;
                return iteration;
            });

        renderer->iterationStagingBufferContext->fillZero();

        auto statusThread = std::jthread([&renderPixelsCount, len, this, &start](const std::stop_token &stop) {
            while (!stop.stop_requested()) {
                float ratio = static_cast<float>(renderPixelsCount.load()) / static_cast<float>(len) * 100;
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format(L"C : {:.3f}%", ratio));

                Sleep(Constants::Status::SET_PROCESS_INTERVAL_MS);
            }
        });

        previewer.dispatch();

        statusThread.request_stop();
        statusThread.join();

        if (state.interruptRequested()) return false;

        const auto syncer = ParallelDispatcher(
            state, w, h, attr.render.threads,
            [this](const uint16_t x, const uint16_t y, uint16_t, uint16_t, float, float, uint32_t) {
                renderer->iterationStagingBufferContext->set(x, y, (*iterationMatrix)(x, y));
            });

        syncer.dispatch();

        if (state.interruptRequested()) return false;
        setStatusMessage(Constants::Status::RENDER_STATUS, L"Done");

        return true;
    }

    void RenderScene::afterCompute(const bool success) {
        if (!success) {
            vkh::logger::log("Recompute cancelled.");
        }
        if (success && attr.fractal.reuseReferenceMethod == FrtReuseReferenceMethod::CENTERED_REFERENCE) {
            attr.fractal.reuseReferenceMethod = FrtReuseReferenceMethod::CURRENT_REFERENCE;
        }
        idleCompute = true;
        backgroundThreads.notifyAll();
    }


    void RenderScene::destroy() {
        state.cancel();
        engine.getCore().getLogicalDevice().waitDeviceIdle();
        renderer = nullptr;
    }
}
