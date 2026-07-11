//
// Created by Merutilm on 2025-08-08.
//

#include "RFFApplication.hpp"

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
#include "../vulkan/GPCDownsampleForBlur.hpp"
#include "../vulkan/RCCStatic2Image.hpp"
#include "../vulkan/SharedDescriptorTemplate.hpp"
#include "../vulkan/SharedImageContextIndices.hpp"
#include "CallbackExplore.hpp"
#include "IOUtilities.h"
#include "Utilities.h"
#include "opencv2/opencv.hpp"
#include "vulkan_helper/engine/executor/ScopedNewCommandBufferExecutor.hpp"
#include "vulkan_helper/engine/window/PlatformWindow.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"
#include "vulkan_helper/util/BufferImageContextUtils.hpp"


namespace merutilm::rff2 {


    void RFFApplication::resolveWindowResize(const uint32_t w, const uint32_t h) {
        state.cancel();
        engine->getCore().getLogicalDevice().waitDeviceIdle();

        if (w <= 0 || h <= 0) {
            return;
        }
        //swapchain recreation is already processed in the application level

        refreshResizeParams({w, h});
        requests.requestRecompute();
        backgroundThreads.notifyAll();
    }


    void RFFApplication::update() {

        if (requests.defaultSettingsRequested) {
            applyDefaultSettings();
            requests.defaultSettingsRequested.exchange(false);
            backgroundThreads.notifyAll();
        }
        if (requests.shaderRequested) {
            applyShaderSettings(settings);
            requests.shaderRequested.exchange(false);
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


        renderer->render();
    }


    Settings RFFApplication::genDefaultAttr() {
#ifndef NDEBUG
        return Settings{
                .fractal =
                        FractalSettings{.general = {.bailout = 2.00001f, .logZoom = 2},
                                        .reference =
                                                {
                                                        .center = fixed_point_complex_i1(
                                                                "-0.85", "0", Perturbator::logZoomToExp10(2)),
                                                        .useParallelRefCalculation = false,
                                                        .sync = CalculationPresets::UltraFast().genRefSync(),
                                                        .compression = CalculationPresets::UltraFast().genRefComp(),
                                                        .reuse = FrtReferenceReuseMethod::DISABLED,
                                                },
                                        .sa = {.appliedTermsCount = 8, .validatedTermsCount = 1, .epsilonPower = -5},
                                        .mpa = CalculationPresets::UltraFast().genMPA(),
                                        .perturb = {.maxIteration = 300,
                                                    .decimalizeIterationMethod = FrtDecimalizeIterationMethod::LOG_LOG,
                                                    .autoMaxIteration = true,
                                                    .autoIterationMultiplier = 100,
                                                    .absoluteIterationMode = true}},
                .render = {.clarityMultiplier = 1.0f, .fps = 60, .linearInterpolation = true, .threads = 1},
                .shader = {.palette = ShdPalettePresets::LongRandom64().genPalette(),
                           .stripe = ShdStripePresets::Disabled().genStripe(),
                           .slope = ShdSlopePresets::Disabled().genSlope(),
                           .color = ShdColorPresets::Disabled().genColor(),
                           .fog = ShdFogPresets::Disabled().genFog(),
                           .bloom = BloomPresets::Disabled().genBloom()},
                .video = {.data = {.defaultZoomIncrement = 2, .isStatic = false},
                          .animation = {.overZoom = 2, .showText = true, .mps = 1},
                          .exportation = {.fps = 60, .bitrate = 9000}}};
#else
        return Settings{
                .fractal =
                        FractalSettings{.general = {.bailout = 2.00001f, .logZoom = 2},
                                        .reference =
                                                {
                                                        .center = fixed_point_complex_i1(
                                                                "-0.85", "0", Perturbator::logZoomToExp10(2)),
                                                        .useParallelRefCalculation = false,
                                                        .sync = CalculationPresets::UltraFast().genRefSync(),
                                                        .compression = CalculationPresets::UltraFast().genRefComp(),
                                                        .reuse = FrtReferenceReuseMethod::DISABLED,
                                                },
                                        .sa = {.appliedTermsCount = 8, .validatedTermsCount = 1, .epsilonPower = -5},
                                        .mpa = CalculationPresets::UltraFast().genMPA(),
                                        .perturb = {.maxIteration = 300,
                                                    .decimalizeIterationMethod = FrtDecimalizeIterationMethod::LOG_LOG,
                                                    .autoMaxIteration = true,
                                                    .autoIterationMultiplier = 100,
                                                    .absoluteIterationMode = false}},
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
#endif
    }

    complex<dex> RFFApplication::offsetConversion(const Settings &s, const int mx, const int my) const {
        using namespace Constants::Fractal;
        const double ox = static_cast<double>(mx) - static_cast<double>(getIterationBufferWidth()) / 2.0;
        const double oy = static_cast<double>(my) - static_cast<double>(getIterationBufferHeight()) / 2.0;

        return {dex(std::abs(ox) < INTENTIONAL_ERROR_OFFSET_MIN_PIX ? INTENTIONAL_ERROR_OFFSET_MIN_PIX : ox) /
                        getDivisor(s) / dex(s.render.clarityMultiplier),
                dex(std::abs(oy) < INTENTIONAL_ERROR_OFFSET_MIN_PIX ? INTENTIONAL_ERROR_OFFSET_MIN_PIX : oy) /
                        getDivisor(s) / dex(s.render.clarityMultiplier)};
    }

    dex RFFApplication::getDivisor(const Settings &settings) {
        return dex_exp::exp10(settings.fractal.general.logZoom);
    }


    uint16_t RFFApplication::calcIterationBufferWidth(const Settings &s) const {
        const float multiplier = s.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(rootWindowContext->getSwapchain().getSwapchainExtent().width) * multiplier);
    }

    uint16_t RFFApplication::calcIterationBufferHeight(const Settings &s) const {
        const float multiplier = s.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(rootWindowContext->getSwapchain().getSwapchainExtent().height) * multiplier);
    }

