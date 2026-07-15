//
// Created by Merutilm on 2025-06-08.
//

#include "FnVideo.hpp"

#include "../constants/Constants.hpp"
#include "../io/RFFLocationBinary.h"
#include "../io/RFFStaticMapBinary.h"
#include "../preset/shader/bloom/ShdBloomPresets.h"
#include "../preset/shader/fog/ShdFogPresets.h"
#include "../preset/shader/slope/ShdSlopePresets.h"
#include "../preset/shader/stripe/ShdStripePresets.h"
#include "Callback.hpp"
#include "IOUtilities.h"
#include "Utilities.h"
#include "VideoWindow.hpp"


namespace merutilm::rff2 {

    void FnVideo::dataSettings(RFFApplication &app) {
        static bool enabled = false;
        ImGui::Checkbox("Data Settings", &enabled);
        if (enabled) {
            auto &[defaultZoomIncrement, isStatic] = app.getSettings().video.data;
            ImGui::Begin("Data Settings");
            if (ImGui::InputFloat("Default Zoom Increment", &defaultZoomIncrement)) {
                defaultZoomIncrement = std::clamp(defaultZoomIncrement, 1.25f, 8.f);
            }

            Utilities::imguiHelpMarker("Set the log-Zoom interval between two adjacent video keyframes.");

            ImGui::Checkbox("Static data", &isStatic);
            Utilities::imguiHelpMarker("Generates using .png image instead of data file. all shaders will be disabled "
                    "when trying to generate video data.");
            ImGui::End();
        }
    }
    void FnVideo::animationSettings(RFFApplication &app) {
        static bool enabled = false;
        ImGui::Checkbox("Animation Settings", &enabled);
        if (enabled) {
            ImGui::Begin("Animation Settings");
            auto &[overZoom, showText, mps] = app.getSettings().video.animation;
            ImGui::InputFloat("Over Zoom", &overZoom);
            Utilities::imguiHelpMarker("Zoom the final video data.");

            ImGui::Checkbox("Show Text", &showText);
            Utilities::imguiHelpMarker("Show the text on video.");

            ImGui::InputFloat("Zoom Speed", &mps);
            Utilities::imguiHelpMarker("Sets the zoom speed, Number of Map(.rfm) data used per second in video");

            ImGui::End();
        }
    }
    void FnVideo::exportSettings(RFFApplication &app) {
        static bool enabled = false;
        ImGui::Checkbox("Export Settings", &enabled);
        if (enabled) {
            ImGui::Begin("Export Settings");
            auto &[fps, bitrate] = app.getSettings().video.exportation;
            ImGui::InputFloat("FPS", &fps);
            Utilities::imguiHelpMarker("Set the fps of the video to export.");
            ImGui::InputScalar("Bitrate", ImGuiDataType_U16, &bitrate);
            Utilities::imguiHelpMarker("Sets the bitrate of the video to export.");

            ImGui::End();
        }

    }
    void FnVideo::generateVidKeyframes(RFFApplication &app) {
        if (ImGui::Button("Generate Video Keyframes", ImVec2(-FLT_MIN, 0))) {
            app.getBackgroundThreads().createThread([&app](BackgroundThread &thread) {
                const auto &state = app.getState();
                const auto dirPtr = IOUtilities::ioDirectoryDialog("Folder to generate keyframes");

                float &logZoom = app.getSettings().fractal.general.logZoom;
                if (dirPtr == nullptr) {
                    return;
                }

                if (!app.getWindowContext().getWindow()->canRenderNow()) {
                    vkh::logger::log_err("Window not found");
                    return;
                }

                const auto &dir = *dirPtr;
                bool nextFrame = false;
                Settings &settings = app.getSettings();
                const VideoSettings &videoSettings = settings.video;

                if (videoSettings.data.isStatic) {
                    settings.shader.stripe = ShdStripePresets::Disabled().genStripe();
                    settings.shader.slope = ShdSlopePresets::Disabled().genSlope();
                    settings.shader.fog = ShdFogPresets::Disabled().genFog();
                    settings.shader.bloom = BloomPresets::Disabled().genBloom();
                    app.getRequests().requestShader();
                    thread.waitUntil([&app] { return !app.getRequests().shaderRequested; });
                }
                const float increment = std::log10(videoSettings.data.defaultZoomIncrement);
                while (logZoom > Constants::Fractal::ZOOM_MIN) {
                    if (nextFrame || state.interruptRequested()) {
                        // incomplete frame
                        app.getRequests().requestRecompute();
                        thread.waitUntil([&app, &state] {
                            return !app.getRequests().recomputeRequested &&
                                   (state.interruptRequested() || app.isIdleCompute());
                        });
                    }
                    if (state.interruptRequested()) {
                        vkh::logger::log("Keyframe generation cancelled.");
                        return;
                    }


                    if (videoSettings.data.isStatic) {
                        app.getRequests().requestCreateImage(IOUtilities::generateFilename(dir, Constants::File::EXT_IMAGE, nullptr).string());
                        thread.waitUntil([&app] { return !app.getRequests().createImageRequested; });
                        RFFStaticMapBinary(logZoom, app.getIterationBufferWidth(),
                                           app.getIterationBufferHeight())
                                .exportAsKeyframe(dir);
                    } else {
                        app.generateMap().exportAsKeyframe(dir);
                    }

                    auto &center = settings.fractal.reference.center;
                    RFFLocationBinary(settings.fractal.general.logZoom, center.real.to_string(),
                                      center.imag.to_string(), settings.fractal.perturb.maxIteration)
                            .exportFile(IOUtilities::generateFilename(dir, Constants::File::EXT_LOCATION, nullptr).string());
                    logZoom -= increment;
                    nextFrame = true;
                }
            });
        };
    }
    void FnVideo::exportZoomVideo(RFFApplication &app) {
        if (ImGui::Button("Export Zooming Video", ImVec2(-FLT_MIN, 0))) {
            app.getBackgroundThreads().createThread([&app](const BackgroundThread &) {
                const auto openPtr = IOUtilities::ioDirectoryDialog("Select Sample Keyframe folder");

                if (openPtr == nullptr) {
                    return;
                }
                const auto &open = *openPtr;
                const auto savePtr = IOUtilities::ioFileDialog(Constants::File::DESC_VIDEO, IOUtilities::SAVE_FILE,
                                                               Constants::File::EXT_VIDEO);
                if (savePtr == nullptr) {
                    return;
                }
                const auto &save = *savePtr;
                VideoWindow::createVideo(app, open, save);
            });
        }

        auto &[mutex, ratio, remainedTimeStr] = app.getVideoProgressInfo();
        if (ratio > 0)
        {
            std::scoped_lock lock(mutex);
            ImGui::ProgressBar(ratio);
            ImGui::Text("%s",  remainedTimeStr.data());
        }
    }

} // namespace merutilm::rff2