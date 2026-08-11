#include "AutoExplorer.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <format>

#include "../formula/Perturbator.h"
#include "RFFApplication.hpp"

namespace merutilm::rff2 {
    void AutoExplorer::start(RFFApplication &app) {
        config.zoomIncrement = std::clamp(config.zoomIncrement, 0.01f, 1000.0f);
        config.stopLogZoom = std::max(config.stopLogZoom, app.getSettings().fractal.general.logZoom);
        config.minimumContrast = std::max(config.minimumContrast, 0.0f);
        config.candidateSamples = std::clamp(config.candidateSamples, 64, 1'000'000);
        config.edgeMarginPercent = std::clamp(config.edgeMarginPercent, 0, 45);
        config.recoveryZoomOut = std::clamp(config.recoveryZoomOut, 0.01f, 1000.0f);
        config.recoveryAvoidRadiusPercent = std::clamp(config.recoveryAvoidRadiusPercent, 0, 45);

        const uint32_t seed = config.seed == 0 ? std::random_device{}() : config.seed;
        random.seed(seed);
        running = true;
        waitingForRender = !app.isIdleCompute();
        lastCompletedRender = app.getCompletedRenderCount();
        stepCount = 0;
        recoveryCount = 0;
        consecutiveRecoveryCount = 0;
        recoveryInProgress = false;
        avoidCandidate = false;
        status = waitingForRender ? "Waiting for the current render" : "Selecting a boundary pixel";
    }

    void AutoExplorer::stop() {
        running = false;
        waitingForRender = false;
        recoveryInProgress = false;
        status = "Stopped";
    }

    void AutoExplorer::update(RFFApplication &app) {
        if (!running)
            return;

        if (waitingForRender) {
            if (!app.isIdleCompute()) {
                status = recoveryInProgress
                        ? std::format("Recovery {}: rendering a wider view", consecutiveRecoveryCount)
                        : "Rendering the next view";
                return;
            }
            if (app.getCompletedRenderCount() == lastCompletedRender)
                return;
            waitingForRender = false;
        }

        if (app.getSettings().fractal.general.logZoom >= config.stopLogZoom) {
            running = false;
            status = "Target depth reached";
            return;
        }

        if (!advance(app))
            running = false;
    }

    AutoExplorer::Candidate AutoExplorer::findCandidate(const RFFApplication &app) {
        const Matrix<double> *matrix = app.getIterationMatrix();
        if (!matrix || matrix->getWidth() < 3 || matrix->getHeight() < 3)
            return {};

        const int width = matrix->getWidth();
        const int height = matrix->getHeight();
        const int marginX = std::clamp(width * config.edgeMarginPercent / 100, 1, (width - 1) / 2);
        const int marginY = std::clamp(height * config.edgeMarginPercent / 100, 1, (height - 1) / 2);
        std::uniform_int_distribution<int> xDistribution(marginX, width - marginX - 1);
        std::uniform_int_distribution<int> yDistribution(marginY, height - marginY - 1);

        Candidate best = {};
        constexpr std::array<std::array<int, 2>, 8> NEIGHBORS = {{
                {{-1, -1}},
                {{0, -1}},
                {{1, -1}},
                {{-1, 0}},
                {{1, 0}},
                {{-1, 1}},
                {{0, 1}},
                {{1, 1}},
        }};

        for (int sample = 0; sample < config.candidateSamples; ++sample) {
            const auto x = static_cast<uint16_t>(xDistribution(random));
            const auto y = static_cast<uint16_t>(yDistribution(random));
            if (avoidCandidate) {
                const float dx = static_cast<float>(x) / static_cast<float>(width) - avoidXRatio;
                const float dy = static_cast<float>(y) / static_cast<float>(height) - avoidYRatio;
                const float radius = static_cast<float>(config.recoveryAvoidRadiusPercent) / 100.0f;
                if (dx * dx + dy * dy < radius * radius)
                    continue;
            }
            double localMin = (*matrix)(x, y);
            double localMax = localMin;
            for (const auto &offset: NEIGHBORS) {
                const double value =
                        (*matrix)(static_cast<uint16_t>(x + offset[0]), static_cast<uint16_t>(y + offset[1]));
                localMin = std::min(localMin, value);
                localMax = std::max(localMax, value);
            }

            const double contrast = localMax - localMin;
            if (contrast > best.contrast)
                best = {x, y, contrast, false};
            if (contrast >= config.minimumContrast)
                return {x, y, contrast, true};
        }

        return best;
    }

