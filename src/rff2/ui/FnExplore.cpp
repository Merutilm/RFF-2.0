//
// Created by Merutilm on 2025-05-16.
//

#include "FnExplore.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <format>
#include <mutex>

#include <cassert>
#include "Utilities.h"

#include "../constants/Constants.hpp"
#include "../locator/MB2Locator.h"
#include "RFFApplication.hpp"

namespace merutilm::rff2 {

    namespace {
        std::atomic<bool> newtonRunning = false;
        std::atomic<uint64_t> newtonPeriod = 0;
        std::mutex newtonStatusMutex;
        std::string newtonStatus = "Ready. The search starts from the current view.";

        void setNewtonStatus(std::string message) {
            std::scoped_lock lock(newtonStatusMutex);
            newtonStatus = std::move(message);
        }

        std::string getNewtonStatus() {
            std::scoped_lock lock(newtonStatusMutex);
            return newtonStatus;
        }
    } // namespace


    void FnExplore::recompute(RFFApplication &app) {
        if (ImGui::Button("Recompute", ImVec2(-FLT_MIN, 0))) {
            return app.getRequests().requestRecompute();
        }
    };
    void FnExplore::reset(RFFApplication &app) {

        if (ImGui::Button("Reset", ImVec2(-FLT_MIN, 0))) {
            app.getRequests().requestDefaultSettings();
            app.getRequests().requestResize(app.rootWindowContext->getSwapchain().getSwapchainExtent());

            app.getRequests().requestShader();
            app.getRequests().requestRecompute();
        }
    }
    void FnExplore::cancelRender(RFFApplication &app) {
        if (ImGui::Button("Cancel", ImVec2(-FLT_MIN, 0))) {
            app.getState().cancel();
        }
    }
    void FnExplore::locateCenteredReference(RFFApplication &app) {
        if (ImGui::Button("Locate Centered Reference", ImVec2(-FLT_MIN, 0))) {

            ParallelRenderState &state = app.getState();

            state.createThread([&] {
                Settings &settings = app.getSettings();
                float startTime = app.rootWindowContext->getWindow()->getTime();
                std::unique_ptr<MB2RenderDataBase> &data = app.getCurrentRenderDataOwnRef();
                if (!data || !data->getReference() || !data->getPerturbator()) {
                    vkh::logger::log_err("Do not reuse Reference during reference calculation!!!");
                    settings.fractal.reference.reuse = false;
                    return;
                }

                uint64_t period = data->getReference()->longestPeriod();
                const auto center = MB2Locator::locateMinibrot(
                        state, *data, *app.getApproxTableCache(), getActionWhileFindingMBCenter(app, period),
                        getActionWhileSeriesApprox(app), getActionWhileCreatingTable(app),
                        getActionWhileFindingZoom(app));
                if (center == nullptr)
                    return;

                FractalSettings refCalc = settings.fractal;
                refCalc.reference.center = center->data->fractalSettings.reference.center;
                refCalc.general.logZoom = center->data->fractalSettings.general.logZoom;
                int refExp10 = Perturbator::logZoomToExp10(refCalc.general.logZoom);
                if (refCalc.general.logZoom > Constants::Fractal::ZOOM_DEADLINE) {
                    data = std::make_unique<DeepMB2RenderData>(
                            state, refCalc, *app.getApproxTableCache(), center->data->getPerturbator()->dcMax, refExp10,
                            data->getReference()->length(), period, getActionWhileRefCalc(app, startTime),
                            getActionWhileSeriesApprox(app), getActionWhileCreatingTable(app), false);
                } else {
                    data = std::make_unique<LightMB2RenderData>(
                            state, refCalc, *app.getApproxTableCache(), center->data->getPerturbator()->dcMax, refExp10,
                            data->getReference()->length(), period, getActionWhileRefCalc(app, startTime),
                            getActionWhileSeriesApprox(app), getActionWhileCreatingTable(app), false);
                }

                settings.fractal.reference.reuse = true;
                app.getRequests().requestRecompute();
            });
        }
    }

