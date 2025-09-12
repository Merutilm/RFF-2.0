//
// Created by Merutilm on 2025-06-08.
//

#include "CallbackVideo.hpp"

#include "../constants/Constants.hpp"
#include "IOUtilities.h"
#include "Callback.hpp"
#include "VideoWindow.hpp"
#include "../io/RFFStaticMapBinary.h"
#include "../preset/shader/bloom/ShdBloomPresets.h"
#include "../preset/shader/fog/ShdFogPresets.h"
#include "../preset/shader/slope/ShdSlopePresets.h"
#include "../preset/shader/stripe/ShdStripePresets.h"


namespace merutilm::rff2 {
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackVideo::DATA_SETTINGS = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[defaultZoomIncrement, isStatic] = scene.getAttribute().video.data;
        auto window = std::make_unique<SettingsWindow>(L"Data Settings");

        window->registerTextInput<float>(L"Default Zoom Increment", &defaultZoomIncrement,
                                         Unparser::FLOAT,
                                         Parser::FLOAT,
                                         [](const float &v) { return v > 1; },
                                         Callback::NOTHING, L"Set Default Zoom increment",
                                         L"Set the w_log-Zoom interval between two adjacent video data.");

        window->registerCheckboxInput(L"Static data", &isStatic, Callback::NOTHING, L"Use static video data",
                                  L"Generates using .png image instead of data file. all shaders will be disabled when trying to generate video data.");

        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };

    const std::function<void(SettingsMenu &, RenderScene &)> CallbackVideo::ANIMATION_SETTINGS = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[overZoom, showText, mps] = scene.getAttribute().video.animation;
        auto window = std::make_unique<SettingsWindow>(L"Animation Settings");
        window->registerTextInput<float>(L"Over Zoom", &overZoom, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::POSITIVE_FLOAT_ZERO, Callback::NOTHING, L"Over Zoom",
                                         L"Zoom the final video data.");
        window->registerCheckboxInput(L"Show Text", &showText, Callback::NOTHING, L"Show Text", L"Show the text on video.");
        window->registerTextInput<float>(L"MPS", &mps, Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                                         Callback::NOTHING, L"MPS",
                                         L"Map per second, Number of video data used per second in video");

        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackVideo::EXPORT_SETTINGS = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto window = std::make_unique<SettingsWindow>(L"Export Settings");
        auto &[fps, bitrate] = scene.getAttribute().video.exportation;
        window->registerTextInput<float>(L"FPS", &fps, Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                                         Callback::NOTHING, L"Set video FPS", L"Set the fps of the video to export.");
        window->registerTextInput<uint32_t>(L"Bitrate", &bitrate, Unparser::U_SHORT, Parser::U_SHORT,
                                            ValidCondition::POSITIVE_U_SHORT, Callback::NOTHING, L"Set the bitrate",
                                            L"Sets the bitrate of the video to export.");

        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackVideo::GENERATE_VID_KEYFRAME = [
            ](const SettingsMenu &, RenderScene &scene) {
        scene.getBackgroundThreads().createThread(
            [&scene](BackgroundThread &thread) {
                const auto &state = scene.getState();
                const auto dirPtr = IOUtilities::ioDirectoryDialog(L"Folder to generate keyframes");

                float &logZoom = scene.getAttribute().fractal.logZoom;
                if (dirPtr == nullptr) {
                    return;
                }

                if (const HWND hwnd = scene.getWindowContext().getWindow().getWindowHandle(); !IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
                    MessageBoxW(nullptr, L"Target Window already been destroyed", L"FATAL", MB_OK | MB_ICONERROR);
                    return;
                }

                const auto &dir = *dirPtr;
                bool nextFrame = false;
                Attribute &settings = scene.getAttribute();
                const VideoAttribute &videoSettings = settings.video;

                if (videoSettings.data.isStatic) {
                    settings.shader.stripe = ShdStripePresets::Disabled().genStripe();
                    settings.shader.slope = ShdSlopePresets::Disabled().genSlope();
                    settings.shader.fog = ShdFogPresets::Disabled().genFog();
                    settings.shader.bloom = BloomPresets::Disabled().genBloom();
                    scene.getRequests().requestShader();
                    thread.waitUntil([&scene] { return !scene.getRequests().shaderRequested; });
                }
                const float increment = std::log10(videoSettings.data.defaultZoomIncrement);
                while (logZoom > Constants::Fractal::ZOOM_MIN) {
                    if (state.interruptRequested() || nextFrame) {
                        //incomplete frame
                        scene.getRequests().requestRecompute();
                        thread.waitUntil([&scene] { return !scene.getRequests().recomputeRequested && scene.isIdleCompute(); });
                    }
                    if (state.interruptRequested()) {
                        return;
                    }
                    if (videoSettings.data.isStatic) {
                        const std::string &path = IOUtilities::generateFileName(dir, Constants::Extension::IMAGE).
                                string();
                        scene.getRequests().requestCreateImage(path);
                        thread.waitUntil([&scene] { return !scene.getRequests().createImageRequested; });
                        RFFStaticMapBinary(logZoom, scene.getIterationBufferWidth(settings), scene.getIterationBufferHeight(settings)).exportAsKeyframe(dir);
                    } else {
                        scene.generateMap().exportAsKeyframe(dir);
                    }
                    logZoom -= increment;
                    nextFrame = true;
                }

                if (state.interruptRequested()) {
                    vkh::logger::w_log(L"Keyframe generation cancelled.");
                }
            });
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackVideo::EXPORT_ZOOM_VID = [
            ](const SettingsMenu &, RenderScene &scene) {
        scene.getBackgroundThreads().createThread([&scene](const BackgroundThread &) {
            const auto openPtr = IOUtilities::ioDirectoryDialog(L"Select Sample Keyframe folder");

            if (openPtr == nullptr) {
                return;
            }
            const auto &open = *openPtr;
            const auto savePtr = IOUtilities::ioFileDialog(L"Save Video Location", Constants::Extension::DESC_VIDEO, IOUtilities::SAVE_FILE,
                                                           Constants::Extension::VIDEO);
            if (savePtr == nullptr) {
                return;
            }
            const auto &save = *savePtr;
            VideoWindow::createVideo(scene.engine, scene.getAttribute(), open, save);
        });
    };
}
