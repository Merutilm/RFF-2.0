//
// Created by Merutilm on 2025-05-07.
//

#include "RFFRenderScene.h"

#include <chrono>
#include <cmath>
#include <format>
#include "glad.h"
#include "RFF.h"
#include "RFFUtilities.h"
#include "../calc/double_exp_math.h"
#include "../formula/DeepMandelbrotPerturbator.h"
#include "../formula/Perturbator.h"
#include "../parallel/ParallelArrayDispatcher.h"
#include "../preset/shader/bloom/BloomPresets.h"
#include "../preset/shader/color/ColorPresets.h"
#include "../preset/shader/fog/FogPresets.h"
#include "../preset/shader/palette/PalettePresets.h"
#include "../preset/shader/slope/SlopePresets.h"
#include "../preset/shader/stripe/StripePresets.h"
#include <opencv2/opencv.hpp>

#include "RFFCallbackExplore.h"
#include "../locator/MandelbrotLocator.h"
#include "../parallel/ParallelDispatcher.h"
#include "../preset/calc/CalculationPresets.h"
#include "../preset/render/RenderPresets.h"

RFFRenderScene::RFFRenderScene() : state(ParallelRenderState()), settings(initSettings()) {
}

RFFRenderScene::~RFFRenderScene() {
    state.cancel();
}

Settings RFFRenderScene::initSettings() {
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
            .absoluteIterationMode = false
        },
        .renderSettings = RenderPresets::High().renderSettings(),
        .shaderSettings = {
            .paletteSettings = PalettePresets::LongRandom64().paletteSettings(),
            .stripeSettings = StripePresets::SlowAnimated().stripeSettings(),
            .slopeSettings = SlopePresets::Normal().slopeSettings(),
            .colorSettings = ColorPresets::WeakContrast().colorSettings(),
            .fogSettings = FogPresets::Medium().fogSettings(),
            .bloomSettings = BloomPresets::Normal().bloomSettings()
        },
        .videoSettings = {
            .dataSettings = {
                .defaultZoomIncrement = 2
            },
            .animationSettings = {
                .overZoom = 2,
                .showText = false,
                .mps = 1
            },
            .exportSettings = {
                .fps = 60,
                .bitrate = 9000
            }
        }
    };
}


void RFFRenderScene::runAction(const UINT message, const WPARAM wParam) {
    switch (message) {
        case WM_LBUTTONDOWN: {
            SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
            mouseX = getMouseXOnIterationBuffer();
            mouseY = getMouseYOnIterationBuffer();
            break;
        }
        case WM_LBUTTONUP: {
            SetCursor(LoadCursor(nullptr, IDC_CROSS));
            mouseX = 0;
            mouseY = 0;
        }
        case WM_MOUSEMOVE: {
            const uint16_t x = getMouseXOnIterationBuffer();
            const uint16_t y = getMouseYOnIterationBuffer();
            if (wParam == MK_LBUTTON && mouseX > 0 && mouseY > 0) {
                SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                const int16_t dx = static_cast<int16_t>(mouseX) - x;
                const int16_t dy = static_cast<int16_t>(mouseY) - y;
                const float m = settings.renderSettings.clarityMultiplier;
                const float logZoom = settings.calculationSettings.logZoom;
                fp_complex &center = settings.calculationSettings.center;
                center = center.addCenterDouble(double_exp::value(static_cast<float>(dx) / m) / getDivisor(settings),
                                                double_exp::value(static_cast<float>(dy) / m) / getDivisor(settings),
                                                Perturbator::logZoomToExp10(logZoom));
                mouseX = x;
                mouseY = y;
                requestRecompute();
            } else {
                SetCursor(LoadCursor(nullptr, IDC_CROSS));
                if (currentMap == nullptr) {
                    return;
                }


                if (auto it = static_cast<uint64_t>((*iterationMatrix)(x, y)); it != 0) {
                    setStatusMessage(RFF::Status::ITERATION_STATUS, std::format("I : {} ({}, {})", it, x, y));
                }
            }
            break;
        }
        case WM_MOUSEWHEEL: {
            const int value = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 1 : -1;
            constexpr float increment = RFF::Render::ZOOM_INTERVAL;

            settings.calculationSettings.logZoom = std::max(RFF::Render::ZOOM_MIN,
                                                            settings.calculationSettings.logZoom);
            if (value == 1) {
                const std::array<double_exp, 2> offset = offsetConversion(settings, getMouseXOnIterationBuffer(),
                                                                          getMouseYOnIterationBuffer());
                const double mzi = 1.0 / pow(10, RFF::Render::ZOOM_INTERVAL);
                float &logZoom = settings.calculationSettings.logZoom;
                logZoom += increment;
                settings.calculationSettings.center = settings.calculationSettings.center.addCenterDouble(
                    offset[0] * (1 - mzi), offset[1] * (1 - mzi), Perturbator::logZoomToExp10(logZoom));
            }
            if (value == -1) {
                const std::array<double_exp, 2> offset = offsetConversion(settings, getMouseXOnIterationBuffer(),
                                                                          getMouseYOnIterationBuffer());
                const double mzo = 1.0 / pow(10, -RFF::Render::ZOOM_INTERVAL);
                float &logZoom = settings.calculationSettings.logZoom;
                logZoom -= increment;
                settings.calculationSettings.center = settings.calculationSettings.center.addCenterDouble(
                    offset[0] * (1 - mzo), offset[1] * (1 - mzo), Perturbator::logZoomToExp10(logZoom));
            }


            requestRecompute();
            break;
        }
        default: {
            //noop
        }
    }
}


