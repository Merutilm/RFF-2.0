//
// Created by Merutilm on 2025-05-07.
//

#include "RFFRenderScene.h"

#include <chrono>
#include <cmath>
#include <commctrl.h>
#include <format>

#include "glad.h"
#include "RFF.h"
#include "RFFUtilities.h"
#include "../calc/double_exp_math.h"
#include "../formula/DeepMandelbrotPerturbator.h"
#include "../formula/Perturbator.h"
#include "../parallel/ParallelArrayDispatcher.h"


RFFRenderScene::RFFRenderScene() : referenceRenderState(ParallelRenderState()), settings(initSettings()) {
}

RFFRenderScene::~RFFRenderScene() {
    referenceRenderState.cancel();
}

Settings RFFRenderScene::initSettings() {
    return Settings{
        .calculationSettings = CalculationSettings{
            .center = fp_complex( "-0.85", "0",
                 //"-1.29255707077531686131098415679305324693162987219277534742408945445699102528813182208390942132824552642640105852802031375797639923173781472397893283277669022615909880587638643429120957543820179919830492623879949932",
                  //"-1.7433380976879299408417853435676017785972000052524291128107561584529660103218876836645852866195456038569337053542405",
               // "0.438169590583770312890168860021043433478705507119371935117854030759551072299659171256225012539071884716681573917133522314360175105572598172732723792994562397110248396170036793222839041625954944698185617470725880129",
                //"-0.00000180836819716880795128873613161993554089471597685393367018109950768833467685704762711890797154859214327088989719746641",
                Perturbator::logZoomToExp10(2)),
            .logZoom = 2,//186.47, //85.190033f,
            .maxIteration = 3000,
            .bailout = 2,
            .decimalizeIterationMethod = DecimalizeIterationMethod::LOG_LOG,
            .mpaSettings = MPASettings{
                .minSkipReference = 4,
                .maxMultiplierBetweenLevel = 2,
                .epsilonPower = -3,
                .mpaSelectionMethod = MPASelectionMethod::HIGHEST,
                .mpaCompressionMethod = MPACompressionMethod::STRONGEST,
            },
            .referenceCompressionSettings = ReferenceCompressionSettings{
                .compressCriteria = 10000,
                .compressionThresholdPower = 6,
            },
            .reuseReferenceMethod = ReuseReferenceMethod::DISABLED,
            .autoMaxIteration = true,
            .absoluteIterationMode = false
        },
        .renderSettings = {
            .clarityMultiplier = 1,
            .antialiasing = true
        },
        .shaderSettings = {
            .paletteSettings = {
                .colors = std::vector{Color{1, 0, 1, 1}, Color{0, 1, 1, 1}, Color{1, 1, 0, 1}},
                .colorSmoothing = ColorSmoothingSettings::NORMAL,
                .iterationInterval = 8000,
                .offsetRatio = 0,
                .animationSpeed = 1000
            },
            .stripeSettings = {
                .stripeType = StripeType::SINGLE_DIRECTION,
                .firstInterval = 10,
                .secondInterval = 50,
                .opacity = 1,
                .offset = 0,
                .animationSpeed = 0.5f
            },
            .slopeSettings = {
                .depth = 300,
                .reflectionRatio = 0.5f,
                .opacity = 1,
                .zenith = 60,
                .azimuth = 135
            },
            .colorSettings = {
                .gamma = 1,
                .exposure = 0,
                .hue = 0,
                .saturation = 0,
                .brightness = 0,
                .contrast = 0
            },
            .fogSettings = {
                .radius = 0.1f,
                .opacity = 0.5f
            },
            .bloomSettings = {
                .threshold = 0,
                .radius = 0.1f,
                .softness = 0,
                .intensity = 1
            }
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
        }
        case WM_MOUSEMOVE: {
            const int x = getMouseXOnIterationBuffer();
            const int y = getMouseYOnIterationBuffer();
            if (wParam == MK_LBUTTON) {
                SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                const int dx = mouseX - x;
                const int dy = mouseY - y;
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


                auto it = static_cast<uint64_t>((*iterationMatrix)(x, y));
                setStatusMessage(RFF::Status::ITERATION_STATUS, std::format("Iterations : {} ({},{})", it, x, y));
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

int RFFRenderScene::getClientWidth() const {
    RECT rect;
    GetClientRect(getRenderWindow(), &rect);
    return rect.right - rect.left;
}

int RFFRenderScene::getClientHeight() const {
    RECT rect;
    GetClientRect(getRenderWindow(), &rect);
    return rect.bottom - rect.top;
}

int RFFRenderScene::getIterationBufferWidth(const Settings &settings) const {
    const float multiplier = settings.renderSettings.clarityMultiplier;
    return static_cast<int>(static_cast<float>(getClientWidth()) * multiplier);
}

int RFFRenderScene::getIterationBufferHeight(const Settings &settings) const {
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


    renderer->add(rendererIteration.get());
    renderer->add(rendererStripe.get());
    renderer->add(rendererSlope.get());
    renderer->add(rendererColorFilter.get());
    renderer->add(rendererFog.get());
    renderer->add(rendererBloom.get());
    renderer->add(rendererAntialiasing.get());

    requestClarity();
    requestResize();
    requestColor();
    requestRecompute();
}

void RFFRenderScene::renderGL() {
    renderer->setTime(RFFUtilities::getTime());

    if (canDisplayed) {
        makeContextCurrent();
        glClear(GL_COLOR_BUFFER_BIT);
        renderer->render();
        swapBuffers();
    }

    if (colorRequested) {
        colorRequested = false;
        applyColor(settings);
    }
    if (resizeRequested) {
        resizeRequested = false;
        referenceRenderState.cancel();
        applyResize();
    }
    if (clarityRequested) {
        clarityRequested = false;
        referenceRenderState.cancel();
        applyClarity();
    }
    if (recomputeRequested) {
        isComputing = true;
        recomputeRequested = false;
        applyComputationalSettings();
        recompute();
    }
}

void RFFRenderScene::requestColor() {
    colorRequested = true;
}

void RFFRenderScene::requestResize() {
    resizeRequested = true;
}

void RFFRenderScene::requestClarity() {
    clarityRequested = true;
}

void RFFRenderScene::requestRecompute() {
    recomputeRequested = true;
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

void RFFRenderScene::applyResize() const {
    glViewport(0, 0, getClientWidth(), getClientHeight());
    renderer->reloadSize(getClientWidth(), getClientHeight());
    rendererIteration->reloadIterationBuffer(getIterationBufferWidth(settings), getIterationBufferHeight(settings),
                                             settings.calculationSettings.maxIteration);
}

void RFFRenderScene::applyClarity() const {
    rendererIteration->reloadIterationBuffer(getIterationBufferWidth(settings), getIterationBufferHeight(settings),
                                             settings.calculationSettings.maxIteration);
}

void RFFRenderScene::applyComputationalSettings() {
    if (settings.calculationSettings.autoMaxIteration) {
        settings.calculationSettings.maxIteration = std::max(RFF::Render::MINIMUM_ITERATION,
                                                             lastPeriod *
                                                             RFF::Render::AUTOMATIC_ITERATION_MULTIPLIER);
    }
}

int RFFRenderScene::getMouseXOnIterationBuffer() const {
    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(getRenderWindow(), &cursor);
    const float multiplier = settings.renderSettings.clarityMultiplier;
    return static_cast<int>(static_cast<float>(cursor.x) * multiplier);
}

int RFFRenderScene::getMouseYOnIterationBuffer() const {
    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(getRenderWindow(), &cursor);
    const float multiplier = settings.renderSettings.clarityMultiplier;
    return getIterationBufferHeight(settings) - static_cast<int>(static_cast<float>(cursor.y) * multiplier);
}


void RFFRenderScene::recompute() {
    referenceRenderState.createThread([this](std::stop_token) {
        const Settings settings = this->settings; //clone the settings
        beforeCompute(settings);
        compute(settings);
        afterCompute();
    });
}

void RFFRenderScene::beforeCompute(const Settings &settings) {
    canDisplayed = false;
    rendererIteration->resetToZero(settings.calculationSettings.maxIteration);
}


void RFFRenderScene::compute(const Settings &settings) {
    const Settings settingsToUse = settings;
    int w = getIterationBufferWidth(settingsToUse);
    int h = getIterationBufferHeight(settingsToUse);

    if (referenceRenderState.interruptRequested()) return;

    auto &calc = settingsToUse.calculationSettings;
    iterationMatrix = std::make_unique<Matrix<double> >(w, h);

    float logZoom = calc.logZoom;
    int exp10 = Perturbator::logZoomToExp10(logZoom);

    if (referenceRenderState.interruptRequested()) return;

    setStatusMessage(RFF::Status::ZOOM_STATUS,
                     std::format("Zoom : {:.06f}E{:d}", pow(10, fmod(logZoom, 1)), static_cast<int>(logZoom)));

    const std::array<double_exp, 2> offset = offsetConversion(settingsToUse, 0, 0);
    double_exp dcMax = double_exp::DEX_ZERO;
    dex_trigonometric::hypot_approx(&dcMax, offset[0], offset[1]);

    const auto refreshInterval = RFFUtilities::getRefreshInterval(logZoom);
    std::function actionPerRefCalcIteration = [refreshInterval, this](const uint64_t p) {
        if (p % refreshInterval == 0) {
            setStatusMessage(RFF::Status::RENDER_STATUS, std::format(std::locale(), "Period {:L}", p));
        }
    };
    std::function actionPerCreatingTableIteration = [refreshInterval, this](const uint64_t p, const double i) {
        if (p % refreshInterval == 0) {
            setStatusMessage(RFF::Status::RENDER_STATUS, std::format("Creating Table... {:.3f}%", i * 100));
        }
    };


    if (referenceRenderState.interruptRequested()) return;
    auto start = std::chrono::high_resolution_clock::now();

    switch (calc.reuseReferenceMethod) {
            using enum ReuseReferenceMethod;
        case CURRENT_REFERENCE: {
            if (auto p = dynamic_cast<LightMandelbrotPerturbator *>(currentPerturbator.get())) {
                currentPerturbator = p->reuse(calc, static_cast<double>(currentPerturbator->getDcMaxExp()), exp10);
            }
            if (auto p = dynamic_cast<DeepMandelbrotPerturbator *>(currentPerturbator.get())) {
                currentPerturbator = p->reuse(calc, currentPerturbator->getDcMaxExp(), exp10);
            }
            break;
        }
        // case CENTERED_REFERENCE: {
        //     uint64_t period = currentPerturbator->getReference().longestPeriod();
        //     MandelbrotLocator center = MandelbrotLocator.locateMinibrot(state, currentID, currentPerturbator,
        //                                                                 ActionsExplore.
        //                                                                 getActionWhileFindingMinibrotCenter(
        //                                                                     panel, logZoom, period),
        //                                                                 ActionsExplore.getActionWhileCreatingTable(
        //                                                                     panel, logZoom),
        //                                                                 ActionsExplore.
        //                                                                 getActionWhileFindingMinibrotZoom(
        //                                                                     panel)
        //     );
        //     currentPerturbator = null;
        //
        //     CalculationSettings refCalc = calc.edit().setCenter(center.center()).setLogZoom(center.logZoom()).build();
        //     int refPrecision = Perturbator.precision(center.logZoom());
        //
        //     if (refCalc.logZoom() > DoubleExponent.EXP_DEADLINE) {
        //         currentPerturbator = new DeepMandelbrotPerturbator(state, currentID, refCalc, center.dcMax(),
        //                                                            refPrecision,
        //                                                            period, actionPerRefCalcIteration,
        //                                                            actionPerCreatingTableIteration)
        //         .
        //         reuse(state, currentID, calc, dcMax, precision);
        //     } else {
        //         currentPerturbator = new LightMandelbrotPerturbator(state, currentID, refCalc,
        //                                                             center.dcMax().doubleValue(),
        //                                                             refPrecision, period, actionPerRefCalcIteration,
        //                                                             actionPerCreatingTableIteration)
        //         .
        //         reuse(state, currentID, calc, dcMax, precision);
        //     }
        // break;
        // }
        case DISABLED: {
            if (logZoom > RFF::Render::ZOOM_DEADLINE) {
                auto p = dynamic_cast<DeepMandelbrotPerturbator *>(currentPerturbator.get());

                auto vector = p == nullptr
                                  ? std::vector<std::vector<DeepPA> >()
                                  : std::move(p->getTable().extractVector());
                currentPerturbator = std::make_unique<DeepMandelbrotPerturbator>(
                    referenceRenderState, calc, dcMax, exp10,
                    0, std::move(vector), std::move(actionPerRefCalcIteration),
                    std::move(actionPerCreatingTableIteration));
            } else {
                auto p = dynamic_cast<LightMandelbrotPerturbator *>(currentPerturbator.get());
                auto vector = p == nullptr
                                  ? std::vector<std::vector<LightPA> >()
                                  : std::move(p->getTable().extractVector());
                currentPerturbator = std::make_unique<LightMandelbrotPerturbator>(
                    referenceRenderState, calc, static_cast<double>(dcMax), exp10,
                    0, std::move(vector), std::move(actionPerRefCalcIteration),
                    std::move(actionPerCreatingTableIteration));
            }
            break;
        }
        default: {
            //noop
        }
    }

    const MandelbrotReference *reference = currentPerturbator->getReference();
    if (reference == RFF::NullPointer::PROCESS_TERMINATED_REFERENCE || referenceRenderState.interruptRequested())
        return;


    lastPeriod = reference->longestPeriod();
    size_t length = reference->length();

    setStatusMessage(RFF::Status::PERIOD_STATUS, std::format("Period : {:L}({:L})", lastPeriod, length));
    setStatusMessage(RFF::Status::RENDER_STATUS, "Preparing...");


    if (referenceRenderState.interruptRequested()) return;

    std::atomic renderPixelsCount = 0;

    auto generator = ParallelArrayDispatcher<double>(
        referenceRenderState, *iterationMatrix,
        [settingsToUse, this, &renderPixelsCount](const int x, const int y, const int xRes, int, float, float, int,
                                                  double) {
            const auto dc = offsetConversion(settingsToUse, x, y);
            const double iteration = currentPerturbator->iterate(dc[0], dc[1]);
            rendererIteration->setIteration(x, y, iteration);

            if (x == xRes - 1 && !canDisplayed) {
                canDisplayed = true;
            }
            ++renderPixelsCount;
            return iteration;
        });

    auto statusThread = std::jthread([&renderPixelsCount, w, h, this, &start](const std::stop_token &stop) {
        while (!stop.stop_requested()) {
            const auto current = std::chrono::system_clock::now();
            const auto elapsed = std::chrono::milliseconds((current - start).count() / 1000000);
            auto hms = std::chrono::hh_mm_ss(elapsed);
            float ratio = static_cast<float>(renderPixelsCount.load()) / static_cast<float>(w * h) * 100;
            setStatusMessage(RFF::Status::TIME_STATUS,
                             std::format("Time : {:02d}:{:02d}:{:02d}:{:03d}", hms.hours().count(),
                                         hms.minutes().count(), hms.seconds().count(), hms.subseconds().count()));
            setStatusMessage(RFF::Status::RENDER_STATUS, std::format("Calculating... {:.3f}%", ratio));

            Sleep(RFF::Status::SET_PROCESS_INTERVAL_MS);
        }
    });

    generator.dispatch();
    statusThread.request_stop();
    statusThread.join();

    currentMap = std::make_unique<RFFMap>(calc.logZoom, lastPeriod, calc.maxIteration, *iterationMatrix);

    setStatusMessage(RFF::Status::RENDER_STATUS, std::string("Done"));
}

void RFFRenderScene::afterCompute() const {
    if (referenceRenderState.interruptRequested()) {
        RFFUtilities::log("Recompute cancelled.");
    }
}


void RFFRenderScene::setStatusMessage(const int index, const std::string_view &message) const {
    (*statusMessageRef)[index] = std::string("  ").append(message);
}

Settings &RFFRenderScene::getSettings() {
    return settings;
}

ParallelRenderState &RFFRenderScene::getState() {
    return referenceRenderState;
}

MandelbrotPerturbator *RFFRenderScene::getCurrentPerturbator() const {
    return currentPerturbator.get();
}

std::unique_ptr<MandelbrotPerturbator> RFFRenderScene::extractCurrentPerturbator() {
    auto ptr = std::move(currentPerturbator);
    currentPerturbator = nullptr;
    return ptr;
}

void RFFRenderScene::setCurrentPerturbator(std::unique_ptr<MandelbrotPerturbator> perturbator) {
    this->currentPerturbator = std::move(perturbator);
}
