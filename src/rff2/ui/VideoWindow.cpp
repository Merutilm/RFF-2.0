//
// Created by Merutilm on 2025-09-06.
//

#include "VideoWindow.hpp"

#include "IOUtilities.h"
#include "../io/RFFDynamicMapBinary.h"
#include "../io/RFFStaticMapBinary.h"
#include "opencv2/opencv.hpp"

namespace merutilm::rff2 {
    VideoWindow::VideoWindow(vkh::EngineRef engine, const int width,
                             const int height) : EngineHandler(engine), width(width), height(height) {
        VideoWindow::init();
    }

    VideoWindow::~VideoWindow() {
        VideoWindow::destroy();
    }


    void VideoWindow::setClientSize(const int width, const int height) const {
        const RECT rect = {0, 0, width, height};
        RECT adjusted = rect;
        AdjustWindowRect(&adjusted, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, false);

        SetWindowPos(videoWindow, nullptr, 0, 0, adjusted.right - adjusted.left,
                     adjusted.bottom - adjusted.top + Constants::Win32::PROGRESS_BAR_HEIGHT, SWP_NOMOVE | SWP_NOZORDER);
        SetWindowPos(renderWindow, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                     SWP_NOZORDER);
        SetWindowPos(bar, nullptr, 0, rect.bottom - rect.top, rect.right - rect.left,
                     Constants::Win32::PROGRESS_BAR_HEIGHT,
                     SWP_NOZORDER);
    }