std::array<double_exp, 2> RFFRenderScene::offsetConversion(const Settings &settings, const int mx, const int my) const {
    return {
        double_exp::value(static_cast<double>(mx) - static_cast<double>(getIterationBufferWidth(settings)) / 2.0) /
        getDivisor(settings)
        / settings.renderSettings.clarityMultiplier,
        double_exp::value(static_cast<double>(my) - static_cast<double>(getIterationBufferHeight(settings)) / 2.0) /
        getDivisor(settings)
        / settings.renderSettings.clarityMultiplier
    };
}

double_exp RFFRenderScene::getDivisor(const Settings &settings) {
    double_exp v = double_exp::DEX_ZERO;
    dex_exp::exp10(&v, settings.calculationSettings.logZoom);
    return v;
}

uint16_t RFFRenderScene::getClientWidth() const {
    RECT rect;
    GetClientRect(getRenderWindow(), &rect);
    return static_cast<uint16_t>(rect.right - rect.left);
}

uint16_t RFFRenderScene::getClientHeight() const {
    RECT rect;
    GetClientRect(getRenderWindow(), &rect);
    return static_cast<uint16_t>(rect.bottom - rect.top);
}

uint16_t RFFRenderScene::getIterationBufferWidth(const Settings &settings) const {
    const float multiplier = settings.renderSettings.clarityMultiplier;
    return static_cast<int>(static_cast<float>(getClientWidth()) * multiplier);
}

uint16_t RFFRenderScene::getIterationBufferHeight(const Settings &settings) const {
    const float multiplier = settings.renderSettings.clarityMultiplier;
    return static_cast<int>(static_cast<float>(getClientHeight()) * multiplier);
}


void RFFRenderScene::configure(const HWND wnd, const HDC hdc, const HGLRC context,
                               std::array<std::string, RFF::Status::LENGTH> *statusMessageRef) {
    RFFScene::configure(wnd, hdc, context);
    this->statusMessageRef = statusMessageRef;

    makeContextCurrent();
    renderer = std::make_unique<GLMultipassRenderer>();

    rendererIteration = std::make_unique<GLRendererIteration>();
    rendererStripe = std::make_unique<GLRendererStripe>();
    rendererSlope = std::make_unique<GLRendererSlope>();
    rendererColorFilter = std::make_unique<GLRendererColor>();
    rendererFog = std::make_unique<GLRendererFog>();
    rendererBloom = std::make_unique<GLRendererBloom>();
    rendererAntialiasing = std::make_unique<GLRendererAntialiasing>();


    renderer->add(*rendererIteration);
    renderer->add(*rendererStripe);
    renderer->add(*rendererSlope);
    renderer->add(*rendererColorFilter);
    renderer->add(*rendererFog);
    renderer->add(*rendererBloom);
    renderer->add(*rendererAntialiasing);

    requestResize();
    requestColor();
    requestRecompute();
}

