//
// Created by Merutilm on 2025-06-08.
//

#include "RFFCallbackVideo.h"

#include "IOUtilities.h"
#include "RFFCallback.h"
#include "RFFVideoWindow.h"


const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackVideo::DATA_SETTINGS = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    auto &[defaultZoomIncrement] = scene.getSettings().videoSettings.dataSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Data Settings");

    window->registerTextInput<float>("Max Iteration", &defaultZoomIncrement,
                                     Unparser::FLOAT,
                                     Parser::FLOAT,
                                     [](const float &v) { return v > 1; },
                                     Callback::NOTHING, "Set Default Zoom increment",
                                     "Set the log-Zoom interval between two adjacent video data.");


    window->setWindowCloseFunction([&settingsMenu] {
        settingsMenu.setCurrentActiveSettingsWindow(nullptr);
    });
    settingsMenu.setCurrentActiveSettingsWindow(std::move(window));
};

const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackVideo::ANIMATION_SETTINGS = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    auto &[overZoom, showText, mps] = scene.getSettings().videoSettings.animationSettings;
    auto window = std::make_unique<RFFSettingsWindow>("Animation Settings");
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
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackVideo::EXPORT_SETTINGS = [
        ](RFFSettingsMenu &settingsMenu, RFFRenderScene &scene) {
    auto window = std::make_unique<RFFSettingsWindow>("Export Settings");
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
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackVideo::GENERATE_VID_KEYFRAME = [
        ](RFFSettingsMenu &, RFFRenderScene &scene) {
    scene.getBackgroundThreads().createThread(
        [&scene](BackgroundThread &thread) {
            const auto &state = scene.getState();
            const auto dirPtr = IOUtilities::ioDirectoryDialog("Folder to generate keyframes");

            float &logZoom = scene.getSettings().calculationSettings.logZoom;
            if (dirPtr == nullptr) {
                return;
            }
            const auto &dir = *dirPtr;

            while (!state.interruptRequested() && logZoom > RFF::Render::ZOOM_MIN) {
                scene.requestRecompute();
                thread.waitUntil([&scene] { return !scene.isRecomputeRequested() && scene.isIdle(); });
                scene.getCurrentMap().exportAsKeyframe(dir);
                logZoom -= std::log10(scene.getSettings().videoSettings.dataSettings.defaultZoomIncrement);
            }

            if (state.interruptRequested()) {
                RFFUtilities::log("Keyframe generation cancelled.");
            }
        });
};
const std::function<void(RFFSettingsMenu &, RFFRenderScene &)> RFFCallbackVideo::EXPORT_ZOOM_VID = [
        ](RFFSettingsMenu &, RFFRenderScene &scene) {
    scene.getBackgroundThreads().createThread([&scene](BackgroundThread &) {

        const auto openPtr = IOUtilities::ioDirectoryDialog("Select Sample Keyframe folder");

        if (openPtr == nullptr) {
            return;
        }
        const auto &open = *openPtr;
        const auto savePtr = IOUtilities::ioFileDialog("Save Video Location", "Video file", IOUtilities::SAVE_FILE,
                                           {RFF::Extension::VIDEO});
        if (savePtr == nullptr) {
            return;
        }
        const auto &save = *savePtr;
        RFFVideoWindow::createVideo(scene.getSettings(), open, save);
    });
};