    uint16_t RFFApplication::getIterationBufferWidth() const {
        return renderer->rcc0->iterationPalette->iterWidth;
    }

    uint16_t RFFApplication::getIterationBufferHeight() const {
        return renderer->rcc0->iterationPalette->iterHeight;
    }



    void RFFApplication::addListeners() {
        Application::addListeners();
        auto &eventSystem = rootWindowContext->getWindow()->eventSystem;

        eventSystem.window.onMaximize.add([this] {
            int w;
            int h;
            glfwGetWindowSize(rootWindowContext->getWindow()->getWindow(), &w, &h);
            resolveWindowResize(w, h);
        });
        eventSystem.window.onRestore.add([this] {
            int w;
            int h;
            glfwGetWindowSize(rootWindowContext->getWindow()->getWindow(), &w, &h);
            resolveWindowResize(w, h);
        });
        eventSystem.resize.onResize.add([this](const int w, const int h) {
            resolveWindowResize(w, h);
        });

        eventSystem.mouse.onMouseDown.add([this](int, int, int) {
            GLFWwindow *window = rootWindowContext->getWindow()->getWindow();
            glfwSetCursor(window, cursorManager->arrowCursor);
        });
        eventSystem.mouse.onMouseUp.add([this](int, int, int) {
            GLFWwindow *window = rootWindowContext->getWindow()->getWindow();
            glfwSetCursor(window, cursorManager->crosshairCursor);
        });
        eventSystem.mouse.onMouseEnter.add([this] {
            GLFWwindow *window = rootWindowContext->getWindow()->getWindow();
            glfwSetCursor(window, cursorManager->crosshairCursor);
        });
        eventSystem.mouse.onMouseExit.add([this] {
            GLFWwindow *window = rootWindowContext->getWindow()->getWindow();
            glfwSetCursor(window, nullptr);
        });


        eventSystem.mouse.onMouseMove.add([this](const int mx, const int my) {
            const uint16_t x = getMouseXOnIterationBuffer(mx);
            const uint16_t y = getMouseYOnIterationBuffer(my);
            if (renderer->iterationStagingBufferContext == nullptr) {
                return;
            }
            auto it = static_cast<uint64_t>((*renderer->iterationStagingBufferContext)(x, y));
            setStatusMessage(Constants::Status::ITERATION_STATUS, std::format("I : {} ({}, {})", it, x, y));
        });

        eventSystem.mouseDrag.onMouseDrag.add(
                [this](const int mb, const int mx, const int my, const int mdx, const int mdy) {
                    const int16_t x = getMouseXOnIterationBuffer(mx);
                    const int16_t y = getMouseYOnIterationBuffer(my);
                    const auto dx = static_cast<int16_t>(getMouseXOnIterationBuffer(mx - mdx) - x);
                    const auto dy = static_cast<int16_t>(getMouseYOnIterationBuffer(my - mdy) - y);

                    if (mb == GLFW_MOUSE_BUTTON_LEFT) {
                        GLFWwindow *window = rootWindowContext->getWindow()->getWindow();
                        glfwSetCursor(window, cursorManager->arrowCursor);
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
            double mdx;
            double mdy;
            glfwGetCursorPos(rootWindowContext->getWindow()->getWindow(), &mdx, &mdy);
            const int mx = static_cast<int>(mdx);
            const int my = static_cast<int>(mdy);
            if (value > 0) {
                const complex<dex> offset =
                        offsetConversion(settings, getMouseXOnIterationBuffer(mx), getMouseYOnIterationBuffer(my));
                const double mzi = 1.0 / pow(10, Constants::Fractal::ZOOM_INTERVAL);
                float &logZoom = settings.fractal.general.logZoom;
                logZoom += increment;

                fixed_point_complex_i1 &center = settings.fractal.reference.center;
                const int exp10 = Perturbator::logZoomToExp10(logZoom);
                center.set_exp10(exp10);
                const fixed_point_complex_i1 add(offset.re * dex(1 - mzi), offset.im * dex(1 - mzi), exp10);
                fixed_point_complex_i1::add(center, center, add);
            } else if (value < 0) {
                const complex<dex> offset =
                        offsetConversion(settings, getMouseXOnIterationBuffer(mx), getMouseYOnIterationBuffer(my));
                const double mzo = 1.0 / pow(10, -Constants::Fractal::ZOOM_INTERVAL);
                float &logZoom = settings.fractal.general.logZoom;
                logZoom -= increment;


                fixed_point_complex_i1 &center = settings.fractal.reference.center;
                const int exp10 = Perturbator::logZoomToExp10(logZoom);
                center.set_exp10(exp10);
                const fixed_point_complex_i1 add(offset.re * dex(1 - mzo), offset.im * dex(1 - mzo), exp10);
                fixed_point_complex_i1::add(center, center, add);
            }


            requests.requestRecompute();
        });
    }



    void RFFApplication::applyDefaultSettings() {
        rootWindowContext->core.getLogicalDevice().waitDeviceIdle();
        settings = genDefaultAttr();
    }


    void RFFApplication::applyCreateImage() {
        const uint32_t frameIndex = renderer->getFrameIndex();
        rootWindowContext->getSyncObject().getFence(frameIndex).wait();

        if (requests.createImageRequestedFilename.empty()) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_IMAGE, IOUtilities::SAVE_FILE,
                                                        Constants::File::EXT_IMAGE);

            if (path == nullptr)
                return;

            requests.createImageRequestedFilename = path->string();
        }
        const auto &imgCtx = rootWindowContext->getSharedImageContext().getImageContextMF(
                SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY)[frameIndex];