    bool AutoExplorer::advance(RFFApplication &app) {
        const Candidate candidate = findCandidate(app);
        if (!(candidate.contrast > 0)) {
            if (config.recoverWhenStuck)
                return recover(app, candidate, "no contrasting boundary");
            status = "No contrasting boundary pixel found";
            return false;
        }

        if (!candidate.meetsThreshold) {
            if (config.recoverWhenStuck &&
                app.getSettings().fractal.general.logZoom > Constants::Fractal::ZOOM_MIN) {
                return recover(app, candidate, "boundary contrast is too low");
            }
            if (!config.useBestFallback) {
                status = "No boundary pixel met the contrast threshold";
                return false;
            }
        }

        Settings &settings = app.getSettings();
        const float remaining = config.stopLogZoom - settings.fractal.general.logZoom;
        const float increment = std::min(config.zoomIncrement, remaining);
        if (!(increment > 0)) {
            status = "Target depth reached";
            return false;
        }

        const complex<dex> offset = app.offsetConversion(settings, candidate.x, candidate.y);
        const dex centerFraction = dex(1.0 - std::pow(10.0, -increment));
        const int newExp10 = Perturbator::logZoomToExp10(settings.fractal.general.logZoom + increment);
        fixed_point_complex_i1 center = settings.fractal.reference.center.create_variant(newExp10);
        const fixed_point_complex_i1 delta(offset.re * centerFraction, offset.im * centerFraction, newExp10);
        fixed_point_complex_i1::add(center, center, delta);

        settings.fractal.reference.center = center;
        settings.fractal.general.logZoom += increment;
        settings.fractal.reference.reuse = false;
        ++stepCount;
        consecutiveRecoveryCount = 0;
        recoveryInProgress = false;
        avoidCandidate = false;

        status = std::format("Step {}: ({}, {}), contrast {:.1f}", stepCount, candidate.x, candidate.y,
                             candidate.contrast);
        app.getRequests().requestRecompute();
        waitingForRender = true;
        lastCompletedRender = app.getCompletedRenderCount();
        return true;
    }

    bool AutoExplorer::recover(RFFApplication &app, const Candidate &failedCandidate, const std::string_view reason) {
        Settings &settings = app.getSettings();
        const float currentZoom = settings.fractal.general.logZoom;
        const float recoveredZoom = std::max(Constants::Fractal::ZOOM_MIN, currentZoom - config.recoveryZoomOut);
        if (!(recoveredZoom < currentZoom)) {
            status = std::format("Stuck at minimum zoom: {}", reason);
            return false;
        }

        if (const Matrix<double> *matrix = app.getIterationMatrix(); matrix && failedCandidate.contrast > 0) {
            avoidCandidate = true;
            avoidXRatio = static_cast<float>(failedCandidate.x) / static_cast<float>(matrix->getWidth());
            avoidYRatio = static_cast<float>(failedCandidate.y) / static_cast<float>(matrix->getHeight());
        }

        settings.fractal.general.logZoom = recoveredZoom;
        settings.fractal.reference.reuse = false;
        ++recoveryCount;
        ++consecutiveRecoveryCount;
        recoveryInProgress = true;
        status = std::format("Recovery {}: {}; zooming out {:.3f}", consecutiveRecoveryCount, reason,
                             currentZoom - recoveredZoom);
        app.getRequests().requestRecompute();
        waitingForRender = true;
        lastCompletedRender = app.getCompletedRenderCount();
        return true;
    }
} // namespace merutilm::rff2
