//
// Created by Merutilm on 2025-08-08.
//

#include "RenderScene.hpp"

#include "CallbackExplore.hpp"
#include "IOUtilities.h"
#include "../../vulkan_helper/executor/RenderPassFullscreenRecorder.hpp"
#include "../../vulkan_helper/executor/ScopedCommandBufferExecutor.hpp"
#include "../constants/VulkanWindowConstants.hpp"
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
#include "../vulkan/RCC3.hpp"
#include "../vulkan/RCCDownsampleForBlur.hpp"
#include "../vulkan/RCC2.hpp"
#include "../vulkan/RCC4.hpp"
#include "../vulkan/RCCPresent.hpp"
#include "../vulkan/SharedDescriptorTemplate.hpp"
#include "../vulkan/SharedImageContextIndices.hpp"
#include "opencv2/opencv.hpp"


namespace merutilm::rff2 {
    RenderScene::RenderScene(vkh::EngineRef engine, const HWND window,
                             std::array<std::wstring, Constants::Status::LENGTH> *statusMessageRef) : EngineHandler(
            engine),
        window(window), attr(genDefaultAttr()), statusMessageRef(statusMessageRef) {
        init();
    }

    RenderScene::~RenderScene() {
        destroy();
    }

    void RenderScene::init() {
        refreshSharedImgContext();
        initRenderContext();
        initShaderPrograms();
        wndRequestFPS();
    }


    void RenderScene::initRenderContext() const {
        const auto swapchainImageContextGetter = [this] {
            auto &swapchain = *getTargetWindowContext().swapchain;
            return vkh::ImageContext::fromSwapchain(engine.getCore(), swapchain);
        };

        engine.attachRenderContext<RCC1>(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX,
                                         [this] { return getInternalImageExtent(); },
                                         swapchainImageContextGetter);
        engine.attachRenderContext<RCCDownsampleForBlur>(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX,
                                                         [this] { return getBlurredImageExtent(); },
                                                         swapchainImageContextGetter);
        engine.attachRenderContext<RCC2>(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX,
                                         [this] { return getInternalImageExtent(); },
                                         swapchainImageContextGetter);
        engine.attachRenderContext<RCC3>(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX,
                                         [this] { return getInternalImageExtent(); },
                                         swapchainImageContextGetter);
        engine.attachRenderContext<RCC4>(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX,
                                         [this] { return getInternalImageExtent(); },
                                         swapchainImageContextGetter);
        engine.attachRenderContext<RCCPresent>(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX,
                                               [this] { return getSwapchainRenderContextExtent(); },
                                               swapchainImageContextGetter);
    }

    void RenderScene::initShaderPrograms() {
        shaderPrograms = std::make_unique<RenderSceneShaderPrograms>(engine);

        for (const auto &sp: shaderPrograms->configurator) {
            sp->pipelineInitialized();
        }

        applyResizeParams();
        applyShaderAttr(attr);
        requests.requestRecompute();
    }

    void RenderScene::resolveWindowResizeEnd() const {
        if (getTargetWindowContext().window->isUnrenderable()) {
            return;
        }
        vkDeviceWaitIdle(engine.getCore().getLogicalDevice().getLogicalDeviceHandle());

        vkh::SwapchainRef swapchain = *getTargetWindowContext().swapchain;
        swapchain.recreate();
    }