        vkh::BufferContext bufCtx =
                vkh::BufferContext::createContext(rootWindowContext->core, {
                                                                   .size = imgCtx.capacity,
                                                                   .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                   .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                           });
        vkh::BufferContext::mapMemory(rootWindowContext->core, bufCtx);
        // NEW COMMAND BUFFER
        {
            const auto executor = vkh::ScopedNewCommandBufferExecutor(rootWindowContext->core, engine->getCommandPool());
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                    executor.getCommandBufferHandle(), imgCtx.image, VK_ACCESS_SHADER_WRITE_BIT,
                    VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT);
            vkh::BufferImageContextUtils::cmdCopyImageToBuffer(executor.getCommandBufferHandle(), imgCtx, bufCtx);
        }
        vkh::BufferContext::unmapMemory(rootWindowContext->core, bufCtx);

        auto img = cv::Mat(static_cast<int>(imgCtx.extent.height), static_cast<int>(imgCtx.extent.width), CV_16UC4,
                           bufCtx.mappedMemory);
        cv::cvtColor(img, img, cv::COLOR_RGBA2BGRA);
        cv::imwrite(requests.createImageRequestedFilename, img);
        vkh::BufferContext::destroyContext(rootWindowContext->core, bufCtx);
    }

    void RFFApplication::applyShaderSettings(const Settings &s) const {
        rootWindowContext->core.getLogicalDevice().waitDeviceIdle();
        renderer->rcc0->iterationPalette->setPalette(s.shader.palette);
        renderer->rcc1->stripe->setStripe(s.shader.stripe);
        renderer->rcc2->slope->setSlope(s.shader.slope);
        renderer->rcc2->color->setColor(s.shader.color);
        renderer->rcc3->fog->setFog(s.shader.fog);
        renderer->rcc4->bloom->setBloom(s.shader.bloom);
        renderer->rcc5->linearInterpolation->setLinearInterpolation(s.render.linearInterpolation);
        renderer->computeBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG, s.shader.fog.radius);
        renderer->computeBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM, s.shader.bloom.radius);
    }

    void RFFApplication::refreshResizeParams(const VkExtent2D swapchainExtent) {
        const uint16_t iw = calcIterationBufferWidth(settings);
        const uint16_t ih = calcIterationBufferHeight(settings);
        const auto &[dWidth, dHeight] = AppRenderer::getBlurredImageExtent(swapchainExtent, settings.render.clarityMultiplier);
        const auto &[sWidth, sHeight] = rootWindowContext->getSwapchain().getSwapchainExtent();

        renderer->rccDownsample->downsample->setRescaledResolution(GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_FOG, {dWidth, dHeight});
        renderer->rccDownsample->downsample->setRescaledResolution(GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_BLOOM, {dWidth, dHeight});
        renderer->rccPresent->present->setRescaledResolution({sWidth, sHeight});
        renderer->rcc0->iterationPalette->resetIterationBuffer(iw, ih);
        iterationMatrix = std::make_unique<Matrix<double>>(iw, ih);
        renderer->iterationStagingBufferContext = std::make_unique<GraphicsMatrixBuffer<double>>(
                rootWindowContext->core, iw, ih, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    }

    void RFFApplication::registerRenderers() {
        renderer = registerRenderer<AppRenderer>(*engine, *rootWindowContext, settings, [this]{renderImGui(); });
        createImGuiContext(renderer->imguiRenderContext);
    }

    void RFFApplication::onStart() {
        cursorManager = std::make_unique<CursorManager>(rootWindowContext->getWindow()->getWindow());
        applyShaderSettings(settings);
        refreshResizeParams(rootWindowContext->getSwapchain().getSwapchainExtent());
        requests.requestRecompute();
    }
    void RFFApplication::renderImGui() {

    }

    void RFFApplication::refreshSharedImgContexts(VkExtent2D extent) {
        using namespace SharedImageContextIndices;
        auto &sharedImg = rootWindowContext->getSharedImageContext();
        sharedImg.cleanupContexts();
        auto iiiGetter = [](const VkExtent2D ex, const VkFormat format, const VkImageUsageFlags usage) {
            return vkh::ImageInitInfo{
                    .imageType = VK_IMAGE_TYPE_2D,
                    .imageViewType = VK_IMAGE_VIEW_TYPE_2D,
                    .imageFormat = format,
                    .extent = {ex.width, ex.height, 1},
                    .useMipmap = VK_FALSE,
                    .arrayLayers = 1,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .imageTiling = VK_IMAGE_TILING_OPTIMAL,
                    .usage = usage,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            };
        };

        const auto internalImageExtent = AppRenderer::getInternalImageExtent(extent, settings.render.clarityMultiplier);
        const auto blurredImageExtent = AppRenderer::getBlurredImageExtent(extent, settings.render.clarityMultiplier);

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

    void RFFApplication::overwriteMatrixFromMap(const RFFDynamicMapBinary &map) const {
        rootWindowContext->core.getLogicalDevice().waitDeviceIdle();
        const uint32_t iw = getIterationBufferWidth();
        const uint32_t ih = getIterationBufferHeight();
        if (iw != map.getMatrix().getWidth() || ih != map.getMatrix().getHeight()) {
            vkh::logger::log_err("Map size mismatch, {}x{} required but provided {}x{}", iw, ih,
                                 map.getMatrix().getWidth(), map.getMatrix().getHeight());
            return;
        }

        renderer->rcc0->iterationPalette->setMaxIteration(static_cast<double>(map.getMaxIteration()));
        renderer->iterationStagingBufferContext->fill(map.getMatrix().getCanvas());
    }

    int16_t RFFApplication::getMouseXOnIterationBuffer(const int mx) const {
        const float multiplier = settings.render.clarityMultiplier;
        return static_cast<int16_t>(static_cast<float>(mx) * multiplier);
    }

    int16_t RFFApplication::getMouseYOnIterationBuffer(const int my) const {
        const float multiplier = settings.render.clarityMultiplier;
        return static_cast<int16_t>(static_cast<float>(getIterationBufferHeight()) -
                                    static_cast<float>(my) * multiplier);
    }

    void RFFApplication::recomputeThreaded() {
        state.createThread([this](const std::stop_token &) {
            const Settings s = this->settings; // clone the settings
            const auto start = std::chrono::high_resolution_clock::now();
            bool success = prepareRenderData(start, s);

            if (success) {
                beforeIterationFill();
                success = fillIteration(start, s);
            }
            afterCompute(success);
        });
    }

    void RFFApplication::beforeIterationFill() const {
        renderer->rcc0->iterationPalette->setMaxIteration(
                static_cast<double>(renderData->fractalSettings.perturb.maxIteration));
    }

    bool RFFApplication::prepareRenderData(const std::chrono::time_point<std::chrono::high_resolution_clock> &start,
                                             const Settings &s) {

        if (state.interruptRequested())
            return false;

        auto &frt = s.fractal;
        const float logZoom = frt.general.logZoom;

        setStatusMessage(Constants::Status::ZOOM_STATUS,
                         std::format("Z : {:.06f}E{:d}", pow(10, fmod(logZoom, 1)), static_cast<int>(logZoom)));

        const complex<dex> offset = offsetConversion(s, 0, 0);
        const dex dcMax = offset.norm_approx();

        uint64_t capacity = renderData && renderData->getReference() ? renderData->getReference()->length() : 0;

        std::function actionPerRefCalcIteration = [this, &start](const uint64_t p) {
            static float time = Utilities::getCurrentTime();
            const float elapsed = Utilities::getCurrentTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = Utilities::getCurrentTime();
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format(std::locale(), "P : {:L}", p));
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
            }
        };
        std::function actionPerCreatingTableIteration = [this, &start](const uint64_t p, const double i) {
            static float time = Utilities::getCurrentTime();
            const float elapsed = Utilities::getCurrentTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = Utilities::getCurrentTime();
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format("A : {:.3f}%", i * 100));
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
            }
        };


        if (state.interruptRequested())
            return false;


        switch (frt.reference.reuse) {
            using enum FrtReferenceReuseMethod;
            case CURRENT_REFERENCE: {
                if (!renderData || !renderData->getReference() || !renderData->getPerturbator()) {
                    vkh::logger::log_err("Do not reuse Reference during reference calculation!!!");
                    return false;
                }
                renderData->translate(frt.general.logZoom, renderData->getPerturbator()->dcMax, frt.perturb,
                                      frt.reference.center);
                break;
            }
            case CENTERED_REFERENCE: {

                if (!renderData || !renderData->getReference() || !renderData->getPerturbator()) {
                    vkh::logger::log_err("Do not reuse Reference during reference calculation!!!");
                    return false;
                }

                uint64_t period = renderData->getReference()->longestPeriod();
                const auto center = MB2Locator::locateMinibrot(
                        state, renderData.get(), CallbackExplore::getActionWhileFindingMBCenter(*this, period),
                        CallbackExplore::getActionWhileCreatingTable(*this),
                        CallbackExplore::getActionWhileFindingZoom(*this));
                if (center == nullptr)
                    return false;

                FractalSettings refCalc = frt;
                refCalc.reference.center = center->data->fractalSettings.reference.center;
                refCalc.general.logZoom = center->data->fractalSettings.general.logZoom;
                int refExp10 = Perturbator::logZoomToExp10(refCalc.general.logZoom);

                if (refCalc.general.logZoom > Constants::Fractal::ZOOM_DEADLINE) {
                    renderData = std::make_unique<DeepMB2RenderData>(
                            state, refCalc, center->data->getPerturbator()->dcMax, refExp10, capacity, period,
                            std::move(actionPerRefCalcIteration), std::move(actionPerCreatingTableIteration), false);
                } else {
                    renderData = std::make_unique<LightMB2RenderData>(
                            state, refCalc, center->data->getPerturbator()->dcMax, refExp10, capacity, period,
                            std::move(actionPerRefCalcIteration), std::move(actionPerCreatingTableIteration), false);
                }
                renderData->translate(frt.general.logZoom, dcMax, frt.perturb, frt.reference.center);
                break;
            }
            case DISABLED: {
                const int exp10 = Perturbator::logZoomToExp10(logZoom);

                if (logZoom > Constants::Fractal::ZOOM_DEADLINE) {
                    renderData = std::make_unique<DeepMB2RenderData>(state, frt, dcMax, exp10, capacity, 0,
                                                                     std::move(actionPerRefCalcIteration),
                                                                     std::move(actionPerCreatingTableIteration), false);
                } else {
                    renderData = std::make_unique<LightMB2RenderData>(
                            state, frt, dcMax, exp10, capacity, 0, std::move(actionPerRefCalcIteration),
                            std::move(actionPerCreatingTableIteration), false);
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
                         std::format("P : {:L} ({:L}, {:L})", reference->longestPeriod(), refLength, mpaLen));
        if (state.interruptRequested())
            return false;


        return true;
    }


    bool RFFApplication::fillIteration(const std::chrono::time_point<std::chrono::high_resolution_clock> &start,
                                         const Settings &s) {
        std::atomic renderPixelsCount = 0;
        const uint16_t w = getIterationBufferWidth();
        const uint16_t h = getIterationBufferHeight();
        uint32_t len = static_cast<uint32_t>(w) * h;

        auto rendered = std::vector<bool>(len);

        auto func = [&s, this, &renderPixelsCount, &rendered](const uint16_t x, const uint16_t y, const uint16_t xRes,
                                                             const uint16_t yRes, float, float, const uint32_t i,
                                                             double) {
            assert(i < rendered.size());
            rendered[i] = true;
            const auto dc = offsetConversion(s, x, y);
            const double iteration = renderData->getPerturbator()->iterate(dc);

            renderer->iterationStagingBufferContext->set(x, y, iteration);

            auto my = static_cast<int16_t>(y + 1);
            while (my < yRes && !rendered[my * xRes + x]) {
                renderer->iterationStagingBufferContext->set(x, my, iteration);
                ++my;
            }

            ++renderPixelsCount;
            return iteration;
        };
        auto previewer = ParallelArrayDispatcher<double>(state, *iterationMatrix, s.render.threads, std::move(func));

        renderer->iterationStagingBufferContext->fillZero();

        auto statusThread = std::jthread([&renderPixelsCount, len, this, &start](const std::stop_token &stop) {
            static float time = Utilities::getCurrentTime();
            while (!stop.stop_requested()) {
                const float elapsed = Utilities::getCurrentTime() - time;
                if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                    time = Utilities::getCurrentTime();
                    float ratio = static_cast<float>(renderPixelsCount.load()) / static_cast<float>(len) * 100;
                    setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
                    setStatusMessage(Constants::Status::RENDER_STATUS, std::format("C : {:.3f}%", ratio));
                }
            }
        });

        previewer.dispatch();

        statusThread.request_stop();
        statusThread.join();

        if (state.interruptRequested())
            return false;

        const auto syncer = ParallelDispatcher(
                state, w, h, s.render.threads,
                [this](const uint16_t x, const uint16_t y, uint16_t, uint16_t, float, float, uint32_t) {
                    renderer->iterationStagingBufferContext->set(x, y, (*iterationMatrix)(x, y));
                });

        syncer.dispatch();

        if (state.interruptRequested())
            return false;
        setStatusMessage(Constants::Status::RENDER_STATUS, "Done");

        return true;
    }

    void RFFApplication::afterCompute(const bool success) {
        if (!success) {
            vkh::logger::log("Recompute cancelled.");
        }
        if (success && settings.fractal.reference.reuse == FrtReferenceReuseMethod::CENTERED_REFERENCE) {
            settings.fractal.reference.reuse = FrtReferenceReuseMethod::CURRENT_REFERENCE;
        }
        idleCompute = true;
        backgroundThreads.notifyAll();
    }


    void RFFApplication::onQuit() {
        state.cancel();
        renderer = nullptr;
    }
} // namespace merutilm::rff2
