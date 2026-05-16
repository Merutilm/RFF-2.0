//
// Created by Merutilm on 2025-08-08.
//

#include "AppRenderManager.hpp"

#include "../calc/dex_exp.h"
#include "../locator/MB2Locator.h"
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
#include "../vulkan/GPCIterationPalette.hpp"
#include "../vulkan/RCC1.hpp"
#include "../vulkan/RCC3.hpp"
#include "../vulkan/RCC4.hpp"
#include "../vulkan/RCC5.hpp"
#include "../vulkan/RCCDownsampleForBlur.hpp"
#include "../vulkan/RCCPresent.hpp"
#include "../vulkan/RCCStatic2Image.hpp"
#include "../vulkan/SharedDescriptorTemplate.hpp"
#include "../vulkan/SharedImageContextIndices.hpp"
#include "CallbackExplore.hpp"
#include "IOUtilities.h"
#include "Utilities.h"
#include "opencv2/opencv.hpp"
#include "vulkan_helper/engine/executor/RenderPassFullscreenRecorder.hpp"
#include "vulkan_helper/engine/executor/ScopedNewCommandBufferExecutor.hpp"
#include "vulkan_helper/engine/window/win/NativeWindow.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"


namespace merutilm::rff2 {
    AppRenderManager::AppRenderManager(vkh::Engine &engine, vkh::WindowContext &wc, vkh::SharedResource &sr,
                                       std::array<std::wstring, Constants::Status::LENGTH> *statusMessageRef) :
        EngineHandler(engine), wc(wc), sr(sr), settings(genDefaultAttr()), statusMessageRef(statusMessageRef) {
        AppRenderManager::init();
    }

    AppRenderManager::~AppRenderManager() { AppRenderManager::cleanup(); }

    void AppRenderManager::init() {
        refreshSharedImgContext();
        attachRenderContext();
        initRenderer();
        refreshRenderContext();
        refreshResizeParams();
        applyShaderAttr(settings);

        addMouseListeners();
        requests.requestRecompute();
    }


    void AppRenderManager::attachRenderContext() const {
        const auto swapchainImageContextGetter = [this] {
            auto &swapchain = wc.getSwapchain();
            return vkh::ImageContext::fromSwapchain(wc.core, swapchain);
        };
        wc.attachRenderContext<RCC0>([this] { return getInternalImageExtent(); }, swapchainImageContextGetter);
        wc.attachRenderContext<RCC1>([this] { return getInternalImageExtent(); }, swapchainImageContextGetter);
        wc.attachRenderContext<RCC2>([this] { return getInternalImageExtent(); }, swapchainImageContextGetter);
        wc.attachRenderContext<RCCDownsampleForBlur>([this] { return getBlurredImageExtent(); },
                                                     swapchainImageContextGetter);
        wc.attachRenderContext<RCC3>([this] { return getInternalImageExtent(); }, swapchainImageContextGetter);
        wc.attachRenderContext<RCC4>([this] { return getInternalImageExtent(); }, swapchainImageContextGetter);
        wc.attachRenderContext<RCC5>([this] { return getInternalImageExtent(); }, swapchainImageContextGetter);
        wc.attachRenderContext<RCCPresent>([this] { return getSwapchainRenderContextExtent(); },
                                           swapchainImageContextGetter);
    }

    void AppRenderManager::resolveWindowResizeEnd() {
        if (!wc.getWindow()->canRenderable()) {
            return;
        }
        wc.core.getLogicalDevice().waitDeviceIdle();

        vkh::Swapchain &swapchain = wc.getSwapchain();
        swapchain.recreate();

        requests.requestResize();
        requests.requestRecompute();
    }


    void AppRenderManager::render() {

        if (requests.defaultSettingsRequested) {
            applyDefaultSettings();
            requests.defaultSettingsRequested.exchange(false);
            backgroundThreads.notifyAll();
        }
        if (requests.shaderRequested) {
            applyShaderAttr(settings);
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
            // it is threaded, not idle
        }

        if (requests.createImageRequested) {
            applyCreateImage();
            requests.createImageRequested.exchange(false);
            backgroundThreads.notifyAll();
        }


        renderer->execute();
    }


