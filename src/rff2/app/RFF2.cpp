//
// Created by Merutilm on 2025-08-08.
//

#include "RFF2.hpp"

#include <ranges>

#include "../io/RFFLocationBinary.h"
#include "../mb/MB2Locator.h"
#include "../parallel/ParallelArrayDispatcher.h"
#include "../preset/calc/CalculationPresets.h"
#include "../preset/render/RenderPresets.h"
#include "../preset/shader/bloom/ShdBloomPresets.h"
#include "../preset/shader/color/ShdColorPresets.h"
#include "../preset/shader/fog/ShdFogPresets.h"
#include "../preset/shader/palette/ShdPalettePresets.h"
#include "../preset/shader/slope/ShdSlopePresets.h"
#include "../preset/shader/stripe/ShdStripePresets.h"
#include "../vulkan/GPCDownsampleForBlur.hpp"
#include "../vulkan/SharedImageContextIndices.hpp"
#include "../vulkan/desc/SharedDescriptorTemplate.hpp"
#include "FnExplore.hpp"
#include "FnFile.hpp"
#include "FnFractal.hpp"
#include "FnPreset.hpp"
#include "FnRender.hpp"
#include "FnShader.hpp"
#include "FnVideo.hpp"
#include "IOUtilities.h"
#include "Utilities.h"
#include "imgui.h"
#include "nfd.hpp"
#include "opencv2/opencv.hpp"
#include "vulkan_helper/engine/executor/ScopedCommandBufferExecutor.hpp"
#include "vulkan_helper/engine/executor/ScopedNewCommandBufferExecutor.hpp"
#include "vulkan_helper/engine/window/PlatformWindow.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"
#include "vulkan_helper/util/BufferImageContextUtils.hpp"


namespace merutilm::rff2 {


    void RFF2::onStart() {
        initialize();
        applyShaderSettings(settings);
        refreshResizeParams(rootWindowContext->getSwapchain().getSwapchainExtent());
        checkBackupLoad();
        requests.requestRecompute();
    }

    void RFF2::initialize() {
        cursorManager = std::make_unique<CursorManager>(rootWindowContext->getWindow()->getWindow());
        computeShaderManager = std::make_unique<ComputeShaderRenderManager>(*rootWindowContext);
        NFD::Init();
        initImGui();
    }

    void RFF2::onResize(const VkExtent2D newExtent) {
        recreateContexts(newExtent);
        if (newExtent.width > 0 || newExtent.height > 0) {
            engine->getCore().getLogicalDevice().waitDeviceIdle();
            state.cancel();
            refreshResizeParams(newExtent);
            requests.requestRecompute();
            backgroundThreads.notifyAll();
        }
    }


    void RFF2::onQuit() {
        state.cancel();
        renderer = nullptr;
        NFD::Quit();
    }


    void RFF2::resolveRequests() {

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

        {
            std::scoped_lock lock2(requests.resizeMutex);
            if (requests.resizeRequested.exchange(false)) {
                onResize(requests.resizeRequestedExtent);
                backgroundThreads.notifyAll();
            }
        }
        {
            std::scoped_lock lock2(requests.createImageMutex);
            if (requests.createImageRequested.exchange(false)) {
                applyCreateImage();
                backgroundThreads.notifyAll();
            }
        }

        auto expected = ComputeState::REQUESTED;
        if (requests.recomputeRequestedState.compare_exchange_weak(expected, ComputeState::RUNNING)) {
            recomputeThreaded();
            // it is threaded, do not notify
        }
    }

    void RFF2::updateMouseInteraction() {
        double mdx;
        double mdy;
        glfwGetCursorPos(rootWindowContext->getWindow()->getWindow(), &mdx, &mdy);
        const int mx = static_cast<int>(mdx);
        const int my = static_cast<int>(mdy);
        const uint16_t x = getMouseXOnIterationBuffer(mx);
        const uint16_t y = getMouseYOnIterationBuffer(my);
        if (renderer->visibleIterationBufferContext == nullptr || x >= getIterationBufferWidth() ||
            y >= getIterationBufferHeight()) {
            return;
        }
        auto it = static_cast<uint64_t>((*renderer->visibleIterationBufferContext)(x, y));
        setStatusMessage(Constants::Status::ITERATION_STATUS,
                         std::format(std::locale("en_US.UTF-8"), "Iterations : {:L}", it, x, y));
    }

    void RFF2::update() {
        updateMouseInteraction();
        resolveRequests();
        invokeUpdaters();
        renderer->render();
    }