    void FnExplore::locateMinibrot(RFFApplication &app) {
        static bool showNewtonWindow = false;
        if (ImGui::Checkbox("Newton-Raphson Zooming", &showNewtonWindow) && !showNewtonWindow &&
            newtonRunning.load()) {
            setNewtonStatus("Cancellation requested...");
            app.getState().interrupt();
        }
        if (!showNewtonWindow)
            return;

        ImGui::Begin("Newton-Raphson Zooming");

        static int zoomTarget = 0;
        static int action = 2;
        static int foldingPreset = 1;
        static int powerPreset = 3;
        static int sizeFactorPreset = 1;
        static float customFolding = 0.5f;
        static float customPower = 1.0f;
        static float customSizeFactor = 10.0f;
        static float relativeStart = 0.0f;
        static bool relativeStartCaptured = false;

        constexpr const char *TARGETS[] = {"Relative (minibrot)", "Absolute (minibrot)"};
        constexpr const char *ACTIONS[] = {"Period", "Center", "Zoom"};
        constexpr const char *FOLDINGS[] = {"Custom",     "0.5 (2x)",     "0.75 (4x)",
                                            "0.875 (8x)", "0.9375 (16x)", "1.0 (Minibrot)"};
        constexpr float FOLDING_VALUES[] = {0.0f, 0.5f, 0.75f, 0.875f, 0.9375f, 1.0f};
        constexpr const char *POWERS[] = {"Custom", "0.75 (zoomed out)", "0.875", "1.0 (actual size)",
                                          "1.125",  "1.25 (zoomed in)"};
        constexpr float POWER_VALUES[] = {0.0f, 0.75f, 0.875f, 1.0f, 1.125f, 1.25f};
        constexpr const char *SIZE_FACTORS[] = {"Custom", "10 (zoomed out)", "4", "1 (actual size)",
                                                "0.25",   "0.1 (zoomed in)"};
        constexpr float SIZE_FACTOR_VALUES[] = {0.0f, 10.0f, 4.0f, 1.0f, 0.25f, 0.1f};

        ImGui::Combo("Actions", &action, ACTIONS, static_cast<int>(std::size(ACTIONS)));

        if (action == 2) {
            ImGui::Combo("Zoom Target", &zoomTarget, TARGETS, static_cast<int>(std::size(TARGETS)));

            if (zoomTarget == 0) {
                ImGui::Checkbox("Use Captured Relative Start", &relativeStartCaptured);
                ImGui::SameLine();
                if (ImGui::Button("Capture")) {
                    relativeStart = app.getSettings().fractal.general.logZoom;
                    relativeStartCaptured = true;
                }
                if (relativeStartCaptured)
                    ImGui::InputFloat("Relative Start", &relativeStart, 0, 0, "%.6f");
                ImGui::Combo("Relative Fold", &foldingPreset, FOLDINGS, static_cast<int>(std::size(FOLDINGS)));
                if (foldingPreset == 0)
                    ImGui::InputFloat("Custom Fold", &customFolding, 0.01f, 0.1f, "%.6f");
            } else {
                ImGui::Combo("Absolute Power", &powerPreset, POWERS, static_cast<int>(std::size(POWERS)));
                if (powerPreset == 0)
                    ImGui::InputFloat("Custom Power", &customPower, 0.01f, 0.1f, "%.6f");
            }

            ImGui::Combo("Size Factor", &sizeFactorPreset, SIZE_FACTORS, static_cast<int>(std::size(SIZE_FACTORS)));
            if (sizeFactorPreset == 0)
                ImGui::InputFloat("Custom Size Factor", &customSizeFactor, 0.1f, 1.0f, "%.6f");
        }

        ImGui::TextDisabled("Period method: RFF Fast-period-guessing (Power 2)");
        const uint64_t detectedPeriod = newtonPeriod.load();
        if (detectedPeriod > 0)
            ImGui::Text("Detected period: %llu", static_cast<unsigned long long>(detectedPeriod));

        const bool running = newtonRunning.load();
        if (!running) {
            if (ImGui::Button("Run Newton Action", ImVec2(-FLT_MIN, 0))) {
                app.getAutoExplorer().stop();
                Settings &settings = app.getSettings();
                settings.fractal.reference.reuse = false;
                app.getState().cancel();

                const MB2RenderDataBase *data = app.getCurrentRenderData();
                std::unique_ptr<ApproxTableCacheBase> *cache = app.getApproxTableCache();
                const MB2ReferenceBase *reference = data ? data->getReference() : nullptr;
                if (!data || !cache || !*cache || !reference) {
                    setNewtonStatus("A completed fractal render is required.");
                } else {
                    const uint64_t period = reference->longestPeriod();
                    newtonPeriod = period;
                    if (action == 0) {
                        setNewtonStatus(std::format("Period found: {}", period));
                    } else {
                        const float runStart = relativeStartCaptured ? relativeStart : settings.fractal.general.logZoom;
                        const float folding = foldingPreset == 0 ? customFolding : FOLDING_VALUES[foldingPreset];
                        const float power = powerPreset == 0 ? customPower : POWER_VALUES[powerPreset];
                        const float sizeFactor =
                                std::max(0.000001f, sizeFactorPreset == 0 ? customSizeFactor
                                                                          : SIZE_FACTOR_VALUES[sizeFactorPreset]);
                        const int runAction = action;
                        const int runTarget = zoomTarget;
                        newtonRunning = true;
                        setNewtonStatus("Starting Newton center search...");

                        app.getState().createThread([&app, data, cache, &settings, period, runAction, runTarget,
                                                     runStart, folding, power, sizeFactor] {
                            try {
                                auto centerProgress = [&app, period](const uint64_t p, const int pass) {
                                    static float lastUpdate = 0;
                                    const float now = app.getWindowContext().getWindow()->getTime();
                                    if (now - lastUpdate > Constants::Status::UI_REFRESH_INTERVAL) {
                                        lastUpdate = now;
                                        setNewtonStatus(std::format(
                                                "Center pass {}: {:.2f}%", pass,
                                                100.0 * static_cast<double>(p) /
                                                        static_cast<double>(std::max<uint64_t>(1, period))));
                                    }
                                    getActionWhileFindingMBCenter(app, period)(p, pass);
                                };

                                if (runAction == 1) {
                                    std::unique_ptr<MB2RenderDataBase> centered = MB2Locator::locateMinibrotCenter(
                                            app.getState(), *data, *cache, centerProgress,
                                            getActionWhileSeriesApprox(app), getActionWhileCreatingTable(app));
                                    if (!centered) {
                                        setNewtonStatus("Center search cancelled or did not converge.");
                                    } else {
                                        settings.fractal.reference.center = centered->fractalSettings.reference.center;
                                        setNewtonStatus("Center found. Rendering centered view...");
                                        app.getRequests().requestRecompute();
                                    }
                                } else {
                                    std::unique_ptr<MB2Locator> locator = MB2Locator::locateMinibrot(
                                            app.getState(), *data, *cache, centerProgress,
                                            getActionWhileSeriesApprox(app), getActionWhileCreatingTable(app),
                                            [&app](const float zoom) {
                                                setNewtonStatus(std::format("Sizing minibrot: log zoom {:.4f}", zoom));
                                                getActionWhileFindingZoom(app)(zoom);
                                            });
                                    if (!locator) {
                                        setNewtonStatus("Newton zoom cancelled or did not converge.");
                                    } else {
                                        const FractalSettings &result = locator->data->fractalSettings;
                                        const float minibrotZoom = result.general.logZoom;
                                        float targetZoom;
                                        if (runTarget == 0) {
                                            targetZoom =
                                                    std::lerp(runStart, minibrotZoom, folding) - std::log10(sizeFactor);
                                        } else {
                                            targetZoom = power * minibrotZoom +
                                                         (1.0f - power) * Constants::Fractal::ZOOM_MIN -
                                                         std::log10(sizeFactor);
                                        }
                                        settings.fractal.reference.center = result.reference.center;
                                        settings.fractal.general.logZoom =
                                                std::max(Constants::Fractal::ZOOM_MIN, targetZoom);
                                        setNewtonStatus(std::format("Done. Minibrot log zoom {:.4f}; view {:.4f}",
                                                                    minibrotZoom, settings.fractal.general.logZoom));
                                        app.getRequests().requestRecompute();
                                    }
                                }
                            } catch (const std::exception &e) {
                                setNewtonStatus(std::string("Newton search failed: ") + e.what());
                            } catch (...) {
                                setNewtonStatus("Newton search failed with an unknown error.");
                            }
                            newtonRunning = false;
                        });
                    }
                }
            }
        }

        if (running) {
            if (ImGui::Button("Stop Newton", ImVec2(-FLT_MIN, 0))) {
                setNewtonStatus("Cancellation requested...");
                app.getState().interrupt();
            }
        }
        ImGui::TextWrapped("%s", getNewtonStatus().c_str());
        if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
            if (newtonRunning.load()) {
                setNewtonStatus("Cancellation requested...");
                app.getState().interrupt();
            }
            showNewtonWindow = false;
        }
        ImGui::End();
    }

    void FnExplore::autoExplorer(RFFApplication &app) {
        AutoExplorer &explorer = app.getAutoExplorer();
        static bool showAutoExplorerWindow = false;
        if (ImGui::Checkbox("Auto Explorer", &showAutoExplorerWindow) && !showAutoExplorerWindow &&
            explorer.isRunning()) {
            explorer.stop();
        }
        if (!showAutoExplorerWindow)
            return;

        ImGui::Begin("Auto Explorer");

        AutoExplorer::Config &config = explorer.config;
        const bool running = explorer.isRunning();

        ImGui::BeginDisabled(running);
        ImGui::InputFloat("Zoom Increment", &config.zoomIncrement, 0.1f, 1.0f, "%.3f");
        ImGui::InputFloat("Stop Log Zoom", &config.stopLogZoom, 1.0f, 10.0f, "%.3f");
        ImGui::InputFloat("Minimum Iteration Contrast", &config.minimumContrast, 1.0f, 10.0f, "%.1f");
        ImGui::InputInt("Candidate Samples", &config.candidateSamples, 256, 2048);
        ImGui::SliderInt("Edge Margin", &config.edgeMarginPercent, 0, 45, "%d%%");
        ImGui::InputScalar("Random Seed (0 = random)", ImGuiDataType_U32, &config.seed);
        ImGui::Checkbox("Fall back to strongest candidate", &config.useBestFallback);
        ImGui::Checkbox("Recover When Stuck", &config.recoverWhenStuck);
        if (config.recoverWhenStuck) {
            ImGui::InputFloat("Recovery Zoom Out", &config.recoveryZoomOut, 0.1f, 0.5f, "%.3f");
            ImGui::SliderInt("Avoid Failed Region", &config.recoveryAvoidRadiusPercent, 0, 45, "%d%%");
        }
        ImGui::EndDisabled();

        if (!running) {
            if (ImGui::Button("Start Auto Explore", ImVec2(-FLT_MIN, 0)))
                explorer.start(app);
        } else if (ImGui::Button("Stop Auto Explore", ImVec2(-FLT_MIN, 0))) {
            explorer.stop();
        }
        ImGui::Text("Steps: %llu", static_cast<unsigned long long>(explorer.getStepCount()));
        ImGui::Text("Recoveries: %llu", static_cast<unsigned long long>(explorer.getRecoveryCount()));
        ImGui::TextWrapped("%s", explorer.getStatus().c_str());
        if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
            explorer.stop();
            showAutoExplorerWindow = false;
        }
        ImGui::End();
    }

    std::function<void(uint64_t, int)> FnExplore::getActionWhileFindingMBCenter(RFFApplication &app,
                                                                                const uint64_t longestPeriod) {
        return [&app, longestPeriod](const uint64_t p, int i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                                     std::format("Location : {:.3f}%[{}]",
                                                 static_cast<float>(100 * p) / static_cast<float>(longestPeriod), i));
            }
        };
    }

    std::function<void(uint64_t, float)> FnExplore::getActionWhileSeriesApprox(RFFApplication &app) {
        return [&app](const uint64_t, const float i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                                     std::format("Series-Approximation : {:.3f}%", i * 100));
            }
        };
    }


    std::function<void(uint64_t, float)> FnExplore::getActionWhileCreatingTable(RFFApplication &app) {
        return [&app](const uint64_t, const float i) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS,
                                     std::format("MP-Approximation : {:.3f}%", i * 100));
            }
        };
    }


    std::function<void(float)> FnExplore::getActionWhileFindingZoom(RFFApplication &app) {
        return [&app](float zoom) {
            app.setStatusMessage(Constants::Status::RENDER_STATUS, std::format("Zoom : 10^{}", zoom));
        };
    }
    std::function<void(uint64_t)> FnExplore::getActionWhileRefCalc(RFFApplication &app, float startTime) {
        return [&app, startTime](const uint64_t p) {
            static float time = app.rootWindowContext->getWindow()->getTime();
            const float elapsed = app.rootWindowContext->getWindow()->getTime() - time;
            if (elapsed > Constants::Status::UI_REFRESH_INTERVAL) {
                time = app.rootWindowContext->getWindow()->getTime();
                app.setStatusMessage(Constants::Status::RENDER_STATUS, std::format(std::locale(), "Period : {:L}", p));
                app.setStatusMessage(Constants::Status::TIME_STATUS,
                                     std::format("Time : {}", Utilities::formatTime(time - startTime)));
            }
        };
    }
} // namespace merutilm::rff2