    Settings AppRenderManager::genDefaultAttr() {
        return Settings{
                .fractal =
                        FractalSettings{
                            .general = {
                                .bailout = 2.00001f,
                                .logZoom = 2
                            },
                            .reference = {
                                .center = fixed_point_complex_i1("-0.85", "0", Perturbator::logZoomToExp10(2)),
                                .useParallelRefCalculation = false,
                                .sync = CalculationPresets::UltraFast().genRefSync(),
                                .compression = CalculationPresets::UltraFast().genRefComp(),
                                .reuse = FrtReferenceReuseMethod::DISABLED,
                            },
                            .mpa = CalculationPresets::UltraFast().genMPA(),
                            .perturb = {
                                .maxIteration = 300,
                                .decimalizeIterationMethod = FrtDecimalizeIterationMethod::LOG_LOG,
                                .autoMaxIteration = true,
                                .autoIterationMultiplier = 100,
                                .absoluteIterationMode = false
                            }
                        },
                .render = RenderPresets::High().genRender(),
                .shader = {.palette = ShdPalettePresets::LongRandom64().genPalette(),
                           .stripe = ShdStripePresets::Disabled().genStripe(),
                           .slope = ShdSlopePresets::Disabled().genSlope(),
                           .color = ShdColorPresets::Disabled().genColor(),
                           .fog = ShdFogPresets::Disabled().genFog(),
                           .bloom = BloomPresets::Disabled().genBloom()},
                .video = {.data = {.defaultZoomIncrement = 2, .isStatic = false},
                          .animation = {.overZoom = 2, .showText = true, .mps = 1},
                          .exportation = {.fps = 60, .bitrate = 9000}}};
    }

    void AppRenderManager::addMouseListeners() {
        auto &eventSystem = wc.getWindow()->eventSystem;

        eventSystem.mouse.onMouseDown.add(
                [](vkh::KeyInput::MouseButton, int, int) { SetCursor(LoadCursor(nullptr, IDC_SIZEALL)); });
        eventSystem.mouse.onMouseUp.add(
                [](vkh::KeyInput::MouseButton, int, int) { SetCursor(LoadCursor(nullptr, IDC_CROSS)); });
        eventSystem.mouse.onMouseEnter.add([] { SetCursor(LoadCursor(nullptr, IDC_CROSS)); });
        eventSystem.mouse.onMouseExit.add([] { SetCursor(LoadCursor(nullptr, IDC_ARROW)); });


        eventSystem.mouse.onMouseMove.add([this](const int mx, const int my) {
            const uint16_t x = getMouseXOnIterationBuffer(mx);
            const uint16_t y = getMouseYOnIterationBuffer(my);
            if (renderer->iterationStagingBufferContext == nullptr) {
                return;
            }

            if (auto it = static_cast<uint64_t>((*renderer->iterationStagingBufferContext)(x, y)); it != 0) {
                setStatusMessage(Constants::Status::ITERATION_STATUS, std::format(L"I : {} ({}, {})", it, x, y));
            }
        });

        eventSystem.mouseDrag.onMouseDrag.add([this](const vkh::KeyInput::MouseButton mb, const int mx, const int my, const int mdx, const int mdy) {
            const int16_t x = getMouseXOnIterationBuffer(mx);
            const int16_t y = getMouseYOnIterationBuffer(my);
            const int16_t dx = getMouseXOnIterationBuffer(mx - mdx) - x;
            const int16_t dy = getMouseYOnIterationBuffer(my - mdy) - y;

            if (mb == vkh::KeyInput::MouseButton::LEFT) {
                SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                const float m = settings.render.clarityMultiplier;
                const float logZoom = settings.fractal.general.logZoom;
                const int exp10 = Perturbator::logZoomToExp10(logZoom);

                fixed_point_complex_i1 &center = settings.fractal.reference.center;
                center.set_exp10(exp10);
                const fixed_point_complex_i1 add(dex(static_cast<float>(dx) / m) / getDivisor(settings),
                                                 dex(static_cast<float>(dy) / m) / getDivisor(settings), exp10);
                fixed_point_complex_i1::add(center, center, add);

                requests.requestRecompute();
            }
        });
        eventSystem.mouseWheel.onMouseScroll.add([this](const int value) {
            constexpr float increment = Constants::Fractal::ZOOM_INTERVAL;

            settings.fractal.general.logZoom = std::max(Constants::Fractal::ZOOM_MIN, settings.fractal.general.logZoom);
            int mx;
            int my;
            wc.getWindow()->getMousePosition(&mx, &my);
            if (value > 0) {
                const std::array<dex, 2> offset =
                        offsetConversion(settings, getMouseXOnIterationBuffer(mx), getMouseYOnIterationBuffer(my));
                const double mzi = 1.0 / pow(10, Constants::Fractal::ZOOM_INTERVAL);
                float &logZoom = settings.fractal.general.logZoom;
                logZoom += increment;

                fixed_point_complex_i1 &center = settings.fractal.reference.center;
                const int exp10 = Perturbator::logZoomToExp10(logZoom);
                center.set_exp10(exp10);
                const fixed_point_complex_i1 add(offset[0] * dex(1 - mzi), offset[1] * dex(1 - mzi), exp10);
                fixed_point_complex_i1::add(center, center, add);
            } else if (value < 0) {
                const std::array<dex, 2> offset =
                        offsetConversion(settings, getMouseXOnIterationBuffer(mx), getMouseYOnIterationBuffer(my));
                const double mzo = 1.0 / pow(10, -Constants::Fractal::ZOOM_INTERVAL);
                float &logZoom = settings.fractal.general.logZoom;
                logZoom -= increment;


                fixed_point_complex_i1 &center = settings.fractal.reference.center;
                const int exp10 = Perturbator::logZoomToExp10(logZoom);
                center.set_exp10(exp10);
                const fixed_point_complex_i1 add(offset[0] * dex(1 - mzo), offset[1] * dex(1 - mzo), exp10);
                fixed_point_complex_i1::add(center, center, add);
            }


            requests.requestRecompute();
        });
    }