    Settings RFF2::genDefaultSettings() {
#ifndef NDEBUG
        return Settings{
                .fractal =
                        FractalSettings{.general = {.bailout = 2.00001f, .logZoom = 2, .threads = 15},
                                        .reference =
                                                {
                                                        .center = fixed_point_complex_i1(
                                                                "-0.85", "0", Perturbator::logZoomToExp10(2)),
                                                        .useParallelRefCalculation = false,
                                                        .sync = CalculationPresets::UltraFast().genRefSync(),
                                                        .compression = CalculationPresets::UltraFast().genRefComp(),
                                                        .reuse = false,
                                                },
                                        .sa = {.use = false,
                                               .appliedTermsCount = 8,
                                               .validatedTermsCount = 1,
                                               .epsilonPower = -5},
                                        .mpa = CalculationPresets::UltraFast().genMPA(),
                                        .perturb = {.maxIteration = 300,
                                                    .decimalizeIterationMethod = FrtDecimalizeIterationMethod::LOG_LOG,
                                                    .autoMaxIteration = true,
                                                    .interiorDetectRadiusPower = 7,
                                                    .autoIterationMultiplier = 100,
                                                    .absoluteIterationMode = false}},
                .render = RenderPresets::Low().genRender(),
                .shader = {.palette = ShdPalettePresets::LongRandom64().genPalette(),
                           .stripe = ShdStripePresets::Disabled().genStripe(),
                           .slope = ShdSlopePresets::Disabled().genSlope(),
                           .color = ShdColorPresets::Disabled().genColor(),
                           .fog = ShdFogPresets::Disabled().genFog(),
                           .bloom = BloomPresets::Disabled().genBloom(),
                           .noiseReduction = {true, 2, 0.1f},
                           .fractal3D = {false, 85, 0, 1, 0, 10.f}},
                .video = {.data = {.defaultZoomIncrement = 2, .isStatic = false},
                          .animation = {.overZoom = 2, .showText = true, .mps = 1},
                          .exportation = {.fps = 60, .bitrate = 9000}},
                .explore = {.autoMoveCursorToCenter = false}};
#else
        return Settings{
                .fractal =
                        FractalSettings{.general = {.bailout = 2.00001f,
                                                    .logZoom = 2,
                                                    .threads = std::thread::hardware_concurrency() - 1},
                                        .reference =
                                                {
                                                        .center = fixed_point_complex_i1(
                                                                "-0.85", "0", Perturbator::logZoomToExp10(2)),
                                                        .useParallelRefCalculation = false,
                                                        .sync = CalculationPresets::UltraFast().genRefSync(),
                                                        .compression = CalculationPresets::UltraFast().genRefComp(),
                                                        .reuse = false,
                                                },
                                        .sa = {.use = false,
                                               .appliedTermsCount = 8,
                                               .validatedTermsCount = 1,
                                               .epsilonPower = -5},
                                        .mpa = CalculationPresets::UltraFast().genMPA(),
                                        .perturb = {.maxIteration = 300,
                                                    .decimalizeIterationMethod = FrtDecimalizeIterationMethod::LOG_LOG,
                                                    .autoMaxIteration = true,
                                                    .interiorDetectRadiusPower = 7,
                                                    .autoIterationMultiplier = 100,
                                                    .absoluteIterationMode = false}},
                .render = RenderPresets::High().genRender(),
                .shader = {.palette = ShdPalettePresets::LongRandom64().genPalette(),
                           .stripe = ShdStripePresets::Disabled().genStripe(),
                           .slope = ShdSlopePresets::Disabled().genSlope(),
                           .color = ShdColorPresets::Disabled().genColor(),
                           .fog = ShdFogPresets::Disabled().genFog(),
                           .bloom = BloomPresets::Disabled().genBloom(),
                           .noiseReduction = {true, 2, 0.1f},
                           .fractal3D = {false, 85, 0, 1, 0, 10.f}},
                .video = {.data = {.defaultZoomIncrement = 2, .isStatic = false},
                          .animation = {.overZoom = 2, .showText = true, .mps = 1},
                          .exportation = {.fps = 60, .bitrate = 9000}},
                .explore = {.autoMoveCursorToCenter = false}};
#endif
    }

    complex<dex> RFF2::offsetConversion(const Settings &s, const int px, const int py) const {
        const double bufOffX = static_cast<double>(px) - static_cast<double>(getIterationBufferWidth()) / 2.0;
        const double bufOffY = static_cast<double>(py) - static_cast<double>(getIterationBufferHeight()) / 2.0;
        return complex(dex(bufOffX), dex(bufOffY)) / getDivisor(s) / dex(s.render.clarityMultiplier);
    }

    std::array<int, 2> RFF2::iterationBufferConversion(const Settings &s, const complex<dex> &offset) const {
        const auto [re, im] = static_cast<complex<double>>(offset * dex(s.render.clarityMultiplier) * getDivisor(s));

        const int px = static_cast<int>((re < 0 ? std::round(re) : std::ceil(re)) + getIterationBufferWidth() / 2.0);
        const int py = static_cast<int>((im < 0 ? std::round(im) : std::ceil(im)) + getIterationBufferHeight() / 2.0);
        return {px, py};
    }

    void RFF2::moveCursor(const int px, const int py) const {
        const int mx = static_cast<int>(static_cast<float>(px) / settings.render.clarityMultiplier);
        const int my = static_cast<int>(static_cast<float>(py) / settings.render.clarityMultiplier);
        glfwSetCursorPos(rootWindowContext->getWindow()->getWindow(), mx,
                         rootWindowContext->getSwapchain().getSwapchainExtent().height - my);
    }

    dex RFF2::getDivisor(const Settings &settings) { return rff_math::exp10(settings.fractal.general.logZoom); }


