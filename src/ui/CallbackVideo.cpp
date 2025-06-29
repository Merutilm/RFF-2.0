//
// Created by Merutilm on 2025-06-08.
//

#include "CallbackVideo.h"

#include "Constants.h"
#include "IOUtilities.h"
#include "Callback.h"
#include "VideoWindow.h"
#include "../io/RFFStaticMapBinary.h"
#include "../preset/shader/bloom/BloomPresets.h"
#include "../preset/shader/fog/FogPresets.h"
#include "../preset/shader/slope/SlopePresets.h"
#include "../preset/shader/stripe/StripePresets.h"


namespace merutilm::rff2 {
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackVideo::DATA_SETTINGS = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[defaultZoomIncrement, isStatic] = scene.getSettings().videoSettings.dataSettings;
        auto window = std::make_unique<SettingsWindow>("Data Settings");

        window->registerTextInput<float>("Default Zoom Increment", &defaultZoomIncrement,
                                         Unparser::FLOAT,
                                         Parser::FLOAT,
                                         [](const float &v) { return v > 1; },
                                         Callback::NOTHING, "Set Default Zoom increment",
                                         "Set the log-Zoom interval between two adjacent video data.");

        window->registerBoolInput("Static data", &isStatic, Callback::NOTHING, "Use static video data",
                                  "Generates using .png image instead of data file. all shaders will be disabled when trying to generate video data.");

        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };

    const std::function<void(SettingsMenu &, RenderScene &)> CallbackVideo::ANIMATION_SETTINGS = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto &[overZoom, showText, mps] = scene.getSettings().videoSettings.animationSettings;
        auto window = std::make_unique<SettingsWindow>("Animation Settings");
        window->registerTextInput<float>("Over Zoom", &overZoom, Unparser::FLOAT, Parser::FLOAT,
                                         ValidCondition::POSITIVE_FLOAT_ZERO, Callback::NOTHING, "Over Zoom",
                                         "Zoom the final video data.");
        window->registerBoolInput("Show Text", &showText, Callback::NOTHING, "Show Text", "Show the text on video.");
        window->registerTextInput<float>("MPS", &mps, Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                                         Callback::NOTHING, "MPS",
                                         "Map per second, Number of video data used per second in video");


        window->setWindowCloseFunction([&settingsMenu] {
            settingsMenu.setCurrentActiveSettingsWindow(nullptr);
        });
        settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackVideo::EXPORT_SETTINGS = [
            ](SettingsMenu &settingsMenu, RenderScene &scene) {
        auto window = std::make_unique<SettingsWindow>("Export Settings");
        auto &[fps, bitrate] = scene.getSettings().videoSettings.exportSettings;
        window->registerTextInput<float>("FPS", &fps, Unparser::FLOAT, Parser::FLOAT, ValidCondition::POSITIVE_FLOAT,
                                         Callback::NOTHING, "Set video FPS", "Set the fps of the video to export.");
        window->registerTextInput<uint32_t>("Bitrate", &bitrate, Unparser::U_SHORT, Parser::U_SHORT,
                                            ValidCondition::POSITIVE_U_SHORT, Callback::NOTHING, "Set the bitrate",
                                            "Sets the bitrate of the video to export.");

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
                const auto dirPtr = IOUtilities::ioDirectoryDialog("Folder to generate keyframes");

                float &logZoom = scene.getSettings().calculationSettings.logZoom;
                if (dirPtr == nullptr) {
                    return;
                }
                if (!IsWindow(scene.getRenderWindow()) || !IsWindowVisible(scene.getRenderWindow())) {
                    MessageBox(nullptr, "Target Window already been destroyed", "FATAL", MB_OK | MB_ICONERROR);
                    return;
                }

                const auto &dir = *dirPtr;
                bool nextFrame = false;
                Settings &settings = scene.getSettings();
                const VideoSettings &videoSettings = settings.videoSettings;

                if (videoSettings.dataSettings.isStatic) {
                    settings.shaderSettings.stripeSettings = StripePresets::Disabled().stripeSettings();
                    settings.shaderSettings.slopeSettings = SlopePresets::Disabled().slopeSettings();
                    settings.shaderSettings.fogSettings = FogPresets::Disabled().fogSettings();
                    settings.shaderSettings.bloomSettings = BloomPresets::Disabled().bloomSettings();
                    scene.requestColor();
                    thread.waitUntil([&scene] { return !scene.isColorRequested(); });
                }
                const float increment = std::log10(videoSettings.dataSettings.defaultZoomIncrement);
                while (logZoom > Constants::Render::ZOOM_MIN) {
                    if (state.interruptRequested() || nextFrame) {
                        //incomplete frame
                        scene.requestRecompute();
                        thread.waitUntil([&scene] { return !scene.isRecomputeRequested() && scene.isIdleCompute(); });
                    }
                    if (state.interruptRequested()) {
                        return;
                    }
                    if (videoSettings.dataSettings.isStatic) {
                        const std::string &path = IOUtilities::generateFileName(dir, Constants::Extension::IMAGE).
                                string();
                        scene.requestCreateImage(path);
                        thread.waitUntil([&scene] { return !scene.isCreateImageRequested(); });
                        RFFStaticMapBinary(logZoom, scene.getIterationBufferWidth(settings), scene.getIterationBufferHeight(settings)).exportAsKeyframe(dir);
                    } else {
                        scene.getCurrentMap().exportAsKeyframe(dir);
                    }
                    logZoom -= increment;
                    nextFrame = true;
                }

                if (state.interruptRequested()) {
                    Utilities::log("Keyframe generation cancelled.");
                }
            });
    };
    const std::function<void(SettingsMenu &, RenderScene &)> CallbackVideo::EXPORT_ZOOM_VID = [
            ](const SettingsMenu &, RenderScene &scene) {
        scene.getBackgroundThreads().createThread([&scene](const BackgroundThread &) {
            const auto openPtr = IOUtilities::ioDirectoryDialog("Select Sample Keyframe folder");

            if (openPtr == nullptr) {
                return;
            }
            const auto &open = *openPtr;
            const auto savePtr = IOUtilities::ioFileDialog("Save Video Location", Constants::Extension::DESC_VIDEO, IOUtilities::SAVE_FILE,
                                                           Constants::Extension::VIDEO);
            if (savePtr == nullptr) {
                return;
            }
            const auto &save = *savePtr;
            VideoWindow::createVideo(scene.getSettings(), open, save);
        });
    };
}