void RFFRenderScene::renderGL() {
    renderer->setTime(RFFUtilities::getTime());

    if (colorRequested.exchange(false)) {
        applyColor(settings);
    }

    if (resizeRequested.exchange(false)) {
        state.cancel();
        applyResize();
    }

    if (recomputeRequested.exchange(false)) {
        recompute();
    }

    if (createImageRequested.exchange(false)) {
        applyCreateImage();
    }

    makeContextCurrent();
    glClear(GL_COLOR_BUFFER_BIT);
    renderer->render();
    renderer->display();
    swapBuffers();
}

void RFFRenderScene::requestColor() {
    colorRequested = true;
}

void RFFRenderScene::requestResize() {
    resizeRequested = true;
}

void RFFRenderScene::requestRecompute() {
    recomputeRequested = true;
}

void RFFRenderScene::requestCreateImage() {
    createImageRequested = true;
}


void RFFRenderScene::applyCreateImage() const {
    const GLuint fbo = renderer->getRenderedFBO();
    const GLuint fboID = renderer->getRenderedFBOTexID();
    const int width = renderer->getWidth();
    const int height = renderer->getHeight();
    GLRenderer::bindFBO(fbo, fboID);
    std::vector<char> pixels(width * height * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    auto img = cv::Mat(height, width, CV_8UC4, pixels.data());
    cv::flip(img, img, 0);
    cv::cvtColor(img, img, cv::COLOR_RGBA2BGRA);
    cv::imwrite("sample.png", img);
    GLRenderer::unbindFBO(fbo);
}

void RFFRenderScene::applyColor(const Settings &settings) const {
    rendererIteration->setPaletteSettings(settings.shaderSettings.paletteSettings);
    rendererStripe->setStripeSettings(settings.shaderSettings.stripeSettings);
    rendererSlope->setSlopeSettings(settings.shaderSettings.slopeSettings);
    rendererColorFilter->setColorSettings(settings.shaderSettings.colorSettings);
    rendererFog->setFogSettings(settings.shaderSettings.fogSettings);
    rendererBloom->setBloomSettings(settings.shaderSettings.bloomSettings);
    rendererAntialiasing->setUse(settings.renderSettings.antialiasing);
}

void RFFRenderScene::applyResize() {
    const int cw = getClientWidth();
    const int ch = getClientHeight();
    const int iw = getIterationBufferWidth(settings);
    const int ih = getIterationBufferHeight(settings);
    glViewport(0, 0, cw, ch);
    rendererSlope->setClarityMultiplier(settings.renderSettings.clarityMultiplier);
    rendererIteration->reloadIterationBuffer(iw, ih, settings.calculationSettings.maxIteration);
    renderer->reloadSize(cw, ch, iw, ih);
    iterationMatrix = std::make_unique<Matrix<double> >(iw, ih);
}

void RFFRenderScene::overwriteMatrixFromMap() const {
    renderer->reloadSize(getClientWidth(), getClientHeight(), currentMap->iterations.width, currentMap->iterations.height);
    rendererIteration->setAllIterations(currentMap->iterations);
}


uint16_t RFFRenderScene::getMouseXOnIterationBuffer() const {
    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(getRenderWindow(), &cursor);
    const float multiplier = settings.renderSettings.clarityMultiplier;
    return static_cast<uint16_t>(static_cast<float>(cursor.x) * multiplier);
}

uint16_t RFFRenderScene::getMouseYOnIterationBuffer() const {
    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(getRenderWindow(), &cursor);
    const float multiplier = settings.renderSettings.clarityMultiplier;
    return getIterationBufferHeight(settings) - static_cast<uint16_t>(static_cast<float>(cursor.y) * multiplier);
}


void RFFRenderScene::recompute() {
    state.createThread([this](std::stop_token) {
        Settings settings = this->settings; //clone the settings
        beforeCompute(settings);
        const bool success = compute(settings);
        afterCompute(success);
    });
}

void RFFRenderScene::beforeCompute(Settings &settings) {
    idle = false;
    settings.calculationSettings.maxIteration = std::max(this->settings.calculationSettings.maxIteration,
                                                         lastPeriod * RFF::Render::AUTOMATIC_ITERATION_MULTIPLIER);
    rendererIteration->setMaxIteration(settings.calculationSettings.maxIteration);
}


bool RFFRenderScene::compute(const Settings &settings) {
    const Settings settingsToUse = settings;
    int w = getIterationBufferWidth(settingsToUse);
    int h = getIterationBufferHeight(settingsToUse);

    if (state.interruptRequested()) return false;

    auto &calc = settingsToUse.calculationSettings;

    float logZoom = calc.logZoom;

    if (state.interruptRequested()) return false;

    setStatusMessage(RFF::Status::ZOOM_STATUS,
                     std::format("Z : {:.06f}E{:d}", pow(10, fmod(logZoom, 1)), static_cast<int>(logZoom)));

    const std::array<double_exp, 2> offset = offsetConversion(settingsToUse, 0, 0);
    double_exp dcMax = double_exp::DEX_ZERO;
    dex_trigonometric::hypot_approx(&dcMax, offset[0], offset[1]);

    const auto refreshInterval = RFFUtilities::getRefreshInterval(logZoom);
    std::function actionPerRefCalcIteration = [refreshInterval, this](const uint64_t p) {
        if (p % refreshInterval == 0) {
            setStatusMessage(RFF::Status::RENDER_STATUS, std::format(std::locale(), "P : {:L}", p));
        }
    };
    std::function actionPerCreatingTableIteration = [refreshInterval, this](const uint64_t p, const double i) {
        if (p % refreshInterval == 0) {
            setStatusMessage(RFF::Status::RENDER_STATUS, std::format("A : {:.3f}%", i * 100));
        }
    };


    if (state.interruptRequested()) return false;
    auto start = std::chrono::high_resolution_clock::now();

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
            uint64_t period = currentPerturbator->getReference()->longestPeriod();
            auto center = MandelbrotLocator::locateMinibrot(state, currentPerturbator.get(), approxTableCache,
                                                            RFFCallbackExplore::getActionWhileFindingMinibrotCenter(
                                                                *this, logZoom, period),
                                                            RFFCallbackExplore::getActionWhileCreatingTable(
                                                                *this, logZoom),
                                                            RFFCallbackExplore::getActionWhileFindingZoom(*this)
            );
            if (center == nullptr) return false;

            CalculationSettings refCalc = calc;
            refCalc.center = center->perturbator->calc.center;
            refCalc.logZoom = center->perturbator->calc.logZoom;
            int refExp10 = Perturbator::logZoomToExp10(refCalc.logZoom);

            if (refCalc.logZoom > RFF::Render::ZOOM_DEADLINE) {
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
            if (logZoom > RFF::Render::ZOOM_DEADLINE) {
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
    if (reference == RFF::NullPointer::PROCESS_TERMINATED_REFERENCE || state.interruptRequested())
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

    setStatusMessage(RFF::Status::PERIOD_STATUS, std::format("P : {:L} ({:L}, {:L})", lastPeriod, refLength, mpaLen));
    if (state.interruptRequested()) return false;


    std::atomic renderPixelsCount = 0;

    auto rendered = std::vector<bool>(w * h);

    auto previewer = ParallelArrayDispatcher<double>(
        state, *iterationMatrix,
        [settingsToUse, this, &renderPixelsCount, &rendered](const int x, const int y, const int xRes, int, float,
                                                             float, const int i,
                                                             double) {
            rendered[i] = true;
            const auto dc = offsetConversion(settingsToUse, x, y);
            const double iteration = currentPerturbator->iterate(dc[0], dc[1]);
            rendererIteration->setIteration(x, y, iteration);

            int my = y - 1;
            while (my >= 0 && !rendered[my * xRes + x]) {
                rendererIteration->setIteration(x, my, iteration);
                --my;
            }

            ++renderPixelsCount;
            return iteration;
        });

    rendererIteration->fillZero();

    auto statusThread = std::jthread([&renderPixelsCount, w, h, this, &start](const std::stop_token &stop) {
        while (!stop.stop_requested()) {
            const auto current = std::chrono::system_clock::now();
            const auto elapsed = std::chrono::milliseconds((current - start).count() / 1000000);
            auto hms = std::chrono::hh_mm_ss(elapsed);
            float ratio = static_cast<float>(renderPixelsCount.load()) / static_cast<float>(w * h) * 100;
            setStatusMessage(RFF::Status::TIME_STATUS,
                             std::format("T : {:02d}:{:02d}:{:02d}:{:03d}", hms.hours().count(),
                                         hms.minutes().count(), hms.seconds().count(), hms.subseconds().count()));
            setStatusMessage(RFF::Status::RENDER_STATUS, std::format("C : {:.3f}%", ratio));

            Sleep(RFF::Status::SET_PROCESS_INTERVAL_MS);
        }
    });

    previewer.dispatch();

    statusThread.request_stop();
    statusThread.join();

    if (state.interruptRequested()) return false;

    auto syncer = ParallelDispatcher(
        state, w, h,
        [this](const uint16_t x, const uint16_t y, int, int, float, float, int) {
            rendererIteration->setIteration(x, y, (*iterationMatrix)(x, y));
        });

    syncer.dispatch();

    if (state.interruptRequested()) return false;

    currentMap = std::make_unique<RFFMap>(calc.logZoom, lastPeriod, calc.maxIteration, *iterationMatrix);
    setStatusMessage(RFF::Status::RENDER_STATUS, std::string("Done"));

    return true;
}

void RFFRenderScene::afterCompute(const bool success) {
    if (!success) {
        RFFUtilities::log("Recompute cancelled.");
    }
    if (success && settings.calculationSettings.reuseReferenceMethod == ReuseReferenceMethod::CENTERED_REFERENCE) {
        settings.calculationSettings.reuseReferenceMethod = ReuseReferenceMethod::CURRENT_REFERENCE;
    }
    idle = true;
    backgroundThreads.notifyAll();
}


void RFFRenderScene::setStatusMessage(const int index, const std::string_view &message) const {
    (*statusMessageRef)[index] = std::string("  ").append(message);
}

Settings &RFFRenderScene::getSettings() {
    return settings;
}

ParallelRenderState &RFFRenderScene::getState() {
    return state;
}

MandelbrotPerturbator *RFFRenderScene::getCurrentPerturbator() const {
    return currentPerturbator.get();
}


void RFFRenderScene::setCurrentPerturbator(std::unique_ptr<MandelbrotPerturbator> perturbator) {
    this->currentPerturbator = std::move(perturbator);
}

ApproxTableCache &RFFRenderScene::getApproxTableCache() {
    return approxTableCache;
}

BackgroundThreads &RFFRenderScene::getBackgroundThreads() {
    return backgroundThreads;
}

RFFMap &RFFRenderScene::getCurrentMap() const {
    return *currentMap;
}

void RFFRenderScene::setCurrentMap(const RFFMap &map) {
    this->currentMap = std::make_unique<RFFMap>(map);
}

bool RFFRenderScene::isRecomputeRequested() const {
    return recomputeRequested;
}

bool RFFRenderScene::isCreateImageRequested() const {
    return createImageRequested;
}

bool RFFRenderScene::isResizeRequested() const {
    return resizeRequested;
}

bool RFFRenderScene::isColorRequested() const {
    return colorRequested;
}

bool RFFRenderScene::isIdle() const {
    return idle;
}

int RFFRenderScene::getCWRequest() const {
    return cwRequest;
}

int RFFRenderScene::getCHRequest() const {
    return chRequest;
}

void RFFRenderScene::clientResizeRequestSolved() {
    cwRequest = 0;
    chRequest = 0;
}