    uint16_t RFF2::calcIterationBufferWidth(const Settings &s) const {
        const float multiplier = s.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(rootWindowContext->getSwapchain().getSwapchainExtent().width) *
                                     multiplier);
    }

    uint16_t RFF2::calcIterationBufferHeight(const Settings &s) const {
        const float multiplier = s.render.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(rootWindowContext->getSwapchain().getSwapchainExtent().height) *
                                     multiplier);
    }

    uint16_t RFF2::getIterationBufferWidth() const { return renderer->visibleIterationBufferContext->getWidth(); }

    uint16_t RFF2::getIterationBufferHeight() const { return renderer->visibleIterationBufferContext->getHeight(); }


    void RFF2::addListeners() {
        auto &eventSystem = rootWindowContext->getWindow()->eventSystem;

        eventSystem.applicationLifecycle.onUpdate.add([this] { update(); });

        eventSystem.resize.onResize.add([this](const int w, const int h) {
            const auto extent = VkExtent2D(w, h);
            requests.requestResize(extent);
        });

        eventSystem.applicationLifecycle.onStart.add([this] { onStart(); });

        eventSystem.applicationLifecycle.onQuit.add([this] {
            rootWindowContext->core.getLogicalDevice().waitDeviceIdle();
            onQuit();
        });


        eventSystem.mouse.onMouseEnter.add(
                [this] { glfwSetCursor(cursorManager->window, cursorManager->crosshairCursor); });
        eventSystem.mouse.onMouseExit.add([this] { glfwSetCursor(cursorManager->window, nullptr); });

        eventSystem.mouseDrag.onMouseDrag.add(
                [this](const int mb, const int mx, const int my, const int mdx, const int mdy) {
                    const int16_t x = getMouseXOnIterationBuffer(mx);
                    const int16_t y = getMouseYOnIterationBuffer(my);
                    const auto dx = static_cast<int16_t>(getMouseXOnIterationBuffer(mx - mdx) - x);
                    const auto dy = static_cast<int16_t>(getMouseYOnIterationBuffer(my - mdy) - y);
                    const auto dxr = -static_cast<float>(dx) / static_cast<float>(getIterationBufferWidth());
                    const auto dyr = static_cast<float>(dy) / static_cast<float>(getIterationBufferHeight());
                    const auto dz = pow(10.0f, -zoomAnimationInfo.targetLogZoomOffsetAim);

                    zoomAnimationInfo.aimChanged = true;
                    zoomAnimationInfo.targetMouseDragOffset += glm::vec2{dxr * dz, dyr * dz};

                    if (mb == GLFW_MOUSE_BUTTON_LEFT) {
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
            settings.fractal.general.logZoom = std::max(Constants::Fractal::ZOOM_MIN, settings.fractal.general.logZoom);
            double mdx;
            double mdy;
            glfwGetCursorPos(rootWindowContext->getWindow()->getWindow(), &mdx, &mdy);
            const int mx = static_cast<int>(mdx);
            const int my = static_cast<int>(mdy);
            const int16_t mix = getMouseXOnIterationBuffer(mx);
            const int16_t miy = getMouseYOnIterationBuffer(my);
            zoom(mix, miy, value > 0 ? Constants::Fractal::ZOOM_INTERVAL : -Constants::Fractal::ZOOM_INTERVAL);
        });
    }


    void RFF2::zoom(const int16_t px, const int16_t py, const float logIncrement) {

        settings.fractal.general.logZoom = std::max(Constants::Fractal::ZOOM_MIN, settings.fractal.general.logZoom);
        const int16_t mix = px;
        const int16_t miy = py;
        const auto mxr = static_cast<float>(mix) / static_cast<float>(getIterationBufferWidth()) - 0.5f;
        const auto myr = static_cast<float>(miy) / static_cast<float>(getIterationBufferHeight()) - 0.5f;
        const auto dz = pow(10.0f, -zoomAnimationInfo.targetLogZoomOffsetAim);

        const auto [re, im] = offsetConversion(settings, mix, miy);
        float &logZoom = settings.fractal.general.logZoom;
        fixed_point_complex_i1 &center = settings.fractal.reference.center;
        const int exp10 = Perturbator::logZoomToExp10(logZoom);
        center.set_exp10(exp10);

        const float mz = pow(10.0f, -logIncrement);
        logZoom += logIncrement;
        const fixed_point_complex_i1 add(re * dex(1 - mz), im * dex(1 - mz), exp10);
        fixed_point_complex_i1::add(center, center, add);

        zoomAnimationInfo.aimChanged = true;
        zoomAnimationInfo.stop();
        zoomAnimationInfo.targetLogZoomOffsetAim += logIncrement;
        zoomAnimationInfo.targetMouseZoomOffsetAim += glm::vec2{mxr * dz * (mz - 1), myr * dz * (1 - mz)};
        requests.requestRecompute();
    }


    void RFF2::applyDefaultSettings() {
        rootWindowContext->core.getLogicalDevice().waitDeviceIdle();
        settings = genDefaultSettings();
    }


    void RFF2::applyCreateImage() {
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

        vkh::BufferContext bufCtx = vkh::BufferContext::createContext(
                rootWindowContext->core,
                {
                        .size = imgCtx.capacity,
                        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                        .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                });
        vkh::BufferContext::mapMemory(rootWindowContext->core, bufCtx);
        // NEW COMMAND BUFFER
        {
            const auto executor =
                    vkh::ScopedNewCommandBufferExecutor(rootWindowContext->core, rootWindowContext->getCommandPool());
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                    executor.getCommandBuffer().getCommandBufferHandle(), imgCtx.image, VK_ACCESS_SHADER_WRITE_BIT,
                    VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 0, 1, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
            vkh::BufferImageContextUtils::cmdCopyImageToBuffer(executor.getCommandBuffer(), imgCtx, bufCtx);
        }
        vkh::BufferContext::unmapMemory(rootWindowContext->core, bufCtx);

        auto img = cv::Mat(static_cast<int>(imgCtx.extent.height), static_cast<int>(imgCtx.extent.width), CV_16UC4,
                           bufCtx.mappedMemory);
        cv::cvtColor(img, img, cv::COLOR_RGBA2BGRA);
        cv::imwrite(requests.createImageRequestedFilename, img);
        vkh::BufferContext::destroyContext(rootWindowContext->core, bufCtx);
    }

    void RFF2::invokeUpdaters() {
        static float time = rootWindowContext->getWindow()->getTime();
        const float t = rootWindowContext->getWindow()->getTime();
        const float dt = t - time;
        time = t;

        if (canShowPreview && !zoomAnimationInfo.aimChanged) {
            renderer->updateStagingBuffer |= renderer->visibleIterationBufferContext->fill();
            renderer->descriptorStorage->iteration->applyMaxIteration();
            zoomAnimationInfo.reset();
        }

        zoomAnimationInfo.update(dt);

        renderer->descriptorStorage->smoothZoom->set(zoomAnimationInfo.targetMouseDragOffset +
                                                             zoomAnimationInfo.targetMouseZoomOffset,
                                                     zoomAnimationInfo.targetLogZoomOffset);
    }

    void RFF2::applyShaderSettings(const Settings &s) const {
        using namespace SharedDescriptorTemplate;
        vkh::Core &core = rootWindowContext->core;
        core.getLogicalDevice().waitDeviceIdle();

        renderer->descriptorStorage->palette->set(s.shader.palette);
        renderer->descriptorStorage->stripe->set(s.shader.stripe);
        renderer->descriptorStorage->color->set(s.shader.color);
        renderer->descriptorStorage->fog->set(s.shader.fog);
        renderer->descriptorStorage->bloom->set(s.shader.bloom);
        renderer->descriptorStorage->noiseReduction->set(s.shader.noiseReduction);
        renderer->descriptorStorage->camera3d->set(s.shader.fractal3D);
        renderer->descriptorStorage->fractal3d->set(s.shader.fractal3D);
    }

    void RFF2::refreshResizeParams(const VkExtent2D swapchainExtent) const {
        const uint16_t iw = calcIterationBufferWidth(settings);
        const uint16_t ih = calcIterationBufferHeight(settings);
        const auto &[dWidth, dHeight] =
                RendererUtils::getBlurredImageExtent(swapchainExtent, settings.render.clarityMultiplier);
        const auto &[sWidth, sHeight] = rootWindowContext->getSwapchain().getSwapchainExtent();

        // shared
        renderer->descriptorStorage->iteration->resetIterationBuffer(iw, ih);
        renderer->descriptorStorage->batchResult->resizeBatchResultBuffer(iw, ih);
        renderer->descriptorStorage->renderMeta->resizeWriteBuffer(iw, ih);
        renderer->descriptorStorage->renderMetaIterationVariant->resetIterationBuffer(iw, ih);

        // unique
        renderer->rccDownsample->downsample->setRescaledResolution(GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_FOG,
                                                                   {dWidth, dHeight});
        renderer->rccDownsample->downsample->setRescaledResolution(
                GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_BLOOM, {dWidth, dHeight});
        renderer->rccPresentPrepare->smoothZoom->setRescaledResolution({sWidth, sHeight});


        renderer->rg1->fractal3d->resetPiplineVI(iw, ih);
        renderer->computeIterate->setExtent({iw, ih});
        renderer->computeIgnoreIsolated->setExtent({iw, ih});

        renderer->visibleIterationBufferContext = std::make_unique<GraphicsMatrixBuffer<double>>(
                rootWindowContext->core, iw, ih, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                        VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
        renderer->updateStagingBuffer = true;
    }

    void RFF2::registerRenderers() {
        renderer = registerRenderer<RFF2Renderer>(*engine, *rootWindowContext, settings, zoomAnimationInfo,
                                                  [this] { renderImGui(); });
        createImGuiContext(renderer->imguiRenderContext);
    }

    void RFF2::initImGui() {

        const ImGuiIO &io = ImGui::GetIO();
        const std::filesystem::path path =
                vkh::ExecutableUtils::getExecutableDirectory() / ".." / "res" / "IBMPlexSansKR-Medium.ttf";
        io.Fonts->AddFontFromFileTTF(path.string().data(), 20.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

        ImGuiStyle &style = ImGui::GetStyle();

        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.11f, .8f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.07f, 0.08f, .8f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.14f, 0.18f, .9f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.28f, .8f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.24f, 0.24f, 0.24f, .9f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.34f, 0.34f, 0.34f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.18f, 0.32f, 0.56f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.50f, 0.95f, 1.0f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.34f, 0.60f, 1.00f, 1.0f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.24f, 0.50f, 0.95f, 1.0f);

        style.Colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.20f, 1.0f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.50f, 0.95f, 1.0f);
        style.Colors[ImGuiCol_TabSelected] = ImVec4(0.24f, 0.50f, 0.95f, 0.85f);
        style.Colors[ImGuiCol_TabDimmed] = ImVec4(0.16f, 0.16f, 0.17f, 1.0f);
        style.Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.20f, 0.20f, 0.22f, 1.0f);

        style.WindowRounding = 8.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 6.0f;
        style.GrabRounding = 6.0f;
        style.PopupRounding = 8.0f;
        style.TabRounding = 6.0f;
        style.ScrollbarRounding = 8.0f;

        style.FrameBorderSize = 0.0f;
        style.WindowBorderSize = 0.0f;
        style.ChildBorderSize = 0.0f;
    }


    void RFF2::renderImGui() {

        renderControlImGui();
        renderStatusImGui();
    }

    void RFF2::renderControlImGui() {
        ImGui::Begin("Control");
        if (ImGui::BeginTabBar("Control")) {
            if (ImGui::BeginTabItem("File")) {
                FnFile::saveMap(*this);
                FnFile::saveImage(*this);
                FnFile::saveLocation(*this);
                FnFile::loadMap(*this);
                FnFile::loadLocation(*this);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Fractal")) {
                FnFractal::reference(*this);
                FnFractal::iterations(*this);
                FnFractal::sa(*this);
                FnFractal::mpa(*this);
                FnFractal::automaticIterations(*this);
                FnFractal::absoluteIterationMode(*this);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Render")) {
                FnRender::setResolutionProperties(*this);
                FnRender::setRenderProperties(*this);
                FnRender::setComputeShader(*this);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Presets")) {
                FnPreset::calculation(*this);
                FnPreset::render(*this);
                FnPreset::resolution(*this);
                FnPreset::shader(*this);

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shader")) {
                FnShader::palette(*this);
                FnShader::stripe(*this);
                FnShader::slope(*this);
                FnShader::color(*this);
                FnShader::fog(*this);
                FnShader::bloom(*this);
                FnShader::noiseReduction(*this);
                FnShader::fractal3D(*this);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Video")) {
                FnVideo::dataSettings(*this);
                FnVideo::animationSettings(*this);
                FnVideo::exportSettings(*this);
                FnVideo::generateVidKeyframes(*this);
                FnVideo::exportZoomVideo(*this);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Explore")) {
                FnExplore::recompute(*this);
                FnExplore::reset(*this);
                FnExplore::cancelRender(*this);
                FnExplore::moveCursorToCenter(*this);
                FnExplore::reuseReference(*this);
                FnExplore::moveToCenter(*this);
                FnExplore::goToOriginalReference(*this);
                FnExplore::locateCenteredReference(*this);
                FnExplore::locateMinibrot(*this);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    void RFF2::renderStatusImGui() const {

        const float height = ImGui::GetTextLineHeight() + ImGui::GetStyle().WindowPadding.y * 2;
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - height));

        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, height));

        ImGui::Begin("StatusBar", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
        if (ImGui::BeginTable("StatusBarTable", static_cast<int>(statusMessages.size()),
                              ImGuiTableFlags_BordersInner)) {
            for (const auto &statusMessage: statusMessages) {
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(statusMessage.c_str());
            }
            ImGui::EndTable();
        }

        ImGui::PopStyleVar();
        ImGui::End();
    }

    void RFF2::refreshSharedImgContexts(const VkExtent2D extent) {
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

        const auto internalImageExtent =
                RendererUtils::getInternalImageExtent(extent, settings.render.clarityMultiplier);
        const auto blurredImageExtent = RendererUtils::getBlurredImageExtent(extent, settings.render.clarityMultiplier);

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
        sharedImg.appendMultiframeImageContext(
                MF_MAIN_RENDER_IMAGE_DEPTH,
                iiiGetter(internalImageExtent, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
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

    void RFF2::overwriteMatrixFromMap(const RFFDynamicMapBinary &map) const {
        rootWindowContext->core.getLogicalDevice().waitDeviceIdle();
        const uint32_t iw = getIterationBufferWidth();
        const uint32_t ih = getIterationBufferHeight();
        if (iw != map.width || ih != map.height) {
            vkh::logger::log_err("Map size mismatch, {}x{} required but provided {}x{}", iw, ih, map.width, map.height);
            return;
        }

        renderer->descriptorStorage->iteration->setMaxIteration(static_cast<double>(map.maxIteration));
        renderer->descriptorStorage->iteration->applyMaxIteration();
        renderer->visibleIterationBufferContext->fill(map.iterations);
        renderer->updateStagingBuffer = true;
    }

    std::filesystem::path RFF2::getBackupLocationPath() {
        return vkh::ExecutableUtils::getExecutableDirectory() /
               std::format("{0}.{1}", Constants::File::BACKUP_FILE_NAME, Constants::File::EXT_LOCATION);
    }

    void RFF2::saveBackup() const {
        const auto path = getBackupLocationPath();
        saveCurrentLocation(path);
    }

    void RFF2::saveCurrentLocation(const std::filesystem::path &path) const {
        auto frt = settings.fractal; // clone the settings
        auto &center = frt.reference.center;
        RFFLocationBinary(frt.general.logZoom, center.real.to_string(), center.imag.to_string(),
                          frt.perturb.maxIteration)
                .exportFile(path);
    }

    void RFF2::loadLocation(const std::filesystem::path &path) {
        const RFFLocationBinary location = RFFLocationBinary::read(path);

        settings.fractal.reference.center = fixed_point_complex_i1(location.getReal(), location.getImag(),
                                                                   Perturbator::logZoomToExp10(location.getLogZoom()));
        settings.fractal.general.logZoom = location.getLogZoom();
        settings.fractal.perturb.maxIteration = location.getMaxIteration();
        requests.requestRecompute();
    }

    int16_t RFF2::getMouseXOnIterationBuffer(const int mx) const {
        const float multiplier = settings.render.clarityMultiplier;
        return static_cast<int16_t>(static_cast<float>(mx) * multiplier);
    }

    int16_t RFF2::getMouseYOnIterationBuffer(const int my) const {
        const float multiplier = settings.render.clarityMultiplier;
        return static_cast<int16_t>(static_cast<float>(getIterationBufferHeight()) -
                                    static_cast<float>(my) * multiplier);
    }

    void RFF2::checkBackupLoad() {
        const auto path = getBackupLocationPath();
        if (std::filesystem::exists(path) &&
            vkh::logger::messagebox_yn("Info", "Last rendered location has been found. Do you want to load it?")) {
            if (!std::filesystem::exists(path))
                return; // user deleted file manually
            loadLocation(path);
        }
    }

    void RFF2::recomputeThreaded() {

        state.createThread([this] {
            Settings s = this->settings; // clone the settings
            const auto start = rootWindowContext->getWindow()->getTime();
            bool success = false;

            try {
                success = prepareRenderData(start, s);

                if (success) {
                    beforeIterationFill();
                    success = fillIteration(start, s);
                }
            } catch (allocation_cancelled &) {
                vkh::logger::log("Memory allocation cancelled by user");
            }

            afterComputeFinally(success);
        });
    }

    void RFF2::moveCursorToCenter() const {
        const std::unique_ptr<fixed_point_complex_i1> off = MB2Locator::findCenterOffset(*renderData);
        auto offDex = static_cast<complex<dex>>(*off);
        if (renderData->getPerturbator()) {
            offDex -= renderData->getPerturbator()->off;
        }
        const auto [width, height] = rootWindowContext->getSwapchain().getSwapchainExtent();

        // multiplying 1.01 to attract reference center to client center
        const std::array<int, 2> ib = iterationBufferConversion(settings, offDex * dex(1.01));

        if (ib[0] >= 0 && ib[1] >= 0 && ib[0] < width && ib[1] < height) {
            moveCursor(ib[0], ib[1]);
        }
    }

    void RFF2::beforeIterationFill() const {
        if (settings.explore.autoMoveCursorToCenter) {
            moveCursorToCenter();
        }
        renderer->descriptorStorage->iteration->setMaxIteration(
                static_cast<double>(renderData->fractalSettings.perturb.maxIteration));

        saveBackup();
    }

    bool RFF2::prepareRenderData(const float startTime, Settings &s) {


        canShowPreview = false;

        if (state.interruptRequested())
            return false;

        auto &frt = s.fractal;
        const float logZoom = frt.general.logZoom;

        setStatusMessage(Constants::Status::ZOOM_STATUS,
                         std::format("Zoom : {:.06f}E{:d}", pow(10, fmod(logZoom, 1)), static_cast<int>(logZoom)));

        const complex<dex> offset = offsetConversion(s, 0, 0);
        const dex dcMax = offset.norm_approx();

        static uint64_t capacity = 0;
        if (renderData && renderData->getReference()) {
            capacity = renderData->getReference()->length();
        }

        std::function actionPerRefCalcIteration = FnExplore::getActionWhileRefCalc(*this, startTime);
        std::function actionPerSeriesApproxIteration = FnExplore::getActionWhileSeriesApprox(*this, startTime);
        std::function actionPerCreatingTableIteration = FnExplore::getActionWhileCreatingTable(*this, startTime);


        if (state.interruptRequested())
            return false;


        const int exp10 = Perturbator::logZoomToExp10(logZoom);
        if (frt.reference.reuse) {
            if (!renderData || !renderData->getReference() || !renderData->getPerturbator()) {
                vkh::logger::log_err("Do not reuse Reference during reference calculation!!!");
                this->settings.fractal.reference.reuse = false;
                requests.requestRecompute();
                return false;
            }

            fixed_point_complex_i1 center = frt.reference.center.create_variant(exp10);
            const fixed_point_complex_i1 referenceCenter = renderData->getReference()->center.create_variant(exp10);
            fixed_point_complex::sub(center, center, referenceCenter);
            const dex distance = static_cast<complex<dex>>(center).norm_approx();


            renderData->translate(frt.general.logZoom, dcMax + distance, frt.perturb, frt.reference.center,
                                  actionPerSeriesApproxIteration);
        } else {
            renderData = nullptr;

            if (s.render.computeShader.use) {
                s.fractal.reference.sync.referenceSynchronizationInterval = 1;
                if (logZoom > Constants::Fractal::COMPUTESHADER_ZOOM_DEADLINE) {
                    renderData = std::make_unique<FexMB2RenderData>(
                            state, frt, approxTableCache, dcMax, exp10, capacity, 0, actionPerRefCalcIteration,
                            actionPerSeriesApproxIteration, actionPerCreatingTableIteration);
                } else {
                    renderData = std::make_unique<NormalMB2RenderData>(
                            state, frt, approxTableCache, dcMax, exp10, capacity, 0, actionPerRefCalcIteration,
                            actionPerSeriesApproxIteration, actionPerCreatingTableIteration);
                }
            }else {
                if (logZoom > Constants::Fractal::MULTITHREAD_ZOOM_DEADLINE) {
                    renderData = std::make_unique<DexMB2RenderData>(
                            state, frt, approxTableCache, dcMax, exp10, capacity, 0, actionPerRefCalcIteration,
                            actionPerSeriesApproxIteration, actionPerCreatingTableIteration);
                } else {
                    renderData = std::make_unique<DoubleMB2RenderData>(
                            state, frt, approxTableCache, dcMax, exp10, capacity, 0, actionPerRefCalcIteration,
                            actionPerSeriesApproxIteration, actionPerCreatingTableIteration);
                }
            }
        }

        const MB2ReferenceBase *reference = renderData->getReference();
        if (!reference || state.interruptRequested())
            return false;

        size_t refLength = reference->length();
        size_t mpaLen = approxTableCache ? approxTableCache->tableSizeUsed : 0;

        setStatusMessage(Constants::Status::PERIOD_STATUS,
                         std::format("Period : {:L} ({:L}, {:L})", reference->longestPeriod(), refLength, mpaLen));
        if (state.interruptRequested())
            return false;

        return true;
    }

    void RFF2::fillIterationComputeShader(const MB2RenderDataBase *renderDataBase, const float startTime,
                                          const Settings &s) {
        setStatusMessage(Constants::Status::RENDER_STATUS, "Computing...");
        const auto cache = dynamic_cast<ApproxTableCache<float> *>(approxTableCache.get());

#ifndef NDEBUG
        const auto tableData = cache ? cache->mpaTable.data() : nullptr;
        const auto mapperData = cache ? cache->flattenIndexMapper.data() : nullptr;
#else
        const auto tableData = cache ? cache->mpaTable : nullptr;
        const auto mapperData = cache ? cache->flattenIndexMapper : nullptr;
#endif

        const uint32_t width = getIterationBufferWidth();
        const uint32_t height = getIterationBufferHeight();
        const VkExtent2D extent{width, height};

        vkh::CommandPool &commandPool = *computeShaderManager->commandPool;

        setStatusMessage(Constants::Status::RENDER_STATUS, "Preparing Render Meta...");

        const auto tableLen = cache ? cache->tableSizeUsed : 0;
        const auto mapperLen = cache ? cache->mapperSizeUsed : 0;

        renderer->computeIterate->setMPAIgnore(s.render.computeShader.completelyIgnoreMpa);

        renderer->descriptorStorage->renderMeta->setBatchSize(Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE);
        renderer->descriptorStorage->renderMeta->clearWriteBuffer(commandPool);
        renderer->descriptorStorage->renderMeta->set(
                renderData->fractalSettings, s.render, dynamic_cast<MB2Reference<float> *>(renderDataBase->getReference())->refOrbit,
                static_cast<complex<float>>(renderData->getPerturbator()->off),
                static_cast<uint32_t>(renderData->fractalSettings.perturb.maxIteration), tableData, tableLen,
                mapperData, mapperLen, commandPool);

        // preparing render meta scope


        if (state.interruptRequested()) {
            return;
        }

        const vkh::BufferContext &iterResultCtx =
                renderer->descriptorStorage->renderMetaIterationVariant->getResultIterationBuffer();
        const vkh::BufferContext &batchResultCtx = renderer->descriptorStorage->batchResult->getBatchResultBuffer();
        computeShaderManager->tryCreateOrResizeTransferDstBuffer(batchResultCtx, iterResultCtx, extent);

        const vkh::BufferContext &dstBatchBuffer = computeShaderManager->dstBatchBuffer;
        const vkh::BufferContext &dstIterBuffer = computeShaderManager->dstIterBuffer;


        std::vector<uint32_t> stagingData(width * height);
        uint64_t glitches = width * height;
        uint64_t currentBatchIteration = 0;
        uint32_t batchSizeMultiplier = 1;
        bool prevIgnoreMpa = s.render.computeShader.completelyIgnoreMpa;

        for (uint32_t i = 0; glitches > s.render.computeShader.allowedGlitchPixelCount; ++i) {

            if (state.interruptRequested()) {
                break;
            }


            computeShaderManager->fence->waitAndReset();

            const auto actualTime = std::chrono::high_resolution_clock::now();

            {
                vkh::CommandBuffer &commandBuffer = *computeShaderManager->commandBuffer;
                vkh::Fence &fence = *computeShaderManager->fence;
                const VkCommandBuffer cbh = commandBuffer.getCommandBufferHandle();

                const vkh::ScopedCommandBufferExecutor executor(*rootWindowContext, commandBuffer, fence,
                                                                VK_NULL_HANDLE, VK_NULL_HANDLE);

                renderer->computeIterate->cmdRender(cbh, 0, {});


                if (s.render.computeShader.interpolateIsolated) {
                    {
                        vkh::ScopedPipelineBarrierRecorder spr(cbh);
                        spr.cmdBufferMemoryBarrier(
                                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                                iterResultCtx.buffer, 0, iterResultCtx.bufferSize, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
                        spr.cmdBufferMemoryBarrier(
                                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
                                batchResultCtx.buffer, 0, batchResultCtx.bufferSize,
                                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
                    }

                    renderer->computeIgnoreIsolated->cmdRender(cbh, 0, {});
                }
                {
                    vkh::ScopedPipelineBarrierRecorder spr(cbh);
                    spr.cmdBufferMemoryBarrier(VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                                               iterResultCtx.buffer, 0, iterResultCtx.bufferSize,
                                               VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

                    spr.cmdBufferMemoryBarrier(VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
                                               batchResultCtx.buffer, 0, batchResultCtx.bufferSize,
                                               VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
                }
                vkh::BufferImageContextUtils::cmdCopyBuffer(commandBuffer, iterResultCtx, dstIterBuffer);
                vkh::BufferImageContextUtils::cmdCopyBuffer(commandBuffer, batchResultCtx, dstBatchBuffer);
                {
                    vkh::ScopedPipelineBarrierRecorder spr(cbh);
                    spr.cmdBufferMemoryBarrier(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_HOST_READ_BIT,
                                               dstBatchBuffer.buffer, 0, dstBatchBuffer.bufferSize,
                                               VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT);
                    spr.cmdBufferMemoryBarrier(VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_HOST_READ_BIT,
                                               dstIterBuffer.buffer, 0, dstIterBuffer.bufferSize,
                                               VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT);
                }
            }

            computeShaderManager->fence->wait();
            const auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(
                    std::chrono::high_resolution_clock::now() - actualTime);

            const float time = rootWindowContext->getWindow()->getTime();
            setStatusMessage(Constants::Status::TIME_STATUS,
                             std::format("Time : {}", Utilities::formatTime(time - startTime)));
            setStatusMessage(Constants::Status::RENDER_STATUS,
                             std::format("Batching... ({:L}, {:L}, {:L}ms)", currentBatchIteration, glitches, static_cast<uint32_t>(elapsed.count() * 1000)));


            currentBatchIteration += Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE * batchSizeMultiplier;

            if (elapsed.count() < settings.render.computeShader.preferredBatchDuration) {
                batchSizeMultiplier *= 2;
                renderer->descriptorStorage->renderMeta->setBatchSize(
                        Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE * batchSizeMultiplier);
            }
            if (elapsed.count() > settings.render.computeShader.preferredBatchDuration * 2) {
                batchSizeMultiplier = std::max(static_cast<uint32_t>(1), batchSizeMultiplier / 2);
                renderer->descriptorStorage->renderMeta->setBatchSize(
                        Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE * batchSizeMultiplier);
            }
            bool currIgnoreMpa = settings.render.computeShader.completelyIgnoreMpa ||
                                 (i > settings.render.computeShader.automaticAcceptMpaBatches &&
                                  settings.render.computeShader.automaticAcceptMpaBatches != 0);
            if (prevIgnoreMpa != currIgnoreMpa) {
                batchSizeMultiplier = 1;
                renderer->computeIterate->setMPAIgnore(currIgnoreMpa);
                renderer->descriptorStorage->renderMeta->setBatchSize(
                        Constants::Render::COMPUTE_SHADER_INIT_BATCH_SIZE);
                prevIgnoreMpa = currIgnoreMpa;
            }


            memcpy(stagingData.data(), dstBatchBuffer.mappedMemory, dstBatchBuffer.bufferSize);
            memcpy(renderer->visibleIterationBufferContext->getData().data(), dstIterBuffer.mappedMemory,
                   dstIterBuffer.bufferSize);

            glitches = std::ranges::count_if(stagingData, [](const uint32_t data) { return data != 1; });

            renderer->visibleIterationBufferContext->markUpdate();
            canShowPreview = true;
        } // batching and checking scope
    }

    void RFF2::fillIterationMultithreaded(const float startTime, const Settings &s) {
        std::atomic renderPixelsCount = 0;
        const uint16_t w = getIterationBufferWidth();
        const uint16_t h = getIterationBufferHeight();

        static std::vector<double> actualIterationMatrix(0);
        if (actualIterationMatrix.size() != w * h)
            actualIterationMatrix.resize(w * h);


        uint32_t len = static_cast<uint32_t>(w) * h;

        auto rendered = std::vector<uint8_t>(len);

        auto func = [&s, this, &renderPixelsCount, &rendered](const uint16_t x, const uint16_t y, const uint16_t xRes,
                                                              const uint16_t yRes, float, float, const uint32_t i,
                                                              double) {
            assert(i < rendered.size());
            rendered[i] = true;
            const auto dc = offsetConversion(s, x, y);
            const double iteration = renderData->getPerturbator()->iterate(dc);

            renderer->visibleIterationBufferContext->set(x, y, iteration);

            auto my = static_cast<int16_t>(y + 1);
            while (my < yRes && !rendered[my * xRes + x]) {
                renderer->visibleIterationBufferContext->set(x, my, iteration);
                ++my;
            }

            ++renderPixelsCount;
            return iteration;
        };
        const auto previewer =
                ParallelArrayDispatcher<double>(state, actualIterationMatrix, w, h, s.fractal.general.threads,
                                                s.render.pixelRenderPriority, std::move(func));


        auto statusThread = std::jthread([&renderPixelsCount, len, this, startTime](const std::stop_token &stop) {
            static float time = rootWindowContext->getWindow()->getTime();
            while (!stop.stop_requested()) {
                const float elapsed = rootWindowContext->getWindow()->getTime() - time;
                if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                    time = rootWindowContext->getWindow()->getTime();
                    float ratio = static_cast<float>(renderPixelsCount.load()) / static_cast<float>(len) * 100;
                    setStatusMessage(Constants::Status::TIME_STATUS,
                                     std::format("Time : {}", Utilities::formatTime(time - startTime)));
                    setStatusMessage(Constants::Status::RENDER_STATUS, std::format("Calculation : {:.3f}%", ratio));
                }
            }
        });


        canShowPreview = true;
        previewer.dispatch();

        statusThread.request_stop();
        statusThread.join();

        if (state.interruptRequested())
            return;

        const auto syncer = ParallelArrayDispatcher<double>(
                state, actualIterationMatrix, w, h, s.fractal.general.threads, RndPixelRenderPriority::SEQUENTIAL,
                [this](const uint16_t x, const uint16_t y, uint16_t, uint16_t, float, float, uint32_t, const double a) {
                    renderer->visibleIterationBufferContext->set(x, y, a);
                    return 0;
                });

        syncer.dispatch();
    }


    bool RFF2::fillIteration(const float startTime, const Settings &s) {

        if (state.interruptRequested())
            return false;

        if (const auto lightDat = dynamic_cast<NormalMB2RenderData *>(renderData.get());
            lightDat && s.render.computeShader.use && !s.fractal.mpa.useCompress &&
            s.fractal.reference.compression.compressCriteria == 0) {
            renderer->rg0->iterationPalette->setPerturbationMainIterator(PerturbationMainIterator::GPU);
            fillIterationComputeShader(lightDat, startTime, s);
        } else {
            renderer->rg0->iterationPalette->setPerturbationMainIterator(PerturbationMainIterator::CPU);
            fillIterationMultithreaded(startTime, s);
        }

        if (state.interruptRequested()) {
            return false;
        }

        setStatusMessage(Constants::Status::RENDER_STATUS, "Done");
        return true;
    }

    void RFF2::afterComputeFinally(const bool success) {
        if (!success) {
            // vkh::logger::log("Recompute cancelled.");
        }
        requests.recomputeRequestedState = success ? ComputeState::IDLE : ComputeState::CANCELLED;
        backgroundThreads.notifyAll();
    }
} // namespace merutilm::rff2
