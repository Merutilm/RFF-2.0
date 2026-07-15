//
// Created by Merutilm on 2025-09-06.
//
#include "VideoWindow.hpp"

#include "../io/RFFDynamicMapBinary.h"
#include "../io/RFFStaticMapBinary.h"
#include "IOUtilities.h"
#include "Utilities.h"
#include "opencv2/opencv.hpp"

namespace merutilm::rff2 {
    VideoWindow::VideoWindow(RFFApplication &app, const int width, const int height) :
        app(app), width(width), height(height) {
        VideoWindow::init();
    }

    VideoWindow::~VideoWindow() { VideoWindow::cleanup(); }

    void VideoWindow::createVideo(RFFApplication &app, const std::filesystem::path &open,
                                  const std::filesystem::path &save) {
        int imgWidth = 0;
        int imgHeight = 0;

        const bool isWindow = app.rootWindowContext->getWindow()->getWindow();


        if (!isWindow) {
            vkh::logger::log_err("Main window doesn't exist");
            return;
        }

        if (app.engine->isValidWindowContext(Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX)) {
            vkh::logger::log_err("Video processor already using");
            return;
        }

        if (app.getSettings().video.data.isStatic) {
            const RFFStaticMapBinary targetMap = RFFStaticMapBinary::readByID(open, 1);
            if (!targetMap.hasData()) {
                vkh::logger::log_err("Cannot create video. There is no samples in the directory");
                return;
            }

            imgWidth = static_cast<int>(targetMap.getWidth());
            imgHeight = static_cast<int>(targetMap.getHeight());
        } else {
            const RFFDynamicMapBinary targetMap = RFFDynamicMapBinary::readByID(open, 1);
            if (!targetMap.hasData()) {
                vkh::logger::log_err("Cannot create video. There is no samples in the directory");
                return;
            }

            const Matrix<double> &targetMatrix = targetMap.getMatrix();

            imgWidth = targetMatrix.getWidth();
            imgHeight = targetMatrix.getHeight();
        }


        const auto cw = static_cast<uint32_t>(std::min(imgWidth, 1280));
        const auto ch = cw * imgHeight / imgWidth;
        auto window = VideoWindow(app, cw, ch);
        window.initScene(VkExtent2D{static_cast<uint32_t>(imgWidth), static_cast<uint32_t>(imgHeight)},
                         app.getSettings());
        auto &scene = *window.scene;
        GLFWwindow *handle = scene.getWindowContext().getWindow()->getWindow();

        const auto &[defaultZoomIncrement, isStatic] = app.getSettings().video.data;
        const auto &[overZoom, showText, mps] = app.getSettings().video.animation;
        const auto &[fps, bitrate] = app.getSettings().video.exportation;


        cv::VideoWriter writer;
        writer.open(save.string(), cv::CAP_FFMPEG, cv::VideoWriter::fourcc('a', 'v', 'c', '1'), fps,
                    cv::Size(imgWidth, imgHeight));

        if (!writer.isOpened()) {
            vkh::logger::log_err("Cannot open file!!");
            return;
        }

        std::jthread imageRenderThread([&, imgWidth, imgHeight] {
            VideoProgressInfo &vpi = app.getVideoProgressInfo();
            const auto frameInterval = mps / fps;
            uint32_t maxNumber;
            if (isStatic) {
                IOUtilities::generateFilename(open, Constants::File::EXT_STATIC_MAP, &maxNumber);
            } else {
                IOUtilities::generateFilename(open, Constants::File::EXT_DYNAMIC_MAP, &maxNumber);
            }
            --maxNumber;

            const float minNumber = -overZoom;
            auto currentFrame = static_cast<float>(maxNumber);
            float currentSec = 0;
            uint32_t pf1 = UINT32_MAX;
            const float startSec = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

            RFFDynamicMapBinary zoomedDynamic = RFFDynamicMapBinary::DEFAULT;
            RFFDynamicMapBinary normalDynamic = RFFDynamicMapBinary::DEFAULT;
            RFFStaticMapBinary zoomedStatic = RFFStaticMapBinary::DEFAULT;
            RFFStaticMapBinary normalStatic = RFFStaticMapBinary::DEFAULT;
            cv::Mat zoomedStaticImage = cv::Mat::zeros(imgHeight, imgWidth, CV_16UC4);
            cv::Mat normalStaticImage = cv::Mat::zeros(imgHeight, imgWidth, CV_16UC4);

            scene.setStatic(isStatic);

            while (currentFrame > minNumber) {
                currentFrame -= frameInterval;
                currentSec += 1 / fps;
                bool requiredRefresh = false;


                if (currentFrame < 1) {
                    if (0 != pf1) {
                        if (isStatic) {
                            zoomedStatic = RFFStaticMapBinary::DEFAULT;
                            normalStatic = RFFStaticMapBinary::readByID(open, 1);
                            zoomedStaticImage = cv::Mat::zeros(imgHeight, imgWidth, CV_16UC4);
                            normalStaticImage = RFFStaticMapBinary::loadImageByID(open, 1);
                        } else {
                            zoomedDynamic = RFFDynamicMapBinary::DEFAULT;
                            normalDynamic = RFFDynamicMapBinary::readByID(open, 1);
                        }
                        pf1 = 0;
                        requiredRefresh = true;
                    }
                } else {
                    if (const auto f1 = static_cast<uint32_t>(currentFrame); f1 != pf1) {
                        const uint32_t f2 = f1 + 1;
                        if (isStatic) {
                            zoomedStatic = RFFStaticMapBinary::readByID(open, f1);
                            normalStatic = RFFStaticMapBinary::readByID(open, f2);
                            zoomedStaticImage = RFFStaticMapBinary::loadImageByID(open, f1);
                            normalStaticImage = RFFStaticMapBinary::loadImageByID(open, f2);
                        } else {
                            zoomedDynamic = RFFDynamicMapBinary::readByID(open, f1);
                            normalDynamic = RFFDynamicMapBinary::readByID(open, f2);
                        }
                        pf1 = f1;
                        requiredRefresh = true;
                    }
                }

                if (glfwWindowShouldClose(handle)) {
                    break;
                }

                scene.setCurrentFrame(currentFrame);
                if (requiredRefresh) {
                    if (isStatic) {
                        scene.setMap(&normalStatic, &zoomedStatic);
                        scene.applyCurrentStaticImage(normalStaticImage, zoomedStaticImage);
                    } else {
                        scene.setMap(&normalDynamic, &zoomedDynamic);
                        scene.applyCurrentDynamicMap(normalDynamic, zoomedDynamic, currentFrame);
                        scene.setMaxIterationDynamic(static_cast<double>(normalDynamic.getMaxIteration()));
                    }
                }


                scene.setTime(currentSec);
                scene.renderOnce();
                VideoBufferCache buffer = scene.createImage();
                writer << generateFrame(buffer, imgWidth, showText);

                const float progressRatio =
                        (static_cast<float>(maxNumber) - currentFrame) / (static_cast<float>(maxNumber) + overZoom);
                const float spentSec = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() - startSec;
                const auto remainedSec = static_cast<uint32_t>((1 - progressRatio) / progressRatio * spentSec);

                std::scoped_lock lock(vpi.mutex);
                vpi.ratio = progressRatio;
                vpi.remainedTimeStr =
                        std::format("Processing... {:.2f}% [{}]", std::clamp(progressRatio, 0.0f, 1.0f) * 100,
                                    Utilities::formatTime(remainedSec));
            }

            writer.release();

            app.engine->getCore().getLogicalDevice().waitDeviceIdle();
            vkh::logger::log("Render Finished!");

            std::scoped_lock lock(vpi.mutex);
            vpi.ratio = 0;
        });

        scene.getWindowContext().getWindow()->start();
    }


