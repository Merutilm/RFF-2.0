//
// Created by Merutilm on 2025-09-06.
//

#include "VideoWindow.hpp"

#include "../io/RFFDynamicMapBinary.h"
#include "../io/RFFStaticMapBinary.h"
#include "IOUtilities.h"
#include "Utilities.h"
#include "opencv2/opencv.hpp"
#include "vulkan_helper/engine/window/win/NativeWindow.hpp"

namespace merutilm::rff2 {
    VideoWindow::VideoWindow(vkh::Engine &engine, vkh::SharedResource &sr, const int width, const int height) :
        EngineHandler(engine), sr(sr), width(width), height(height) {
        VideoWindow::init();
    }

    VideoWindow::~VideoWindow() { VideoWindow::cleanup(); }

    void VideoWindow::addListeners() const {
        scene->getWindowContext().getWindow()->eventSystem.applicationLifecycle.onUpdate.add([this] {
            RECT rc;
            GetClientRect(bar, &rc);

            const HDC hdcBar = GetDC(bar);

            const auto pos = barRatio;

            RECT prc = rc;
            prc.right = static_cast<int>(std::lerp(static_cast<float>(prc.left), static_cast<float>(prc.right), pos));

            const HBRUSH pBar = CreateSolidBrush(Constants::Win32::COLOR_PROGRESS_BACKGROUND_PROG);
            FillRect(hdcBar, &prc, pBar);
            DeleteObject(pBar);

            RECT brc = rc;
            brc.left = prc.right;
            const HBRUSH bBar = CreateSolidBrush(Constants::Win32::COLOR_PROGRESS_BACKGROUND_BACK);
            FillRect(hdcBar, &brc, bBar);
            DeleteObject(bBar);

            SetBkMode(hdcBar, TRANSPARENT);

            const HRGN tempRgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
            IntersectClipRect(hdcBar, prc.left, prc.top, prc.right, prc.bottom);

            SetTextColor(hdcBar, Constants::Win32::COLOR_PROGRESS_TEXT_PROG);
            DrawTextW(hdcBar, barText.data(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectClipRgn(hdcBar, tempRgn);
            IntersectClipRect(hdcBar, brc.left, brc.top, brc.right, brc.bottom);

            SetTextColor(hdcBar, Constants::Win32::COLOR_PROGRESS_TEXT_BACK);
            DrawTextW(hdcBar, barText.data(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectClipRgn(hdcBar, nullptr);
            DeleteObject(tempRgn);
            ReleaseDC(bar, hdcBar);
        });
    }

    void VideoWindow::createVideo(vkh::Engine &engine, vkh::SharedResource &sr, const Settings &settings,
                                  const std::filesystem::path &open, const std::filesystem::path &save) {
        int imgWidth = 0;
        int imgHeight = 0;
        HWND wnd = dynamic_cast<vkh::NativeWindow *>(
                           engine.getWindowContext(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).getWindow())
                           ->getMainWindow();
        wnd = IsWindow(wnd) ? wnd : nullptr;

        if (engine.isValidWindowContext(Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX)) {
            MessageBoxW(wnd, L"Video processor already using", L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return;
        }

        if (settings.video.data.isStatic) {
            const RFFStaticMapBinary targetMap = RFFStaticMapBinary::readByID(open, 1);
            if (!targetMap.hasData()) {
                MessageBoxW(wnd, L"Cannot create video. There is no samples in the directory", L"Export failed",
                            MB_TOPMOST | MB_ICONERROR | MB_OK);
                return;
            }

            imgWidth = static_cast<int>(targetMap.getWidth());
            imgHeight = static_cast<int>(targetMap.getHeight());
        } else {
            const RFFDynamicMapBinary targetMap = RFFDynamicMapBinary::readByID(open, 1);
            if (!targetMap.hasData()) {
                MessageBoxW(wnd, L"Cannot create video. There is no samples in the directory", L"Export failed",
                            MB_TOPMOST | MB_ICONERROR | MB_OK);
                return;
            }

            const Matrix<double> &targetMatrix = targetMap.getMatrix();

            imgWidth = targetMatrix.getWidth();
            imgHeight = targetMatrix.getHeight();
        }


        const auto cw = static_cast<uint32_t>(std::min(imgWidth, 1280));
        const auto ch = cw * imgHeight / imgWidth;
        auto window = VideoWindow(engine, sr, cw, ch);
        window.initScene(VkExtent2D{static_cast<uint32_t>(imgWidth), static_cast<uint32_t>(imgHeight)}, settings);
        auto &scene = *window.scene;


        const auto &[defaultZoomIncrement, isStatic] = settings.video.data;
        const auto &[overZoom, showText, mps] = settings.video.animation;
        const auto &[fps, bitrate] = settings.video.exportation;


        cv::VideoWriter writer;
        writer.open(save.string(), cv::CAP_FFMPEG, cv::VideoWriter::fourcc('a', 'v', 'c', '1'), fps,
                    cv::Size(imgWidth, imgHeight));

        if (!writer.isOpened()) {
            MessageBoxW(wnd, L"Cannot open file!!", L"Export failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return;
        }

        std::jthread imageRenderThread([&, imgWidth, imgHeight] {
            auto nwh = window.getNativeWindowHandle();

            const auto frameInterval = mps / fps;
            uint32_t maxNumber;
            if (isStatic) {
                IOUtilities::generateFilename(open, Constants::File::EXT_STATIC_MAP, &maxNumber);
            }else {
                IOUtilities::generateFilename(open, Constants::File::EXT_DYNAMIC_MAP, &maxNumber);
            }
            --maxNumber;

            const float minNumber = -overZoom;
            auto currentFrame = static_cast<float>(maxNumber);
            float currentSec = 0;
            uint32_t pf1 = UINT32_MAX;
            const float startSec = Utilities::getCurrentTime();

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

                if (!IsWindowVisible(nwh)) {
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
                const float spentSec = Utilities::getCurrentTime() - startSec;
                const auto remainedSec = static_cast<uint32_t>((1 - progressRatio) / progressRatio * spentSec);
                window.barRatio = progressRatio;
                window.barText = std::format(L"Processing... {:.2f}% [{}]", std::clamp(progressRatio, 0.0f, 1.0f) * 100, Utilities::formatTime(remainedSec));
            }

            writer.release();

            engine.getCore().getLogicalDevice().waitDeviceIdle();
            MessageBoxW(IsWindow(wnd) ? wnd : nullptr, L"Render Finished!", L"Done",
                        MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
            if (IsWindowVisible(nwh)) {
                PostMessage(nwh, WM_CLOSE, 0, 0);
            }
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
                .name = L"Video Window",
                .icon = L"#1",
                .widthInfo = {.min = width, .max = width, .first = width},
                .heightInfo = {.min = height, .max = height, .first = height},
                .useMenubar = false,
                .resizable = false,
                .filedrop = false,
                .paddings = {.top = 0, .left = 0, .bottom = Constants::Win32::PROGRESS_BAR_HEIGHT, .right = 0}};

        auto &wc = engine.attachWindowContext(std::move(wic), Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX);

        scene = std::make_unique<VideoWindowRenderManager>(engine, wc, sr, videoExtent, targetSettings);
        bar = CreateWindowExW(0, WC_STATICW, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS,
                              0, height, width, Constants::Win32::PROGRESS_BAR_HEIGHT, getNativeWindowHandle(),
                              nullptr, nullptr, nullptr);

        addListeners();
    }


    HWND VideoWindow::getNativeWindowHandle() const {
        return dynamic_cast<vkh::NativeWindow *>(scene->getWindowContext().getWindow())->getMainWindow();
    }

    void VideoWindow::init() {}


    void VideoWindow::cleanup() {
        engine.getCore().getLogicalDevice().waitDeviceIdle();
        const auto nwh = getNativeWindowHandle();
        scene = nullptr;
        engine.detachWindowContext(Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX);
        if (IsWindow(nwh)) {
            DestroyWindow(nwh);
        }
    }
} // namespace merutilm::rff2
