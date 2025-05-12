//
// Created by Merutilm on 2025-05-07.
//

#include "RFFRenderScene.h"

#include <chrono>
#include <cmath>
#include <commctrl.h>
#include <format>

#include "glad.h"
#include "RFFConstants.h"
#include "RFFLogger.h"
#include "../approx/ApproxMath.h"
#include "../formula/Perturbator.h"
#include "../parallel/ParallelArrayDispatcher.h"
#include "../value/PointDouble.h"


RFFRenderScene::RFFRenderScene() : referenceRenderState(ParallelRenderState()), settings(initSettings()) {
}


Settings RFFRenderScene::initSettings() {
    return Settings{
        .calculationSettings = CalculationSettings{
            .center = Center("-0.85", "0", Perturbator::logZoomToExp10(2)),
            .logZoom = 2,
            .maxIteration = 1000,
            .bailout = 2,
            .decimalizeIterationMethod = DecimalizeIterationMethod::LOG_LOG,
            .mpaSettings = MPASettings{
                .minSkipReference = 4,
                .maxMultiplierBetweenLevel = 2,
                .epsilonPower = -4,
                .mpaSelectionMethod = MPASelectionMethod::HIGHEST,
                .mpaCompressionMethod = MPACompressionMethod::STRONGEST
            },
            .referenceCompressionSettings = ReferenceCompressionSettings{
                .compressCriteria = 1,
                .compressionThresholdPower = 6,
            },
            .reuseReferenceMethod = ReuseReferenceMethod::DISABLED,
            .autoIteration = true,
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
                .iterationInterval = 1400,
                .offsetRatio = 0,
                .animationSpeed = 0
            },
            .stripeSettings = {
                .stripeType = StripeType::NONE,
                .firstInterval = 1,
                .secondInterval = 1,
                .opacity = 0,
                .offset = 0,
                .animationSpeed = 0
            },
            .slopeSettings = {
                .depth = 0,
                .reflectionRatio = 0,
                .opacity = 0,
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
                .radius = 0,
                .opacity = 0
            },
            .bloomSettings = {
                .threshold = 0,
                .radius = 0,
                .softness = 0,
                .intensity = 0
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
            if (wParam == MK_LBUTTON) {
                SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                const int x = getMouseXOnIterationBuffer();
                const int y = getMouseYOnIterationBuffer();
                const int dx = mouseX - x;
                const int dy = mouseY - y;
                const float logZoom = settings.calculationSettings.logZoom;
                Center &center = settings.calculationSettings.center;
                center = center.addCenterDouble(dx / getDivisor(settings), dy / getDivisor(settings),
                                                Perturbator::logZoomToExp10(logZoom));
                mouseX = x;
                mouseY = y;
                requestRecompute();
            } else {
                SetCursor(LoadCursor(nullptr, IDC_CROSS));
            }
            break;
        }
        case WM_MOUSEWHEEL: {
            const int value = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? 1 : -1;
            constexpr float increment = RFFConstants::Render::ZOOM_INTERVAL;

            settings.calculationSettings.logZoom = std::max(RFFConstants::Render::ZOOM_MIN,
                                                            settings.calculationSettings.logZoom);
            if (value == 1) {
                const PointDouble offset = offsetConversion(settings, getMouseXOnIterationBuffer(),
                                                            getMouseYOnIterationBuffer());
                const double mzi = 1.0 / pow(10, RFFConstants::Render::ZOOM_INTERVAL);
                float &logZoom = settings.calculationSettings.logZoom;
                logZoom += increment;
                settings.calculationSettings.center = settings.calculationSettings.center.addCenterDouble(
                    offset.getX() * (1 - mzi), offset.getY() * (1 - mzi), Perturbator::logZoomToExp10(logZoom));
            }
            if (value == -1) {
                const PointDouble offset = offsetConversion(settings, getMouseXOnIterationBuffer(),
                                                            getMouseYOnIterationBuffer());
                const double mzo = 1.0 / pow(10, -RFFConstants::Render::ZOOM_INTERVAL);
                float &logZoom = settings.calculationSettings.logZoom;
                logZoom -= increment;
                settings.calculationSettings.center = settings.calculationSettings.center.addCenterDouble(
                    offset.getX() * (1 - mzo), offset.getY() * (1 - mzo), Perturbator::logZoomToExp10(logZoom));
            }


            requestRecompute();
            break;
        }
        default: {
            //noop
        }
    }
}