    LRESULT VideoWindow::videoWindowProc(const HWND hwnd, const UINT message, const WPARAM wParam,
                                         const LPARAM lParam) {
        const auto &window = *reinterpret_cast<VideoWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        switch (message) {
            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }
            case WM_PAINT: {
                PAINTSTRUCT ps;
                RECT rc;
                GetClientRect(window.bar, &rc);

                const HDC hdcBar = BeginPaint(window.bar, &ps);

                const auto pos = window.barRatio;

                RECT prc = rc;
                prc.right = static_cast<int>(
                    std::lerp(static_cast<float>(prc.left), static_cast<float>(prc.right), pos));

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
                DrawTextW(hdcBar, window.barText.data(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                SelectClipRgn(hdcBar, tempRgn);
                IntersectClipRect(hdcBar, brc.left, brc.top, brc.right, brc.bottom);

                SetTextColor(hdcBar, Constants::Win32::COLOR_PROGRESS_TEXT_BACK);
                DrawTextW(hdcBar, window.barText.data(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                EndPaint(window.bar, &ps);
                SelectClipRgn(hdcBar, nullptr);
                DeleteObject(tempRgn);
                return 0;
            }
            default: break;
        }
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }


    void VideoWindow::createVideo(vkh::EngineRef engine,
                                  const Attribute &attr,
                                  const std::filesystem::path &open,
                                  const std::filesystem::path &save) {
        int imgWidth = 0;
        int imgHeight = 0;
        HWND wnd = engine.getWindowContext(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).getWindow().
                getWindowHandle();
        wnd = IsWindow(wnd) ? wnd : nullptr;

        if (engine.isValidWindowContext(Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX)) {
            MessageBoxW(wnd, L"Video processor already using", L"Error", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return;
        }

        if (attr.video.data.isStatic) {
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
        auto window = VideoWindow(engine, cw, ch);
        window.createScene(VkExtent2D{static_cast<uint32_t>(imgWidth), static_cast<uint32_t>(imgHeight)}, attr);
        auto &scene = *window.scene;
        bool exitFlag = false;


        const auto &[defaultZoomIncrement, isStatic] = attr.video.data;
        const auto &[overZoom, showText, mps] = attr.video.animation;
        const auto &[fps, bitrate] = attr.video.exportation;



        cv::VideoWriter writer;
        writer.open(save.string(), cv::VideoWriter::fourcc('a', 'v', 'c', '1'), fps,
                    cv::Size(imgWidth, imgHeight));

        if (!writer.isOpened()) {
            MessageBoxW(wnd, L"Cannot open file!!", L"Export failed", MB_TOPMOST | MB_ICONERROR | MB_OK);
            return;
        }

        std::jthread queueResolveThread([&, wnd] {
            std::unique_ptr<VideoBufferCache> buffer = nullptr;
            while (!exitFlag || !scene.getQueuedBuffers().empty()) {
                //MUTEX LOCK SCOPE BEGIN
                {
                    std::mutex &mutex = scene.getBufferCachedMutex();
                    std::unique_lock lock(mutex);
                    scene.getBufferCachedCondition().wait(lock, [&scene, &exitFlag] {
                        return !scene.getQueuedBuffers().empty() || exitFlag;
                    });
                    if (exitFlag && scene.getQueuedBuffers().empty()) {
                        buffer = nullptr;
                        break;
                    }
                    buffer = std::move(scene.getQueuedBuffers().front());
                    scene.getQueuedBuffers().pop();
                    scene.getBufferCachedCondition().notify_all();
                }
                //MUTEX LOCK SCOPE END
                auto &img = buffer->image;
                if (showText) {
                    const int xg = std::max(1, imgWidth / 72);
                    const int yg = std::max(1, imgWidth / 192);
                    const int loc = std::max(1, imgWidth / 40);
                    const float size = std::max(1.0f, static_cast<float>(imgWidth) / 800);
                    const int off = std::max(1, loc / 15);
                    const int tkn = std::max(1, off / 2);

                    const std::string zoomStr = std::format("Zoom : {:6f}E{:d}",
                                                            std::pow(10, std::fmod(buffer->zoom, 1)),
                                                            static_cast<int>(buffer->zoom));
                    cv::putText(img, zoomStr, cv::Point(xg + off, loc + yg + off), cv::FONT_HERSHEY_PLAIN, size,
                                cv::Scalar(0, 0, 0));
                    cv::putText(img, zoomStr, cv::Point(xg, loc + yg), cv::FONT_HERSHEY_PLAIN, size,
                                cv::Scalar(255, 255, 255), tkn, cv::LINE_AA);
                }
                writer << img;
            }
            engine.getCore().getLogicalDevice().waitDeviceIdle();
            MessageBoxW(IsWindow(wnd) ? wnd : nullptr, L"Render Finished!", L"Done",
                        MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
        });

        std::jthread imageRenderThread([&, imgWidth, imgHeight] {
            const auto frameInterval = mps / fps;
            const uint32_t maxNumber = isStatic
                                           ? IOUtilities::fileNameCount(open, Constants::Extension::STATIC_MAP)
                                           : IOUtilities::fileNameCount(open, Constants::Extension::DYNAMIC_MAP);
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

                if (!IsWindowVisible(window.videoWindow)) {
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
                scene.queueImage();
                scene.getBufferCachedCondition().notify_all();

                const float progressRatio = (static_cast<float>(maxNumber) - currentFrame) / (
                                                static_cast<float>(maxNumber) + overZoom);
                const float spentSec = Utilities::getCurrentTime() - startSec;
                float remainedSec = (1 - progressRatio) / progressRatio * spentSec;
                const auto remainedTime = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::duration<float>(remainedSec));
                auto hms = std::chrono::hh_mm_ss(remainedTime);

                window.barRatio = progressRatio;
                window.barText = std::format(L"Processing... {:2f}% [{:%H:%M:%S}]", progressRatio * 100, hms);
                InvalidateRect(window.videoWindow, nullptr, FALSE);
            }

            exitFlag = true;
            scene.getBufferCachedCondition().notify_all();
            if (queueResolveThread.joinable()) queueResolveThread.join();
            writer.release();
            if (IsWindowVisible(window.videoWindow)) {
                PostMessage(window.videoWindow, WM_CLOSE, 0, 0);
            }
        });
        messageLoop();
    }

    void VideoWindow::messageLoop() {
        MSG msg;

        while (GetMessage(&msg, nullptr, 0, 0) != 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void VideoWindow::init() {
        videoWindow = CreateWindowExW(0,
                                      Constants::Win32::CLASS_VIDEO_WINDOW,
                                      L"Preview video",
                                      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT, nullptr, nullptr,
                                      nullptr, nullptr);

        renderWindow = CreateWindowExW(0, Constants::Win32::CLASS_VIDEO_RENDER_WINDOW, nullptr,
                                       WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS, CW_USEDEFAULT,
                                       CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, videoWindow, nullptr, nullptr,
                                       nullptr);

        bar = CreateWindowExW(0, WC_STATICW, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, videoWindow, nullptr, nullptr, nullptr);

        SetWindowLongPtr(videoWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        SetWindowPos(videoWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        setClientSize(width, height);
        UpdateWindow(videoWindow);
        ShowWindow(videoWindow, SW_SHOW);
    }

    void VideoWindow::createScene(const VkExtent2D &videoExtent, const Attribute &targetAttribute) {
        const auto wc = engine.
                attachWindowContext(renderWindow, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX);
        scene = std::make_unique<VideoRenderScene>(engine, *wc, videoExtent, targetAttribute);
    }

    void VideoWindow::destroy() {
        engine.getCore().getLogicalDevice().waitDeviceIdle();
        scene = nullptr;
        engine.detachWindowContext(Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX);
        if (IsWindow(videoWindow)) {
            DestroyWindow(videoWindow);
        }
    }
}
