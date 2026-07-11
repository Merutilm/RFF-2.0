//
// Created by Merutilm on 2025-06-08.
//

#ifdef _WIN32
#include "CallbackVideo.hpp"

#include "../constants/Constants.hpp"
#include "../io/RFFLocationBinary.h"
#include "../io/RFFStaticMapBinary.h"
#include "../preset/shader/bloom/ShdBloomPresets.h"
#include "../preset/shader/fog/ShdFogPresets.h"
#include "../preset/shader/slope/ShdSlopePresets.h"
#include "../preset/shader/stripe/ShdStripePresets.h"
#include "Callback.hpp"
#include "IOUtilities.h"
#include "SettingsWindow.hpp"
#include "VideoWindow.hpp"


namespace merutilm::rff2 {

    std::function<void()> CallbackVideo::fnDataSettings(AppRenderManager &arm) {
        return [&arm] {
            auto &[defaultZoomIncrement, isStatic] = arm.getSettings().video.data;
            auto window = std::make_unique<SettingsWindow>("Data Settings");

            window->registerTextInput<float>(
                    "Default Zoom Increment", &defaultZoomIncrement, Unparser::FLOAT, Parser::FLOAT,
                    [](const float &v) { return v > 1; }, Callback::NOTHING, "Set Default Zoom increment",
                    "Set the w_log-Zoom interval between two adjacent video data.");

            window->registerCheckboxInput(
                    "Static data", &isStatic, Callback::NOTHING, "Use static video data",
                    "Generates using .png image instead of data file. all shaders will be disabled "
                    "when trying to generate video data.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackVideo::fnAnimationSettings(AppRenderManager &arm) {
        return [&arm] {
            auto &[overZoom, showText, mps] = arm.getSettings().video.animation;
            auto window = std::make_unique<SettingsWindow>("Animation Settings");
            window->registerTextInput<float>("Over Zoom", &overZoom, Unparser::FLOAT, Parser::FLOAT,
                                             ValidCondition::POSITIVE_FLOAT_ZERO, Callback::NOTHING, "Over Zoom",
                                             "Zoom the final video data.");
            window->registerCheckboxInput("Show Text", &showText, Callback::NOTHING, "Show Text",
                                          "Show the text on video.");
            window->registerTextInput<float>("Zoom Speed", &mps, Unparser::FLOAT, Parser::FLOAT,
                                             ValidCondition::POSITIVE_FLOAT, Callback::NOTHING, "Set Zoom Speed",
                                             "Sets the zoom speed, Number of Map(.rfm) data used per second in video");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackVideo::fnExportSettings(AppRenderManager &arm) {
        return [&arm] {
            auto window = std::make_unique<SettingsWindow>("Export Settings");
            auto &[fps, bitrate] = arm.getSettings().video.exportation;
            window->registerTextInput<float>("FPS", &fps, Unparser::FLOAT, Parser::FLOAT,
                                             ValidCondition::POSITIVE_FLOAT, Callback::NOTHING, "Set video FPS",
                                             "Set the fps of the video to export.");
            window->registerTextInput<uint32_t>("Bitrate", &bitrate, Unparser::UINT16, Parser::UINT16,
                                                ValidCondition::POSITIVE_UINT16, Callback::NOTHING, "Set the bitrate",
                                                "Sets the bitrate of the video to export.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackVideo::fnGenerateVidKeyframes(AppRenderManager &arm) {
        return [&arm] {
            arm.getBackgroundThreads().createThread([&arm](BackgroundThread &thread) {
                const auto &state = arm.getState();
                const auto dirPtr = IOUtilities::ioDirectoryDialog("Folder to generate keyframes");

                float &logZoom = arm.getSettings().fractal.general.logZoom;
                if (dirPtr == nullptr) {
                    return;
                }

                if (const HWND hwnd =
                            dynamic_cast<vkh::NativeWindow *>(arm.getWindowContext().getWindow())->getMainWindow();
                    !IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
                    MessageBoxW(nullptr, "Target Window already been destroyed", "FATAL", MB_OK | MB_ICONERROR);
                    return;
                }

                const auto &dir = *dirPtr;
                bool nextFrame = false;
                Settings &settings = arm.getSettings();
                const VideoSettings &videoSettings = settings.video;

                if (videoSettings.data.isStatic) {
                    settings.shader.stripe = ShdStripePresets::Disabled().genStripe();
                    settings.shader.slope = ShdSlopePresets::Disabled().genSlope();
                    settings.shader.fog = ShdFogPresets::Disabled().genFog();
                    settings.shader.bloom = BloomPresets::Disabled().genBloom();
                    arm.getRequests().requestShader();
                    thread.waitUntil([&arm] { return !arm.getRequests().shaderRequested; });
                }
                const float increment = std::log10(videoSettings.data.defaultZoomIncrement);
                while (logZoom > Constants::Fractal::ZOOM_MIN) {
                    if (nextFrame || state.interruptRequested()) {
                        // incomplete frame
                        arm.getRequests().requestRecompute();
                        thread.waitUntil([&arm, &state] {
                            return !arm.getRequests().recomputeRequested &&
                                   (state.interruptRequested() || arm.isIdleCompute());
                        });
                    }
                    if (state.interruptRequested()) {
                        vkh::logger::log("Keyframe generation cancelled.");
                        return;
                    }


                    if (videoSettings.data.isStatic) {
                        arm.getRequests().requestCreateImage(IOUtilities::generateFilename(dir, Constants::File::EXT_IMAGE, nullptr).string());
                        thread.waitUntil([&arm] { return !arm.getRequests().createImageRequested; });
                        RFFStaticMapBinary(logZoom, arm.getIterationBufferWidth(settings),
                                           arm.getIterationBufferHeight(settings))
                                .exportAsKeyframe(dir);
                    } else {
                        arm.generateMap().exportAsKeyframe(dir);
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
    std::function<void()> CallbackVideo::fnExportZoomVideo(AppRenderManager &arm) {
        return [&arm] {
            arm.getBackgroundThreads().createThread([&arm](const BackgroundThread &) {
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
                VideoWindow::createVideo(arm.engine, arm.getSharedResource(), arm.getSettings(), open, save);
            });
        };
    }

} // namespace merutilm::rff2
#endif