PointDouble RFFRenderScene::offsetConversion(const Settings &settings, const int mx, const int my) const {
    return PointDouble(
        (static_cast<double>(mx) - static_cast<double>(getIterationBufferWidth(settings)) / 2.0) / getDivisor(settings),
        (static_cast<double>(my) - static_cast<double>(getIterationBufferHeight(settings)) / 2.0) / getDivisor(settings)
    );
}

double RFFRenderScene::getDivisor(const Settings &settings) {
    return pow(10, settings.calculationSettings.logZoom);
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
                            std::array<std::string, RFFConstants::Status::LENGTH> *statusMessageRef) {
    RFFScene::configure(wnd, hdc, context);
    this->statusMessageRef = statusMessageRef;

    makeContextCurrent();
    rendererIteration = std::make_unique<GLRendererIteration>();
    rendererIteration->setAsLastFBO();

    requestColor();
    requestResize();
    requestRecompute();
}

void RFFRenderScene::renderGL() {
    if (canBeDisplayed) {
        makeContextCurrent();
        glClear(GL_COLOR_BUFFER_BIT);
        rendererIteration->render();
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

void RFFRenderScene::requestRecompute() {
    canBeDisplayed = false;
    recomputeRequested = true;
}

void RFFRenderScene::applyColor(const Settings &settings) const {
    rendererIteration->setPaletteSettings(&settings.shaderSettings.paletteSettings);
}

void RFFRenderScene::applyResize() const {
    glViewport(0, 0, getClientWidth(), getClientHeight());
    rendererIteration->reloadSize(getClientWidth(), getClientHeight());
}


void RFFRenderScene::applyComputationalSettings() {
    if (settings.calculationSettings.autoIteration) {
        settings.calculationSettings.maxIteration = std::max(RFFConstants::Render::MINIMUM_ITERATION,
                                                             lastPeriod *
                                                             RFFConstants::Render::AUTOMATIC_ITERATION_MULTIPLIER);
    }

    rendererIteration->reloadIterationBuffer(getIterationBufferWidth(settings), getIterationBufferHeight(settings),
                                             settings.calculationSettings.maxIteration);
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
    return static_cast<int>(static_cast<float>(getIterationBufferHeight(settings) - cursor.y) * multiplier);
}


void RFFRenderScene::recompute() {
    referenceRenderState.createThread([this](std::stop_token) {
        compute();
        if (referenceRenderState.interruptRequested()) {
            RFFLogger::log("Recompute cancelled.");
        }
    });
}

void RFFRenderScene::compute() {
    compute0(settings);
}


void RFFRenderScene::compute0(const Settings &settings) {
    const Settings settingsToUse = settings;
    int w = getIterationBufferWidth(settingsToUse);
    int h = getIterationBufferHeight(settingsToUse);

    if (referenceRenderState.interruptRequested()) return;

    auto &calc = settingsToUse.calculationSettings;
    iterationMatrix = std::make_unique<Matrix<double> >(w, h);

    float logZoom = calc.logZoom;
    int precision = Perturbator::logZoomToExp10(logZoom);

    if (referenceRenderState.interruptRequested()) return;

    setStatusMessage(RFFConstants::Status::ZOOM_STATUS,
                     std::format("Zoom : {:.06f}E{:d}", pow(10, fmod(logZoom, 1)), static_cast<int>(logZoom)));

    PointDouble offset = offsetConversion(settingsToUse, 0, 0);
    double dcMax = ApproxMath::hypotApproximate(offset.getX(), offset.getY());

    const auto refreshInterval = static_cast<int>(100000.0 / logZoom);
    std::function actionPerRefCalcIteration = [refreshInterval, this](const uint64_t p) {
        if (p % refreshInterval == 0) {
            setStatusMessage(RFFConstants::Status::RENDER_STATUS, std::format(std::locale(), "Period {:L}", p));
        }
    };
    std::function actionPerCreatingTableIteration = [refreshInterval, this](const uint64_t p, const double i) {
        if (p % refreshInterval == 0) {
            setStatusMessage(RFFConstants::Status::RENDER_STATUS, std::format("Creating Table... {:.2f}%", i * 100));
        }
    };


    if (referenceRenderState.interruptRequested()) return;
    auto start = std::chrono::high_resolution_clock::now();

    switch (calc.reuseReferenceMethod) {
            using enum ReuseReferenceMethod;
        case CURRENT_REFERENCE: {
            currentPerturbator = currentPerturbator->reuse(calc, currentPerturbator->getDcMax(), precision);
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
        //     currentPerturbator = null; //try to call gc
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
            // if (logZoom > DoubleExponent.EXP_DEADLINE) {
            //     currentPerturbator = new DeepMandelbrotPerturbator(state, currentID, calc, dcMax, precision, -1,
            //                                                        actionPerRefCalcIteration,
            //                                                        actionPerCreatingTableIteration);
            // } else {

            auto vector = currentPerturbator == nullptr
                              ? std::vector<std::vector<LightPA>>()
                              : std::move(currentPerturbator->getTable().extractVector());

            currentPerturbator = std::make_unique<LightMandelbrotPerturbator>(
                referenceRenderState, calc, dcMax, precision,
                0, std::move(vector), std::move(actionPerRefCalcIteration),
                std::move(actionPerCreatingTableIteration));
            break;
        }
        default: {
            //noop
        }
    }

    if (referenceRenderState.interruptRequested()) return;

    lastPeriod = currentPerturbator->getReference().longestPeriod();
    size_t length = currentPerturbator->getReference().length();
    //currentMap = new RFFMap(calc.logZoom(), period, calc.maxIteration(), iterations);

    setStatusMessage(RFFConstants::Status::PERIOD_STATUS, std::format("Period : {:L}", lastPeriod));
    setStatusMessage(RFFConstants::Status::RENDER_STATUS, "Preparing...");


    if (referenceRenderState.interruptRequested()) return;

    std::atomic renderPixelsCount = 0;

    auto generator = ParallelArrayDispatcher<double>(
        referenceRenderState, *iterationMatrix,
        [settingsToUse, this, &renderPixelsCount](const int x, const int y, const int xRes, int, float, float, int,
                                                  double) {
            const PointDouble dc = offsetConversion(settingsToUse, x, y);
            const double iteration = currentPerturbator->iterate(dc.getX(), dc.getY());
            rendererIteration->setIteration(x, y, iteration);

            if (x == xRes - 1) {
                canBeDisplayed = true;
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
            setStatusMessage(RFFConstants::Status::TIME_STATUS,
                             std::format("Time : {:02d}:{:02d}:{:02d}:{:03d}", hms.hours().count(),
                                         hms.minutes().count(), hms.seconds().count(), hms.subseconds().count()));
            setStatusMessage(RFFConstants::Status::RENDER_STATUS, std::format("Calculating... {:.3f}%", ratio));

            Sleep(RFFConstants::Status::SET_PROCESS_INTERVAL_MS);
        }
    });

    generator.dispatch();
    statusThread.request_stop();
    statusThread.join();
    setStatusMessage(RFFConstants::Status::RENDER_STATUS, std::string("Done"));
}


void RFFRenderScene::setStatusMessage(const int index, const std::string_view &message) const {
    (*statusMessageRef)[index] = std::string("  ").append(message);
}
