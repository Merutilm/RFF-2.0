//
// Created by Merutilm on 2025-08-08.
//

#include "RenderScene.hpp"

#include "CallbackExplore.h"
#include "IOUtilities.h"
#include "../constants/VulkanWindowConstants.hpp"
#include "../vulkan/BasicRenderContextConfigurator.hpp"
#include "../vulkan/IterationPalettePipelineConfigurator.hpp"
#include "../vulkan/Template2PipelineConfigurator.hpp"
#include "../calc/dex_exp.h"
#include "../formula/DeepMandelbrotPerturbator.h"
#include "../formula/LightMandelbrotPerturbator.h"
#include "../locator/MandelbrotLocator.h"
#include "../parallel/ParallelArrayDispatcher.h"
#include "../parallel/ParallelDispatcher.h"
#include "../preset/calc/CalculationPresets.h"
#include "../preset/render/RenderPresets.h"
#include "../preset/shader/bloom/BloomPresets.h"
#include "../preset/shader/color/ColorPresets.h"
#include "../preset/shader/fog/FogPresets.h"
#include "../preset/shader/palette/PalettePresets.h"
#include "../preset/shader/slope/SlopePresets.h"
#include "../preset/shader/stripe/StripePresets.h"
#include "../vulkan/SharedDescriptorTemplate.hpp"


namespace merutilm::rff2 {
    RenderScene::RenderScene(vkh::Engine &engine, const HWND window,
                             std::array<std::string, Constants::Status::LENGTH> *statusMessageRef) : engine(engine),
        window(window), settings(defaultSettings()), statusMessageRef(statusMessageRef) {
        init();
    }

    RenderScene::~RenderScene() {
        destroy();
    }

    void RenderScene::init() {
        initRenderContext();
        initShaderPrograms();
    }

    void RenderScene::render(const VkCommandBuffer cbh, const uint32_t frameIndex, const VkExtent2D &extent) {
        using namespace SharedDescriptorTemplate;

        engine.getRepositories().getRepository<vkh::SharedDescriptorRepo>()->
                pick(vkh::DescriptorTemplate::from<DescTime>(),
                     engine.getRepositories().getDescriptorRequiresRepositoryContext()).getDescriptorManager().get<
                    std::unique_ptr<
                        vkh::Uniform> >(DescTime::BINDING_UBO_TIME)
                ->set(DescTime::TARGET_TIME_CURRENT, Utilities::getCurrentTime());

        if (colorRequested) {
            applyColor(settings);
            colorRequested.exchange(false);
            backgroundThreads.notifyAll();
        }

        if (resizeRequested) {
            state.cancel();
            applyResize();
            resizeRequested.exchange(false);
            backgroundThreads.notifyAll();
        }

        if (recomputeRequested) {
            idleCompute = false;
            recomputeRequested.exchange(false);
            recomputeThreaded();
            //it is threaded, not idle
        }

        if (createImageRequested) {
            applyCreateImage();
            createImageRequested.exchange(false);
            backgroundThreads.notifyAll();
        }


        for (int i = 0; i < shaderPrograms.size(); ++i) {
            shaderPrograms[i]->pipeline->bind(cbh, frameIndex);
            shaderPrograms[i]->render(cbh, frameIndex, extent.width, extent.height);
            if (i < shaderPrograms.size() - 1) {
                vkCmdNextSubpass(cbh, VK_SUBPASS_CONTENTS_INLINE);
            }
        }
    }

