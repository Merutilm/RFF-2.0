//
// Created by Merutilm on 2025-06-08.
//

#include "CallbackVideo.hpp"

#include "../constants/Constants.hpp"
#include "../io/RFFStaticMapBinary.h"
#include "../preset/shader/bloom/ShdBloomPresets.h"
#include "../preset/shader/fog/ShdFogPresets.h"
#include "../preset/shader/slope/ShdSlopePresets.h"
#include "../preset/shader/stripe/ShdStripePresets.h"
#include "Callback.hpp"
#include "IOUtilities.h"
#include "SettingsWindow.hpp"
#include "VideoWindow.hpp"
#include "vulkan_helper/engine/window/win/NativeWindow.hpp"


namespace merutilm::rff2 {

    std::function<void()> CallbackVideo::fnDataSettings(AppRenderManager &arm) {
        return [&arm] {
            auto &[defaultZoomIncrement, isStatic] = arm.getSettings().video.data;
            auto window = std::make_unique<SettingsWindow>(L"Data Settings");

            window->registerTextInput<float>(
                    L"Default Zoom Increment", &defaultZoomIncrement, Unparser::FLOAT, Parser::FLOAT,
                    [](const float &v) { return v > 1; }, Callback::NOTHING, L"Set Default Zoom increment",
                    L"Set the w_log-Zoom interval between two adjacent video data.");

            window->registerCheckboxInput(
                    L"Static data", &isStatic, Callback::NOTHING, L"Use static video data",
                    L"Generates using .png image instead of data file. all shaders will be disabled "
                    L"when trying to generate video data.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackVideo::fnAnimationSettings(AppRenderManager &arm) {
        return [&arm] {
            auto &[overZoom, showText, mps] = arm.getSettings().video.animation;
            auto window = std::make_unique<SettingsWindow>(L"Animation Settings");
            window->registerTextInput<float>(L"Over Zoom", &overZoom, Unparser::FLOAT, Parser::FLOAT,
                                             ValidCondition::POSITIVE_FLOAT_ZERO, Callback::NOTHING, L"Over Zoom",
                                             L"Zoom the final video data.");
            window->registerCheckboxInput(L"Show Text", &showText, Callback::NOTHING, L"Show Text",
                                          L"Show the text on video.");
            window->registerTextInput<float>(L"Zoom Speed", &mps, Unparser::FLOAT, Parser::FLOAT,
                                             ValidCondition::POSITIVE_FLOAT, Callback::NOTHING, L"Set Zoom Speed",
                                             L"Sets the zoom speed, Number of Map(.rfm) data used per second in video");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackVideo::fnExportSettings(AppRenderManager &arm) {
        return [&arm] {
            auto window = std::make_unique<SettingsWindow>(L"Export Settings");
            auto &[fps, bitrate] = arm.getSettings().video.exportation;
            window->registerTextInput<float>(L"FPS", &fps, Unparser::FLOAT, Parser::FLOAT,
                                             ValidCondition::POSITIVE_FLOAT, Callback::NOTHING, L"Set video FPS",
                                             L"Set the fps of the video to export.");
            window->registerTextInput<uint32_t>(L"Bitrate", &bitrate, Unparser::UINT16, Parser::UINT16,
                                                ValidCondition::POSITIVE_UINT16, Callback::NOTHING, L"Set the bitrate",
                                                L"Sets the bitrate of the video to export.");
            window->setWindowCloseFunction([&arm] { arm.setCurrentSettingsWindows(nullptr); });
            arm.setCurrentSettingsWindows(std::move(window));
        };
    }
    std::function<void()> CallbackVideo::fnGenerateVidKeyframes(AppRenderManager &arm) {
        return [&arm] {
            arm.getBackgroundThreads().createThread([&arm](BackgroundThread &thread) {
                const auto &state = arm.getState();
                const auto dirPtr = IOUtilities::ioDirectoryDialog(L"Folder to generate keyframes");

                float &logZoom = arm.getSettings().fractal.logZoom;
                if (dirPtr == nullptr) {
                    return;
                }

                if (const HWND hwnd = dynamic_cast<vkh::NativeWindow *>(arm.getWindowContext().getWindow())->getMainWindow();
                    !IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
                    MessageBoxW(nullptr, L"Target Window already been destroyed", L"FATAL", MB_OK | MB_ICONERROR);
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
                    if (state.interruptRequested() || nextFrame) {
                        // incomplete frame
                        arm.getRequests().requestRecompute();
                        thread.waitUntil(
                                [&arm] { return !arm.getRequests().recomputeRequested && arm.isIdleCompute(); });
                    }
                    if (state.interruptRequested()) {
                        return;
                    }
                    if (videoSettings.data.isStatic) {
                        const std::string &path =
                                IOUtilities::generateFileName(dir, Constants::Extension::IMAGE).string();
                        arm.getRequests().requestCreateImage(path);
                        thread.waitUntil([&arm] { return !arm.getRequests().createImageRequested; });
                        RFFStaticMapBinary(logZoom, arm.getIterationBufferWidth(settings),
                                           arm.getIterationBufferHeight(settings))
                                .exportAsKeyframe(dir);
                    } else {
                        arm.generateMap().exportAsKeyframe(dir);
                    }
                    logZoom -= increment;
                    nextFrame = true;
                }

                if (state.interruptRequested()) {
                    vkh::logger::w_log(L"Keyframe generation cancelled.");
                }
            });
        };
    }
    std::function<void()> CallbackVideo::fnExportZoomVideo(AppRenderManager &arm) {
        return [&arm] {
            arm.getBackgroundThreads().createThread([&arm](const BackgroundThread &) {
                const auto openPtr = IOUtilities::ioDirectoryDialog(L"Select Sample Keyframe folder");

                if (openPtr == nullptr) {
                    return;
                }
                const auto &open = *openPtr;
                const auto savePtr = IOUtilities::ioFileDialog(L"Save Video Location", Constants::Extension::DESC_VIDEO,
                                                               IOUtilities::SAVE_FILE, Constants::Extension::VIDEO);
                if (savePtr == nullptr) {
                    return;
                }
                const auto &save = *savePtr;
                VideoWindow::createVideo(arm.engine, arm.getSharedResource(), arm.getSettings(), open, save);
            });
        };
    }

} // namespace merutilm::rff2