    std::array<dex, 2> AppRenderManager::offsetConversion(const Settings &settings, const int mx, const int my) const {
        using namespace Constants::Fractal;
        const double ox = static_cast<double>(mx) - static_cast<double>(getIterationBufferWidth(settings)) / 2.0;
        const double oy = static_cast<double>(my) - static_cast<double>(getIterationBufferHeight(settings)) / 2.0;

        return {dex(std::abs(ox) < INTENTIONAL_ERROR_OFFSET_MIN_PIX ? INTENTIONAL_ERROR_OFFSET_MIN_PIX : ox) /
                        getDivisor(settings) / dex(settings.render.clarityMultiplier),
                dex(std::abs(oy) < INTENTIONAL_ERROR_OFFSET_MIN_PIX ? INTENTIONAL_ERROR_OFFSET_MIN_PIX : oy) /
                        getDivisor(settings) / dex(settings.render.clarityMultiplier)};
    }

    dex AppRenderManager::getDivisor(const Settings &settings) { return dex_exp::exp10(settings.fractal.general.logZoom); }


    uint16_t AppRenderManager::getIterationBufferWidth(const Settings &settings) const {
        const float multiplier = settings.render.clarityMultiplier;
        uint16_t w;
        wc.getWindow()->getRenderWindowExtent(&w, nullptr);
        return static_cast<uint16_t>(static_cast<float>(w) * multiplier);
    }

    uint16_t AppRenderManager::getIterationBufferHeight(const Settings &settings) const {
        const float multiplier = settings.render.clarityMultiplier;
        uint16_t h;
        wc.getWindow()->getRenderWindowExtent(nullptr, &h);
        return static_cast<uint16_t>(static_cast<float>(h) * multiplier);
    }

    void AppRenderManager::applyDefaultSettings() {
        wc.core.getLogicalDevice().waitDeviceIdle();
        settings = genDefaultAttr();
    }