    Settings RenderScene::defaultSettings() {
        return Settings{
            .calculationSettings = CalculationSettings{
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
                .decimalizeIterationMethod = DecimalizeIterationMethod::LOG_LOG,
                .mpaSettings = CalculationPresets::UltraFast().mpaSettings(),
                .referenceCompressionSettings = CalculationPresets::UltraFast().referenceCompressionSettings(),
                .reuseReferenceMethod = ReuseReferenceMethod::DISABLED,
                .autoMaxIteration = true,
                .autoIterationMultiplier = 100,
                .absoluteIterationMode = false
            },
            .renderSettings = RenderPresets::High().renderSettings(),
            .shaderSettings = {
                .paletteSettings = PalettePresets::LongRandom64().paletteSettings(),
                .stripeSettings = StripePresets::SlowAnimated().stripeSettings(),
                .slopeSettings = SlopePresets::Translucent().slopeSettings(),
                .colorSettings = ColorPresets::WeakContrast().colorSettings(),
                .fogSettings = FogPresets::Medium().fogSettings(),
                .bloomSettings = BloomPresets::Normal().bloomSettings()
            },
            .videoSettings = {
                .dataSettings = {
                    .defaultZoomIncrement = 2,
                    .isStatic = false
                },
                .animationSettings = {
                    .overZoom = 2,
                    .showText = true,
                    .mps = 1
                },
                .exportSettings = {
                    .fps = 60,
                    .bitrate = 9000
                }
            }
        };
    }