    void RenderScene::render(const uint32_t frameIndex, const uint32_t swapchainImageIndex) {
        if (requests.defaultSettingsRequested) {
            applyDefaultSettings();
            requests.defaultSettingsRequested.exchange(false);
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


        draw(frameIndex, swapchainImageIndex);
    }

    void RenderScene::draw(const uint32_t frameIndex, const uint32_t swapchainImageIndex) const {
        vkh::DescriptorUpdateQueue queue = vkh::DescriptorUpdater::createQueue();
        const VkDevice device = engine.getCore().getLogicalDevice().getLogicalDeviceHandle();

        for (const auto &shaderProgram: shaderPrograms->configurator) {
            shaderProgram->updateQueue(queue, frameIndex);
        }

        vkh::DescriptorUpdater::write(device, queue);


        const VkFence renderFence = engine.getSyncObject().getFence(frameIndex).getFenceHandle();
        const VkSemaphore imageAvailableSemaphore = engine.getSyncObject().getSemaphore(frameIndex).
                getImageAvailable();
        const VkSemaphore renderFinishedSemaphore = engine.getSyncObject().getSemaphore(frameIndex).
                getRenderFinished();
        const VkCommandBuffer cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
        const auto mfg = [this, &frameIndex](const uint32_t index) {
            return getTargetWindowContext().sharedImageContext->getImageContextMF(index)[frameIndex].image;
        };

        vkh::ScopedCommandBufferExecutor executor(engine, frameIndex, renderFence, imageAvailableSemaphore,
                                                  renderFinishedSemaphore);
        vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC1>(
            engine, frameIndex, {
                shaderPrograms->rendererIteration, shaderPrograms->rendererStripe, shaderPrograms->rendererSlope,
                shaderPrograms->rendererColor
            }, {{}, {}, {}, {}});

        // [IN] EXTERNAL
        // [SUBPASS OUT] SECONDARY (iteration)
        // [SUBPASS IN] SECONDARY
        // [SUBPASS OUT] PRIMARY (stripe)
        // [SUBPASS IN] PRIMARY
        // [SUBPASS OUT] SECONDARY (slope)
        // [SUBPASS IN] SECONDARY
        // [OUT] PRIMARY (color)

        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                          mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                          0, 1,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        // [BARRIER] PRIMARY

        vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<
            RCCDownsampleForBlur>(
            engine, frameIndex, {shaderPrograms->rendererDownsampleForBlur}, {
                {GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_FOG}
            });

        // [IN] PRIMARY
        // [OUT] DOWNSAMPLED_PRIMARY

        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                          mfg(
                                                              SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                                                          VK_IMAGE_LAYOUT_GENERAL,
                                                          0, 1,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

        // [BARRIER] DOWNSAMPLED_PRIMARY

        shaderPrograms->rendererBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG);

        // [IN] DOWNSAMPLED_PRIMARY
        // [OUT] DOWNSAMPLED_SECONDARY

        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                          mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                          0, 1,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        vkh::BarrierUtils::cmdImageMemoryBarrier(
            cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
            VK_ACCESS_SHADER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        // [BARRIER] PRIMARY
        // [BARRIER] DOWNSAMPLED_SECONDARY

        vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC2>(
            engine, frameIndex, {shaderPrograms->rendererFog, shaderPrograms->rendererBloomThreshold}, {{}, {}});

        // [IN] PRIMARY
        // [IN] DOWNSAMPLED_SECONDARY
        // [PRESERVED SUBPASS OUT] SECONDARY
        // [SUBPASS IN] SECONDARY
        // [OUT] PRIMARY (Threshold Masked)

        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                          mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                          0, 1,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        // [BARRIER] PRIMARY

        vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCCDownsampleForBlur>(
            engine, frameIndex, {shaderPrograms->rendererDownsampleForBlur}, {
                {GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_BLOOM}
            });
        // [IN] PRIMARY
        // [OUT] DOWNSAMPLED_PRIMARY

        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                          mfg(
                                                              SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                                                          VK_IMAGE_LAYOUT_GENERAL,
                                                          0, 1,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
        // [BARRIER] DOWNSAMPLED_PRIMARY

        shaderPrograms->rendererBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM);

        // [IN] DOWNSAMPLED_PRIMARY
        // [OUT] DOWNSAMPLED_SECONDARY

        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                          mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                          0, 1,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        vkh::BarrierUtils::cmdImageMemoryBarrier(
            cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
            VK_ACCESS_SHADER_WRITE_BIT,
            VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        // [BARRIER] SECONDARY
        // [BARRIER] DOWNSAMPLED_SECONDARY


        vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC3>(
            engine, frameIndex, {shaderPrograms->rendererBloom}, {{}});

        // [IN] SECONDARY
        // [IN] DOWNSAMPLED_SECONDARY
        // [OUT] PRIMARY

        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                          mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                          0, 1,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC4>(
            engine, frameIndex, {shaderPrograms->rendererLinearInterpolation}, {{}});

        // [IN] PRIMARY
        // [OUT] SECONDARY

        vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                          mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                          0, 1,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

        // [BARRIER] SECONDARY

        vkh::RenderPassFullscreenRecorder::cmdFullscreenPresentOnlyRenderPass<RCCPresent>(
            engine, frameIndex, swapchainImageIndex, {shaderPrograms->rendererPresent}, {{}});

        // [IN] SECONDARY
        // [OUT] EXTERNAL
    }


    Attribute RenderScene::genDefaultAttr() {
        return Attribute{
            .calc = CalcAttribute{
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
                .decimalizeIterationMethod = CalDecimalizeIterationMethod::LOG_LOG,
                .mpaAttribute = CalculationPresets::UltraFast().genMPA(),
                .referenceCompAttribute = CalculationPresets::UltraFast().genReferenceCompression(),
                .reuseReferenceMethod = CalReuseReferenceMethod::DISABLED,
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
                .dataAttribute = {
                    .defaultZoomIncrement = 2,
                    .isStatic = false
                },
                .animationAttribute = {
                    .overZoom = 2,
                    .showText = true,
                    .mps = 1
                },
                .exportAttribute = {
                    .fps = 60,
                    .bitrate = 9000
                }
            }
        };
    }

    LRESULT RenderScene::renderSceneProc(const HWND hwnd, const UINT msg, const WPARAM wparam, const LPARAM lparam) {
        RenderScene &scene = *reinterpret_cast<RenderScene *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        scene.runAction(msg, wparam, lparam);
        return DefWindowProc(hwnd, msg, wparam, lparam);
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
                    const float logZoom = attr.calc.logZoom;
                    fp_complex &center = attr.calc.center;
                    center = center.addCenterDouble(dex::value(static_cast<float>(dx) / m) / getDivisor(attr),
                                                    dex::value(static_cast<float>(dy) / m) / getDivisor(attr),
                                                    Perturbator::logZoomToExp10(logZoom));
                    interactedMX = x;
                    interactedMY = y;
                    requests.requestRecompute();
                } else {
                    SetCursor(LoadCursor(nullptr, IDC_CROSS));
                    if (currentMap == nullptr) {
                        return;
                    }


                    if (auto it = static_cast<uint64_t>((*iterationMatrix)(x, y)); it != 0) {
                        setStatusMessage(Constants::Status::ITERATION_STATUS,
                                         std::format(L"I : {} ({}, {})", it, x, y));
                    }
                }
                break;
            }
            case WM_MOUSEWHEEL: {
                const int value = GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? 1 : -1;
                constexpr float increment = Constants::Render::ZOOM_INTERVAL;

                attr.calc.logZoom = std::max(Constants::Render::ZOOM_MIN,
                                             attr.calc.logZoom);
                if (value == 1) {
                    const std::array<dex, 2> offset = offsetConversion(attr, getMouseXOnIterationBuffer(),
                                                                       getMouseYOnIterationBuffer());
                    const double mzi = 1.0 / pow(10, Constants::Render::ZOOM_INTERVAL);
                    float &logZoom = attr.calc.logZoom;
                    logZoom += increment;
                    attr.calc.center = attr.calc.center.addCenterDouble(
                        offset[0] * (1 - mzi),
                        offset[1] * (1 - mzi),
                        Perturbator::logZoomToExp10(logZoom));
                }
                if (value == -1) {
                    const std::array<dex, 2> offset = offsetConversion(attr, getMouseXOnIterationBuffer(),
                                                                       getMouseYOnIterationBuffer());
                    const double mzo = 1.0 / pow(10, -Constants::Render::ZOOM_INTERVAL);
                    float &logZoom = attr.calc.logZoom;
                    logZoom -= increment;
                    attr.calc.center = attr.calc.center.addCenterDouble(
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
        return {
            dex::value(static_cast<double>(mx) - static_cast<double>(getIterationBufferWidth(settings)) / 2.0) /
            getDivisor(settings)
            / settings.render.clarityMultiplier,
            dex::value(static_cast<double>(my) - static_cast<double>(getIterationBufferHeight(settings)) / 2.0) /
            getDivisor(settings)
            / settings.render.clarityMultiplier
        };
    }

    dex RenderScene::getDivisor(const Attribute &settings) {
        dex v = dex::ZERO;
        dex_exp::exp10(&v, settings.calc.logZoom);
        return v;
    }

    uint16_t RenderScene::getClientWidth() const {
        RECT rect;
        GetClientRect(window, &rect);
        return static_cast<uint16_t>(rect.right - rect.left);
    }

    uint16_t RenderScene::getClientHeight() const {
        RECT rect;
        GetClientRect(window, &rect);
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

    void RenderScene::applyDefaultSettings() {
        vkDeviceWaitIdle(engine.getCore().getLogicalDevice().getLogicalDeviceHandle());
        attr = genDefaultAttr();
    }


    void RenderScene::applyCreateImage() {
        vkDeviceWaitIdle(engine.getCore().getLogicalDevice().getLogicalDeviceHandle());
        if (requests.createImageRequestedFilename.empty()) {
            requests.createImageRequestedFilename = IOUtilities::ioFileDialog(
                L"Save image", Constants::Extension::DESC_IMAGE,
                IOUtilities::SAVE_FILE,
                Constants::Extension::IMAGE)->string();
        }
        const auto &imgCtx = getTargetWindowContext().sharedImageContext->getImageContextMF(
            SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY)[0];

        vkh::BufferContext bufCtx = vkh::BufferContext::createContext(engine.getCore(), {
                                                                          .size = imgCtx.capacity,
                                                                          .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                                          .properties =
                                                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                                      });
        vkh::BufferContext::mapMemory(engine.getCore(), bufCtx);
        //NEW COMMAND BUFFER
        {
            const auto executor = vkh::ScopedNewCommandBufferExecutor(engine.getCore(), engine.getCommandPool());
            vkh::BarrierUtils::cmdImageMemoryBarrier(executor.getCommandBufferHandle(), imgCtx.image,
                                                     VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 0, 1,
                                                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                     VK_PIPELINE_STAGE_TRANSFER_BIT);
            vkh::BufferImageContextUtils::cmdCopyImageToBuffer(executor.getCommandBufferHandle(), imgCtx, bufCtx);
        }
        vkh::BufferContext::unmapMemory(engine.getCore(), bufCtx);
        auto img = cv::Mat(imgCtx.extent.height, imgCtx.extent.width, CV_16UC4, bufCtx.mappedMemory);
        cv::cvtColor(img, img, cv::COLOR_RGBA2BGRA);
        cv::imwrite(requests.createImageRequestedFilename, img);
        vkh::BufferContext::destroyContext(engine.getCore(), bufCtx);
    }

    void RenderScene::applyShaderAttr(const Attribute &attr) const {
        vkDeviceWaitIdle(engine.getCore().getLogicalDevice().getLogicalDeviceHandle());
        shaderPrograms->rendererIteration->setPalette(attr.shader.palette);
        shaderPrograms->rendererStripe->setStripe(attr.shader.stripe);
        shaderPrograms->rendererSlope->setSlope(attr.shader.slope);
        shaderPrograms->rendererColor->setColor(attr.shader.color);
        shaderPrograms->rendererFog->setFog(attr.shader.fog);
        shaderPrograms->rendererBloom->setBloom(attr.shader.bloom);
        shaderPrograms->rendererLinearInterpolation->setLinearInterpolation(attr.render.linearInterpolation);
        shaderPrograms->rendererBoxBlur->setBlurSize(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG, attr.shader.fog.radius);
        shaderPrograms->rendererBoxBlur->
                setBlurSize(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM, attr.shader.bloom.radius);
    }

    void RenderScene::applyResizeParams() {
        const uint16_t iw = getIterationBufferWidth(attr);
        const uint16_t ih = getIterationBufferHeight(attr);
        const auto &[dWidth, dHeight] = getBlurredImageExtent();
        const auto &[sWidth, sHeight] = getSwapchainRenderContextExtent();

        for (const auto &sp: shaderPrograms->configurator) {
            sp->windowResized();
        }
        shaderPrograms->rendererDownsampleForBlur->setRescaledResolution(0, {dWidth, dHeight});
        shaderPrograms->rendererDownsampleForBlur->setRescaledResolution(1, {dWidth, dHeight});
        shaderPrograms->rendererPresent->setRescaledResolution({sWidth, sHeight});

        shaderPrograms->rendererIteration->resetIterationBuffer(iw, ih);
        iterationMatrix = std::make_unique<Matrix<double> >(iw, ih);
    }

    void RenderScene::applyResize() {
        using namespace SharedImageContextIndices;
        vkDeviceWaitIdle(engine.getCore().getLogicalDevice().getLogicalDeviceHandle());

        refreshSharedImgContext();

        for (auto &context: engine.getRenderContexts()) {
            context->recreate();
        }
        applyResizeParams();
    }


    void RenderScene::refreshSharedImgContext() const {
        using namespace SharedImageContextIndices;
        auto &sharedImg = *getTargetWindowContext().sharedImageContext;
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
                                                         VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                                         VK_IMAGE_USAGE_SAMPLED_BIT));
        sharedImg.appendMultiframeImageContext(MF_MAIN_RENDER_IMAGE_SECONDARY,
                                               iiiGetter(internalImageExtent, VK_FORMAT_R16G16B16A16_UNORM,
                                                         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                         VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
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

    void RenderScene::overwriteMatrixFromMap() const {
        vkDeviceWaitIdle(engine.getCore().getLogicalDevice().getLogicalDeviceHandle());
        shaderPrograms->rendererIteration->setMaxIteration(static_cast<double>(currentMap->getMaxIteration()));
        shaderPrograms->rendererIteration->setAllIterations(currentMap->getMatrix().getCanvas());
    }

    uint16_t RenderScene::getMouseXOnIterationBuffer() const {
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(window, &cursor);
        const float multiplier = attr.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(cursor.x) * multiplier);
    }

    uint16_t RenderScene::getMouseYOnIterationBuffer() const {
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(window, &cursor);
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

    void RenderScene::beforeCompute(Attribute &settings) const {
        settings.calc.maxIteration = settings.calc.autoMaxIteration
                                         ? lastPeriod * settings.calc.
                                           autoIterationMultiplier
                                         : this->attr.calc.maxIteration;
        shaderPrograms->rendererIteration->setMaxIteration(static_cast<double>(settings.calc.maxIteration));
    }

    bool RenderScene::compute(const Attribute &settings) {
        auto start = std::chrono::high_resolution_clock::now();
        const uint16_t w = getIterationBufferWidth(settings);
        const uint16_t h = getIterationBufferHeight(settings);
        uint32_t len = uint32_t(w) * h;

        if (state.interruptRequested()) return false;

        auto &calc = settings.calc;

        const float logZoom = calc.logZoom;

        if (state.interruptRequested()) return false;

        setStatusMessage(Constants::Status::ZOOM_STATUS,
                         std::format(L"Z : {:.06f}E{:d}", pow(10, fmod(logZoom, 1)), static_cast<int>(logZoom)));

        const std::array<dex, 2> offset = offsetConversion(settings, 0, 0);
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
                using enum CalReuseReferenceMethod;
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

                CalcAttribute refCalc = calc;
                refCalc.center = center->perturbator->calc.center;
                refCalc.logZoom = center->perturbator->calc.logZoom;
                int refExp10 = Perturbator::logZoomToExp10(refCalc.logZoom);

                if (refCalc.logZoom > Constants::Render::ZOOM_DEADLINE) {
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
                if (logZoom > Constants::Render::ZOOM_DEADLINE) {
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
            state, *iterationMatrix,
            [settings, this, &renderPixelsCount, &rendered](const uint16_t x, const uint16_t y,
                                                            const uint16_t xRes, uint16_t, float,
                                                            float, const uint32_t i,
                                                            double) {
                rendered[i] = true;
                const auto dc = offsetConversion(settings, x, y);
                const double iteration = currentPerturbator->iterate(dc[0], dc[1]);
                shaderPrograms->rendererIteration->setIteration(x, y, iteration);

                auto my = static_cast<int16_t>(y - 1);
                while (my >= 0 && !rendered[my * xRes + x]) {
                    shaderPrograms->rendererIteration->setIteration(x, my, iteration);
                    --my;
                }

                ++renderPixelsCount;
                return iteration;
            });

        shaderPrograms->rendererIteration->resetIterationBuffer();

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
            state, w, h,
            [this](const uint16_t x, const uint16_t y, uint16_t, uint16_t, float, float, uint32_t) {
                shaderPrograms->rendererIteration->setIteration(x, y, (*iterationMatrix)(x, y));
            });

        syncer.dispatch();

        if (state.interruptRequested()) return false;

        currentMap = std::make_unique<RFFDynamicMapBinary>(calc.logZoom, lastPeriod, calc.maxIteration,
                                                           *iterationMatrix);
        setStatusMessage(Constants::Status::RENDER_STATUS, L"Done");

        return true;
    }

    void RenderScene::afterCompute(const bool success) {
        if (!success) {
            vkh::logger::log("Recompute cancelled.");
        }
        if (success && attr.calc.reuseReferenceMethod == CalReuseReferenceMethod::CENTERED_REFERENCE) {
            attr.calc.reuseReferenceMethod = CalReuseReferenceMethod::CURRENT_REFERENCE;
        }
        idleCompute = true;
        backgroundThreads.notifyAll();
    }


    void RenderScene::destroy() {
        state.cancel();
        shaderPrograms = nullptr;
    }
}