    cv::Mat VideoWindow::generateFrame(const VideoBufferCache &buffer, const int imgWidth, const bool showText) {

        auto &img = buffer.image;
        if (showText) {
            const int xg = std::max(1, imgWidth / 72);
            const int yg = std::max(1, imgWidth / 192);
            const int loc = std::max(1, imgWidth / 40);
            const float size = std::max(1.0f, static_cast<float>(imgWidth) / 800);
            const int off = std::max(1, loc / 15);
            const int tkn = std::max(1, off / 2);

            const std::string zoomStr = std::format("Zoom : {:6f}E{:d}", std::pow(10, std::fmod(buffer.zoom, 1)),
                                                    static_cast<int>(buffer.zoom));
            cv::putText(img, zoomStr, cv::Point(xg + off, loc + yg + off), cv::FONT_HERSHEY_PLAIN, size,
                        cv::Scalar(0, 0, 0));
            cv::putText(img, zoomStr, cv::Point(xg, loc + yg), cv::FONT_HERSHEY_PLAIN, size, cv::Scalar(255, 255, 255),
                        tkn, cv::LINE_AA);
        }
        return img;
    }


    void VideoWindow::initScene(const VkExtent2D &videoExtent, const Settings &targetSettings) {

        vkh::WindowInitializerSettings wic{
                .framerate = 60,
                .name = "Video Window",
                .icon = "../res/icon.png",
                .widthInfo = {.min = 0, .max = INT_MAX, .first = width},
                .heightInfo = {.min = 0, .max = INT_MAX, .first = height},
                .resizable = false,
                .filedrop = false,
        };

        auto &videoWc =
                app.engine->attachWindowContext(std::move(wic), Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX);

        scene = std::make_unique<VideoWindowRenderManager>(*app.engine, videoWc, videoExtent, targetSettings);
    }

    void VideoWindow::init() {}


    void VideoWindow::cleanup() {
        app.engine->getCore().getLogicalDevice().waitDeviceIdle();
        scene = nullptr;
        app.engine->detachWindowContext(Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX);
    }
} // namespace merutilm::rff2