    void RenderScene::runAction(const UINT message, const WPARAM wParam) {
        switch (message) {
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
                if (wParam == MK_LBUTTON && interactedMX > 0 && interactedMY > 0) {
                    SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                    const auto dx = static_cast<int16_t>(interactedMX - x);
                    const auto dy = static_cast<int16_t>(interactedMY - y);
                    const float m = settings.renderSettings.clarityMultiplier;
                    const float logZoom = settings.calculationSettings.logZoom;
                    fp_complex &center = settings.calculationSettings.center;
                    center = center.addCenterDouble(dex::value(static_cast<float>(dx) / m) / getDivisor(settings),
                                                    dex::value(static_cast<float>(dy) / m) / getDivisor(settings),
                                                    Perturbator::logZoomToExp10(logZoom));
                    interactedMX = x;
                    interactedMY = y;
                    requestRecompute();
                } else {
                    SetCursor(LoadCursor(nullptr, IDC_CROSS));
                    if (currentMap == nullptr) {
                        return;
                    }


                    if (auto it = static_cast<uint64_t>((*iterationMatrix)(x, y)); it != 0) {
                        setStatusMessage(Constants::Status::ITERATION_STATUS, std::format("I : {} ({}, {})", it, x, y));
                    }
                }
                break;
            }
            case WM_MOUSEWHEEL: {
                const int value = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 1 : -1;
                constexpr float increment = Constants::Render::ZOOM_INTERVAL;

                settings.calculationSettings.logZoom = std::max(Constants::Render::ZOOM_MIN,
                                                                settings.calculationSettings.logZoom);
                if (value == 1) {
                    const std::array<dex, 2> offset = offsetConversion(settings, getMouseXOnIterationBuffer(),
                                                                       getMouseYOnIterationBuffer());
                    const double mzi = 1.0 / pow(10, Constants::Render::ZOOM_INTERVAL);
                    float &logZoom = settings.calculationSettings.logZoom;
                    logZoom += increment;
                    settings.calculationSettings.center = settings.calculationSettings.center.addCenterDouble(
                        offset[0] * (1 - mzi),
                        offset[1] * (1 - mzi),
                        Perturbator::logZoomToExp10(logZoom));
                }
                if (value == -1) {
                    const std::array<dex, 2> offset = offsetConversion(settings, getMouseXOnIterationBuffer(),
                                                                       getMouseYOnIterationBuffer());
                    const double mzo = 1.0 / pow(10, -Constants::Render::ZOOM_INTERVAL);
                    float &logZoom = settings.calculationSettings.logZoom;
                    logZoom -= increment;
                    settings.calculationSettings.center = settings.calculationSettings.center.addCenterDouble(
                        offset[0] * (1 - mzo),
                        offset[1] * (1 - mzo),
                        Perturbator::logZoomToExp10(logZoom));
                }


                requestRecompute();
                break;
            }
            default: {
                //noop
            }
        }
    }

    std::array<dex, 2> RenderScene::offsetConversion(const Settings &settings, int mx, int my) const {
        return {
            dex::value(static_cast<double>(mx) - static_cast<double>(getIterationBufferWidth(settings)) / 2.0) /
            getDivisor(settings)
            / settings.renderSettings.clarityMultiplier,
            dex::value(static_cast<double>(my) - static_cast<double>(getIterationBufferHeight(settings)) / 2.0) /
            getDivisor(settings)
            / settings.renderSettings.clarityMultiplier
        };
    }

    dex RenderScene::getDivisor(const Settings &settings) {
        dex v = dex::ZERO;
        dex_exp::exp10(&v, settings.calculationSettings.logZoom);
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

    uint16_t RenderScene::getIterationBufferWidth(const Settings &settings) const {
        const float multiplier = settings.renderSettings.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(getClientWidth()) * multiplier);
    }

    uint16_t RenderScene::getIterationBufferHeight(const Settings &settings) const {
        const float multiplier = settings.renderSettings.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(getClientHeight()) * multiplier);
    }

    void RenderScene::applyCreateImage() {
        if (createImageRequestedFilename.empty()) {
            createImageRequestedFilename = IOUtilities::ioFileDialog("Save image", Constants::Extension::DESC_IMAGE,
                                                                     IOUtilities::SAVE_FILE,
                                                                     Constants::Extension::IMAGE)->string();
        }
        // const GLuint fbo = renderer->getRenderedFBO();
        // const GLuint fboID = renderer->getRenderedFBOTexID();
        // const int width = renderer->getWidth();
        // const int height = renderer->getHeight();
        // GLRenderer::bindFBO(fbo, fboID);
        // std::vector<char> pixels(width * height * 4);
        // glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        //
        // auto img = cv::Mat(height, width, CV_8UC4, pixels.data());
        // cv::flip(img, img, 0);
        // cv::cvtColor(img, img, cv::COLOR_RGBA2BGRA);
        // cv::imwrite(createImageRequestedFilename, img);
        // GLRenderer::unbindFBO(fbo);
    }

    void RenderScene::applyColor(const Settings &settings) const {
        // rendererIteration->setPaletteSettings(settings.shaderSettings.paletteSettings);
        // rendererStripe->setStripeSettings(settings.shaderSettings.stripeSettings);
        // rendererSlope->setSlopeSettings(settings.shaderSettings.slopeSettings);
        // rendererColorFilter->setColorSettings(settings.shaderSettings.colorSettings);
        // rendererFog->setFogSettings(settings.shaderSettings.fogSettings);
        // rendererBloom->setBloomSettings(settings.shaderSettings.bloomSettings);
        // rendererAntialiasing->setUse(settings.renderSettings.antialiasing);
    }

    void RenderScene::applyResize() {
        const uint16_t cw = getClientWidth();
        const uint16_t ch = getClientHeight();
        const uint16_t iw = getIterationBufferWidth(settings);
        const uint16_t ih = getIterationBufferHeight(settings);
        // glViewport(0, 0, cw, ch);
        // rendererSlope->setClarityMultiplier(settings.renderSettings.clarityMultiplier);
        // rendererIteration->reloadIterationBuffer(iw, ih);
        // renderer->reloadSize(cw, ch, iw, ih);
        iterationMatrix = std::make_unique<Matrix<double> >(iw, ih);
    }

    void RenderScene::overwriteMatrixFromMap() const {
        // renderer->reloadSize(getClientWidth(), getClientHeight(), currentMap->getMatrix().getWidth(),
        //                      currentMap->getMatrix().getHeight());
        // rendererIteration->setMaxIteration(static_cast<double>(currentMap->getMaxIteration()));
        // rendererIteration->setAllIterations(currentMap->getMatrix());
    }

    uint16_t RenderScene::getMouseXOnIterationBuffer() const {
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(window, &cursor);
        const float multiplier = settings.renderSettings.clarityMultiplier;
        return static_cast<uint16_t>(static_cast<float>(cursor.x) * multiplier);
    }

    uint16_t RenderScene::getMouseYOnIterationBuffer() const {
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(window, &cursor);
        const float multiplier = settings.renderSettings.clarityMultiplier;
        return getIterationBufferHeight(settings) - static_cast<uint16_t>(static_cast<float>(cursor.y) * multiplier);
    }

    void RenderScene::recomputeThreaded() {
        state.createThread([this](std::stop_token) {
            Settings settings = this->settings; //clone the settings
            beforeCompute(settings);
            const bool success = compute(settings);
            afterCompute(success);
        });
    }

    void RenderScene::beforeCompute(Settings &settings) const {
        settings.calculationSettings.maxIteration = settings.calculationSettings.autoMaxIteration
                                                        ? lastPeriod * settings.calculationSettings.
                                                          autoIterationMultiplier
                                                        : this->settings.calculationSettings.maxIteration;
        // rendererIteration->setMaxIteration(static_cast<double>(settings.calculationSettings.maxIteration));
    }

    bool RenderScene::compute(const Settings &settings) {
        auto start = std::chrono::high_resolution_clock::now();
        const uint16_t w = getIterationBufferWidth(settings);
        const uint16_t h = getIterationBufferHeight(settings);
        uint32_t len = uint32_t(w) * h;

        if (state.interruptRequested()) return false;

        auto &calc = settings.calculationSettings;

        float logZoom = calc.logZoom;

        if (state.interruptRequested()) return false;

        setStatusMessage(Constants::Status::ZOOM_STATUS,
                         std::format("Z : {:.06f}E{:d}", pow(10, fmod(logZoom, 1)), static_cast<int>(logZoom)));

        const std::array<dex, 2> offset = offsetConversion(settings, 0, 0);
        dex dcMax = dex::ZERO;
        dex_trigonometric::hypot_approx(&dcMax, offset[0], offset[1]);
        const auto refreshInterval = Utilities::getRefreshInterval(logZoom);
        std::function actionPerRefCalcIteration = [refreshInterval, this, &start](const uint64_t p) {
            if (p % refreshInterval == 0) {
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format(std::locale(), "P : {:L}", p));
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
            }
        };
        std::function actionPerCreatingTableIteration = [refreshInterval, this, &start
                ](const uint64_t p, const double i) {
            if (p % refreshInterval == 0) {
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format("A : {:.3f}%", i * 100));
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
            }
        };


        if (state.interruptRequested()) return false;
        switch (calc.reuseReferenceMethod) {
                using enum ReuseReferenceMethod;
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
                // uint64_t period = currentPerturbator->getReference()->longestPeriod();
                // auto center = MandelbrotLocator::locateMinibrot(state, currentPerturbator.get(), approxTableCache,
                //                                                 CallbackExplore::getActionWhileFindingMinibrotCenter(
                //                                                     *this, logZoom, period),
                //                                                 CallbackExplore::getActionWhileCreatingTable(
                //                                                     *this, logZoom),
                //                                                 CallbackExplore::getActionWhileFindingZoom(*this)
                // );
                // if (center == nullptr) return false;
                //
                // CalculationSettings refCalc = calc;
                // refCalc.center = center->perturbator->calc.center;
                // refCalc.logZoom = center->perturbator->calc.logZoom;
                // int refExp10 = Perturbator::logZoomToExp10(refCalc.logZoom);
                //
                // if (refCalc.logZoom > Constants::Render::ZOOM_DEADLINE) {
                //     currentPerturbator = std::make_unique<DeepMandelbrotPerturbator>(
                //                 state, refCalc, center->perturbator->getDcMaxAsDoubleExp(),
                //                 refExp10,
                //                 period, approxTableCache, std::move(actionPerRefCalcIteration),
                //                 std::move(actionPerCreatingTableIteration))
                //             ->reuse(calc, dcMax, approxTableCache);
                // } else {
                //     currentPerturbator = std::make_unique<LightMandelbrotPerturbator>(state, refCalc,
                //                 static_cast<double>(center->perturbator->getDcMaxAsDoubleExp()),
                //                 refExp10, period, approxTableCache, std::move(actionPerRefCalcIteration),
                //                 std::move(actionPerCreatingTableIteration))
                //             ->reuse(calc, static_cast<double>(dcMax), approxTableCache);
                // }
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
        if (auto t = dynamic_cast<LightMandelbrotPerturbator *>(currentPerturbator.get())) {
            mpaLen = t->getTable().getLength();
        }
        if (auto t = dynamic_cast<DeepMandelbrotPerturbator *>(currentPerturbator.get())) {
            mpaLen = t->getTable().getLength();
        }

        setStatusMessage(Constants::Status::PERIOD_STATUS,
                         std::format("P : {:L} ({:L}, {:L})", lastPeriod, refLength, mpaLen));
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
                // rendererIteration->setIteration(x, y, iteration);

                int16_t my = y - 1;
                while (my >= 0 && !rendered[my * xRes + x]) {
                    // rendererIteration->setIteration(x, my, iteration);
                    --my;
                }

                ++renderPixelsCount;
                return iteration;
            });

        // rendererIteration->fillZero();

        auto statusThread = std::jthread([&renderPixelsCount, len, this, &start](const std::stop_token &stop) {
            while (!stop.stop_requested()) {
                float ratio = static_cast<float>(renderPixelsCount.load()) / static_cast<float>(len) * 100;
                setStatusMessage(Constants::Status::TIME_STATUS, Utilities::elapsed_time(start));
                setStatusMessage(Constants::Status::RENDER_STATUS, std::format("C : {:.3f}%", ratio));

                Sleep(Constants::Status::SET_PROCESS_INTERVAL_MS);
            }
        });

        previewer.dispatch();

        statusThread.request_stop();
        statusThread.join();

        if (state.interruptRequested()) return false;

        auto syncer = ParallelDispatcher(
            state, w, h,
            [this](const uint16_t x, const uint16_t y, uint16_t, uint16_t, float, float, uint32_t) {
                // rendererIteration->setIteration(x, y, (*iterationMatrix)(x, y));
            });

        syncer.dispatch();

        if (state.interruptRequested()) return false;

        currentMap = std::make_unique<RFFDynamicMapBinary>(calc.logZoom, lastPeriod, calc.maxIteration,
                                                           *iterationMatrix);
        setStatusMessage(Constants::Status::RENDER_STATUS, std::string("Done"));

        return true;
    }

    void RenderScene::afterCompute(bool success) {
        if (!success) {
            vkh::logger::log("Recompute cancelled.");
        }
        if (success && settings.calculationSettings.reuseReferenceMethod == ReuseReferenceMethod::CENTERED_REFERENCE) {
            settings.calculationSettings.reuseReferenceMethod = ReuseReferenceMethod::CURRENT_REFERENCE;
        }
        idleCompute = true;
        backgroundThreads.notifyAll();
    }


    void RenderScene::initRenderContext() const {
        const auto &swapchain = *engine.getCore().getWindowContext(
            Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).swapchain;
        auto configurator = std::make_unique<BasicRenderContextConfigurator>(engine.getCore(), swapchain);

        engine.attachRenderContext(std::make_unique<vkh::RenderContext>(engine.getCore(),
                                                                        swapchain.populateSwapchainExtent(),
                                                                        std::move(configurator)));
    }

    void RenderScene::initShaderPrograms() {
        rendererIteration = vkh::PipelineConfigurator::createShaderProgram<IterationPalettePipelineConfigurator>(
            shaderPrograms, engine, BasicRenderContextConfigurator::SUBPASS_ITERATION_INDEX);
        vkh::PipelineConfigurator::createShaderProgram<Template2PipelineConfigurator>(
            shaderPrograms, engine, BasicRenderContextConfigurator::SUBPASS_SECONDARY_INDEX);
    }


    void RenderScene::destroy() {
        shaderPrograms.clear();
    }
}