    void AppRenderManager::applyCreateImage() {
        const uint32_t frameIndex = renderer->getFrameIndex();
        wc.getSyncObject().getFence(frameIndex).wait();

        if (requests.createImageRequestedFilename.empty()) {
            const auto path = IOUtilities::ioFileDialog(L"Save image", Constants::File::DESC_IMAGE,
                                                        IOUtilities::SAVE_FILE, Constants::File::EXT_IMAGE);

            if (path == nullptr)
                return;

            requests.createImageRequestedFilename = path->string();
        }
        const auto &imgCtx = wc.getSharedImageContext().getImageContextMF(
                SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY)[frameIndex];

        vkh::BufferContext bufCtx =
                vkh::BufferContext::createContext(wc.core, {
                                                                   .size = imgCtx.capacity,
                                                                   .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                   .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                           });
        vkh::BufferContext::mapMemory(wc.core, bufCtx);
        // NEW COMMAND BUFFER
        {
            const auto executor = vkh::ScopedNewCommandBufferExecutor(wc.core, wc.getCommandPool());
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                    executor.getCommandBufferHandle(), imgCtx.image, VK_ACCESS_SHADER_WRITE_BIT,
                    VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
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

    void AppRenderManager::applyShaderAttr(const Settings &settings) const {
        wc.core.getLogicalDevice().waitDeviceIdle();
        renderer->rendererIteration->setPalette(settings.shader.palette);
        renderer->rendererStripe->setStripe(settings.shader.stripe);
        renderer->rendererSlope->setSlope(settings.shader.slope);
        renderer->rendererColor->setColor(settings.shader.color);
        renderer->rendererFog->setFog(settings.shader.fog);
        renderer->rendererBloom->setBloom(settings.shader.bloom);
        renderer->rendererLinearInterpolation->setLinearInterpolation(settings.render.linearInterpolation);
        renderer->rendererBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG, settings.shader.fog.radius);
        renderer->rendererBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM, settings.shader.bloom.radius);
    }

    void AppRenderManager::refreshResizeParams() {
        const uint16_t iw = getIterationBufferWidth(settings);
        const uint16_t ih = getIterationBufferHeight(settings);
        const auto &[dWidth, dHeight] = getBlurredImageExtent();
        const auto &[sWidth, sHeight] = getSwapchainRenderContextExtent();

        renderer->rendererDownsampleForBlur->setRescaledResolution(0, {dWidth, dHeight});
        renderer->rendererDownsampleForBlur->setRescaledResolution(1, {dWidth, dHeight});
        renderer->rendererPresent->setRescaledResolution({sWidth, sHeight});
        renderer->rendererIteration->resetIterationBuffer(iw, ih);
        iterationMatrix = std::make_unique<Matrix<double>>(iw, ih);
        renderer->iterationStagingBufferContext = std::make_unique<GraphicsMatrixBuffer<double>>(
                wc.core, iw, ih, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    }

    void AppRenderManager::initRenderer() {
        wc.core.getLogicalDevice().waitDeviceIdle();
        renderer = std::make_unique<AppRenderer>(engine, wc, *sr.vertexBufferPP, *sr.indexBufferPP);
    }


    void AppRenderManager::applyResize() {
        wc.core.getLogicalDevice().waitDeviceIdle();
        refreshSharedImgContext();
        refreshRenderContext();
        refreshResizeParams();
    }

    void AppRenderManager::refreshRenderContext() const {
        for (auto &context: wc.getRenderContexts()) {
            context->recreate();
        }

        for (const auto &sp: renderer->configurators) {
            sp->renderContextRefreshed();
        }
    }


    void AppRenderManager::refreshSharedImgContext() const {
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
        sharedImg.appendMultiframeImageContext(
                MF_MAIN_RENDER_IMAGE_SECONDARY,
                iiiGetter(internalImageExtent, VK_FORMAT_R16G16B16A16_UNORM,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT));
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

    void AppRenderManager::overwriteMatrixFromMap(const RFFDynamicMapBinary &map) const {
        wc.core.getLogicalDevice().waitDeviceIdle();
        const uint32_t iw = getIterationBufferWidth(settings);
        const uint32_t ih = getIterationBufferHeight(settings);
        if (iw != map.getMatrix().getWidth() || ih != map.getMatrix().getHeight()) {
            vkh::logger::log_err("Map size mismatch, {}x{} required but provided {}x{}", iw, ih,
                                 map.getMatrix().getWidth(), map.getMatrix().getHeight());
            return;
        }

        renderer->rendererIteration->setMaxIteration(static_cast<double>(map.getMaxIteration()));
        renderer->iterationStagingBufferContext->fill(map.getMatrix().getCanvas());
    }

    uint16_t AppRenderManager::getMouseXOnIterationBuffer(const int mx) const {
        const float multiplier = settings.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(mx) * multiplier);
    }

    uint16_t AppRenderManager::getMouseYOnIterationBuffer(const int my) const {
        const float multiplier = settings.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(getIterationBufferHeight(settings)) -
                                     static_cast<float>(my) * multiplier);
    }

    void AppRenderManager::recomputeThreaded() {
        state.createThread([this](const std::stop_token &) {
            const Settings settings = this->settings; // clone the settings
            const auto start = std::chrono::high_resolution_clock::now();
            bool success = prepareRenderData(start, settings);

            if (success) {
                beforeIterationFill();
                success = fillIteration(start, settings);
            }
            afterCompute(success);
        });
    }

    void AppRenderManager::beforeIterationFill() const {
        renderer->rendererIteration->setMaxIteration(static_cast<double>(renderData->fractalSettings.perturb.maxIteration));
    }

    bool AppRenderManager::prepareRenderData(const std::chrono::time_point<std::chrono::high_resolution_clock> &start, const Settings &settings) {

        if (state.interruptRequested())
            return false;

        auto &frt = settings.fractal;

        const float logZoom = frt.general.logZoom;

        if (state.interruptRequested())
            return false;

        setStatusMessage(Constants::Status::ZOOM_STATUS,
                         std::format(L"Z : {:.06f}E{:d}", pow(10, fmod(logZoom, 1)), static_cast<int>(logZoom)));

        const std::array<dex, 2> offset = offsetConversion(settings, 0, 0);
        const dex dcMax = dex_trig::hypot_approx(offset[0], offset[1]);

        uint64_t capacity = renderData && renderData->getReference()
                                    ? renderData->getReference()->length()
                                    : 0;


        const auto refreshInterval = Utilities::getRefreshInterval(logZoom);
        std::function actionPerRefCalcIteration = [refreshInterval, this, &start](const uint64_t p) {
            if (p % refreshInterval == 0) {
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format(std::locale(), L"P : {:L}", p));
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
            }
        };
        std::function actionPerCreatingTableIteration = [refreshInterval, this, &start](const uint64_t p,
                                                                                        const double i) {
            if (p % refreshInterval == 0) {
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format(L"A : {:.3f}%", i * 100));
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
            }
        };


        if (state.interruptRequested())
            return false;


        switch (frt.reference.reuse) {
            using enum FrtReferenceReuseMethod;
            case CURRENT_REFERENCE: {
                if (!renderData || !renderData->getReference() || !renderData->getPerturbator()) {
                    MessageBox(nullptr, "Do not reuse Reference during reference calculation!!!", "Error",
                               MB_ICONERROR | MB_OK);
                    return false;
                }
                renderData->translate(frt.general.logZoom, renderData->getPerturbator()->dcMax, frt.perturb,
                                      frt.reference.center);
                break;
            }
            case CENTERED_REFERENCE: {

                if (!renderData || !renderData->getReference() || !renderData->getPerturbator()) {
                    MessageBox(nullptr, "Do not reuse Reference during reference calculation!!!", "Error", MB_ICONERROR | MB_OK);
                    return false;
                }

                uint64_t period = renderData->getReference()->longestPeriod();
                const auto center = MB2Locator::locateMinibrot(
                        state, renderData.get(), CallbackExplore::getActionWhileFindingMBCenter(*this, logZoom, period),
                        CallbackExplore::getActionWhileCreatingTable(*this, logZoom),
                        CallbackExplore::getActionWhileFindingZoom(*this));
                if (center == nullptr)
                    return false;

                FractalSettings refCalc = frt;
                refCalc.reference.center = center->data->fractalSettings.reference.center;
                refCalc.general.logZoom = center->data->fractalSettings.general.logZoom;
                int refExp10 = Perturbator::logZoomToExp10(refCalc.general.logZoom);

                if (refCalc.general.logZoom > Constants::Fractal::ZOOM_DEADLINE) {
                    renderData = std::make_unique<DeepMB2RenderData>(
                                    state, refCalc, center->data->getPerturbator()->dcMax, refExp10, capacity,
                                    period, std::move(actionPerRefCalcIteration),
                                    std::move(actionPerCreatingTableIteration), false);
                } else {
                    renderData = std::make_unique<LightMB2RenderData>(
                                    state, refCalc, center->data->getPerturbator()->dcMax, refExp10, capacity,
                                    period, std::move(actionPerRefCalcIteration),
                                    std::move(actionPerCreatingTableIteration), false);
                }
                renderData->translate(frt.general.logZoom, dcMax, frt.perturb, frt.reference.center);
                break;
            }
            case DISABLED: {
                const int exp10 = Perturbator::logZoomToExp10(logZoom);
                if (logZoom > Constants::Fractal::ZOOM_DEADLINE) {
                    renderData = DeepMB2RenderData::generateNew(
                            state, frt, dcMax, exp10, capacity,
                            std::move(actionPerRefCalcIteration), std::move(actionPerCreatingTableIteration));
                } else {
                    renderData = LightMB2RenderData::generateNew(
                            state, frt, dcMax, exp10, capacity,
                            std::move(actionPerRefCalcIteration), std::move(actionPerCreatingTableIteration));
                }
                break;
            }
            default: {
                // noop
            }
        }

        const MB2ReferenceBase *reference = renderData->getReference();
        if (!reference || state.interruptRequested())
            return false;

        size_t refLength = reference->length();
        size_t mpaLen;
        if (const auto t = dynamic_cast<LightMB2RenderData *>(renderData.get())) {
            mpaLen = t->table->getLength();
        }
        if (const auto t = dynamic_cast<DeepMB2RenderData *>(renderData.get())) {
            mpaLen = t->table->getLength();
        }

        setStatusMessage(Constants::Status::PERIOD_STATUS,
                         std::format(L"P : {:L} ({:L}, {:L})", reference->longestPeriod(), refLength, mpaLen));
        if (state.interruptRequested())
            return false;


        return true;
    }


    bool AppRenderManager::fillIteration(const std::chrono::time_point<std::chrono::high_resolution_clock> &start, const Settings &settings) {
        std::atomic renderPixelsCount = 0;
        const uint16_t w = getIterationBufferWidth(settings);
        const uint16_t h = getIterationBufferHeight(settings);
        uint32_t len = uint32_t(w) * h;

        auto rendered = std::vector<bool>(len);

        auto func = [settings, this, &renderPixelsCount, &rendered](const uint16_t x, const uint16_t y,
                                                                    const uint16_t xRes, const uint16_t yRes, float,
                                                                    float, const uint32_t i, double) {
            rendered[i] = true;
            const auto dc = offsetConversion(settings, x, y);
            const double iteration = renderData->getPerturbator()->iterate(dc[0], dc[1]);

            renderer->iterationStagingBufferContext->set(x, y, iteration);

            auto my = static_cast<int16_t>(y + 1);
            while (my < yRes && !rendered[my * xRes + x]) {
                renderer->iterationStagingBufferContext->set(x, my, iteration);
                ++my;
            }

            ++renderPixelsCount;
            return iteration;
        };
        auto previewer =
                ParallelArrayDispatcher<double>(state, *iterationMatrix, settings.render.threads, std::move(func));

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

        if (state.interruptRequested())
            return false;

        const auto syncer = ParallelDispatcher(
                state, w, h, settings.render.threads,
                [this](const uint16_t x, const uint16_t y, uint16_t, uint16_t, float, float, uint32_t) {
                    renderer->iterationStagingBufferContext->set(x, y, (*iterationMatrix)(x, y));
                });

        syncer.dispatch();

        if (state.interruptRequested())
            return false;
        setStatusMessage(Constants::Status::RENDER_STATUS, L"Done");

        return true;
    }

    void AppRenderManager::afterCompute(const bool success) {
        if (!success) {
            vkh::logger::log("Recompute cancelled.");
        }
        if (success && settings.fractal.reference.reuse == FrtReferenceReuseMethod::CENTERED_REFERENCE) {
            settings.fractal.reference.reuse = FrtReferenceReuseMethod::CURRENT_REFERENCE;
        }
        idleCompute = true;
        backgroundThreads.notifyAll();
    }


    void AppRenderManager::cleanup() {
        state.cancel();
        engine.getCore().getLogicalDevice().waitDeviceIdle();
        renderer = nullptr;
    }
} // namespace merutilm::rff2
