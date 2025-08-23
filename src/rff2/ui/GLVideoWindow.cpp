//
// Created by Merutilm on 2025-06-12.
//


#include "GLVideoWindow.h"

#include "IOUtilities.h"
#include "../io/RFFDynamicMapBinary.h"
#include "opencv2/videoio.hpp"
#include <commctrl.h>

#include "../constants/Constants.hpp"
#include "../io/RFFStaticMapBinary.h"
#include "../parallel/BackgroundThreads.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

namespace merutilm::rff2 {
    VideoWindow::VideoWindow(const uint16_t width, const uint16_t height) : scene(GLVideoRenderScene()) {
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

        bar = CreateWindowEx(0, WC_STATIC, nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, videoWindow, nullptr, nullptr, nullptr);


        SetWindowLongPtr(videoWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        SetWindowPos(videoWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        setClientSize(width, height);
        UpdateWindow(videoWindow);
        ShowWindow(videoWindow, SW_SHOW);
        hdc = GetDC(renderWindow);
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
                MessageBox(hwnd, "Render Finished!", "Done", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
                ReleaseDC(window.renderWindow, window.hdc);
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
                DrawText(hdcBar, window.barText.data(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                SelectClipRgn(hdcBar, tempRgn);
                IntersectClipRect(hdcBar, brc.left, brc.top, brc.right, brc.bottom);

                SetTextColor(hdcBar, Constants::Win32::COLOR_PROGRESS_TEXT_BACK);
                DrawText(hdcBar, window.barText.data(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                EndPaint(window.bar, &ps);
                SelectClipRgn(hdcBar, nullptr);
                DeleteObject(tempRgn);
                return 0;
            }
            default: break;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }


    void VideoWindow::createVideo(const Attribute &settings,
                                  const std::filesystem::path &open,
                                  const std::filesystem::path &save) {
        uint16_t imgWidth = 0;
        uint16_t imgHeight = 0;

        if (settings.video.dataAttribute.isStatic) {
            const RFFStaticMapBinary targetMap = RFFStaticMapBinary::readByID(open, 1);
            if (!targetMap.hasData()) {
                MessageBox(nullptr, "Cannot create video. There is no samples in the directory", "Export failed",
                           MB_ICONERROR | MB_OK);
                return;
            }

            imgWidth = targetMap.getWidth();
            imgHeight = targetMap.getHeight();
        }else {
            const RFFDynamicMapBinary targetMap = RFFDynamicMapBinary::readByID(open, 1);
            if (!targetMap.hasData()) {
                MessageBox(nullptr, "Cannot create video. There is no samples in the directory", "Export failed",
                           MB_ICONERROR | MB_OK);
                return;
            }

            const Matrix<double> &targetMatrix = targetMap.getMatrix();

            imgWidth = targetMatrix.getWidth();
            imgHeight = targetMatrix.getHeight();
        }

        cv::VideoWriter writer;

        uint16_t cw = std::min(imgWidth, static_cast<uint16_t>(1280));
        auto ch = static_cast<uint16_t>(static_cast<uint32_t>(cw) * imgHeight / imgWidth);


        auto window = VideoWindow(cw, ch);
        std::jthread thread(
            [&window, &cw, &ch, &imgWidth, &imgHeight, &writer, &settings, &open, &save] {
                // WGLContextLoader::createContext(window.hdc, &window.context);
                window.scene.configure(window.renderWindow, window.hdc, window.context);
                window.scene.makeContextCurrent();
                window.scene.reloadSize(cw, ch, imgWidth, imgHeight);
                window.scene.applyColor(settings);

                const auto &[defaultZoomIncrement, isStatic] = settings.video.dataAttribute;
                const auto &[overZoom, showText, mps] = settings.video.animationAttribute;
                const auto &[fps, bitrate] = settings.video.exportAttribute;

                const auto frameInterval = mps / fps;
                const uint32_t maxNumber = isStatic ? IOUtilities::fileNameCount(open, Constants::Extension::STATIC_MAP) : IOUtilities::fileNameCount(open, Constants::Extension::DYNAMIC_MAP);
                const float minNumber = -overZoom;
                auto currentFrameNumber = static_cast<float>(maxNumber);
                float currentSec = 0;
                uint32_t pf1 = UINT32_MAX;

                writer.open(save.string(), CV_FOURCC('a', 'v', 'c', '1'), fps, cv::Size(imgWidth, imgHeight));
                if (!writer.isOpened()) {
                    MessageBox(nullptr, "Cannot open file!!", "Export failed", MB_ICONERROR | MB_OK);
                    return;
                }
                const float startSec = Utilities::getCurrentTime();


                RFFDynamicMapBinary zoomedDynamic = RFFDynamicMapBinary::DEFAULT;
                RFFDynamicMapBinary normalDynamic = RFFDynamicMapBinary::DEFAULT;
                RFFStaticMapBinary zoomedStatic = RFFStaticMapBinary::DEFAULT;
                RFFStaticMapBinary normalStatic = RFFStaticMapBinary::DEFAULT;
                cv::Mat zoomedStaticImage = cv::Mat::zeros(imgHeight, imgWidth, CV_8UC3);
                cv::Mat normalStaticImage = cv::Mat::zeros(imgHeight, imgWidth, CV_8UC3);


                while (currentFrameNumber > minNumber) {
                    currentFrameNumber -= frameInterval;
                    currentSec += 1 / fps;
                    bool requiredRefresh = false;


                    if (currentFrameNumber < 1) {
                        if (0 != pf1) {
                            if (isStatic) {
                                zoomedStatic = RFFStaticMapBinary::DEFAULT;
                                normalStatic = RFFStaticMapBinary::readByID(open, 1);
                                zoomedStaticImage = cv::Mat::zeros(imgHeight, imgWidth, CV_8UC3);
                                normalStaticImage = RFFStaticMapBinary::loadImageByID(open, 1);
                                cv::flip(normalStaticImage, normalStaticImage, 0);
                            } else {
                                zoomedDynamic = RFFDynamicMapBinary::DEFAULT;
                                normalDynamic = RFFDynamicMapBinary::readByID(open, 1);
                            }
                            pf1 = 0;
                            requiredRefresh = true;
                        }
                    } else {
                        if (const auto f1 = static_cast<uint32_t>(currentFrameNumber); f1 != pf1) {
                            const uint32_t f2 = f1 + 1;
                            if (isStatic) {
                                zoomedStatic = RFFStaticMapBinary::readByID(open, f1);
                                normalStatic = RFFStaticMapBinary::readByID(open, f2);
                                zoomedStaticImage = RFFStaticMapBinary::loadImageByID(open, f1);
                                normalStaticImage = RFFStaticMapBinary::loadImageByID(open, f2);
                                cv::flip(normalStaticImage, normalStaticImage, 0);
                                cv::flip(zoomedStaticImage, zoomedStaticImage, 0);
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

                    cv::Mat img = cv::Mat::zeros(imgHeight, imgWidth, CV_8UC3);
                    window.scene.setStatic(isStatic);
                    window.scene.setCurrentFrame(currentFrameNumber);
                    window.scene.applyCurrentFrame();

                    if (requiredRefresh) {
                        if (isStatic) {
                            window.scene.setMap(&normalStatic, &zoomedStatic);
                            window.scene.applyCurrentStaticImage(normalStaticImage, zoomedStaticImage);
                        } else {
                            window.scene.setMap(&normalDynamic, &zoomedDynamic);
                            window.scene.applyCurrentDynamicMap(normalDynamic, zoomedDynamic);
                        }
                    }
                    const float zoom = window.scene.calculateZoom(defaultZoomIncrement);
                    window.scene.getDynamicRenderer().setTime(currentSec);
                    window.scene.renderGL();
                    img = window.scene.getCurrentImage();

                    if (showText) {
                        const int xg = std::max(1, imgWidth / 72);
                        const int yg = std::max(1, imgWidth / 192);
                        const int loc = std::max(1, imgWidth / 40);
                        const float size = std::max(1.0f, static_cast<float>(imgWidth) / 800);
                        const int off = std::max(1, loc / 15);
                        const int tkn = std::max(1, off / 2);

                        std::string zoomStr = std::format("Zoom : {:6f}E{:d}", std::pow(10, std::fmod(zoom, 1)),
                                                          static_cast<int>(zoom));
                        cv::putText(img, zoomStr, cv::Point(xg + off, loc + yg + off), cv::FONT_HERSHEY_PLAIN, size,
                                    cv::Scalar(0, 0, 0));
                        cv::putText(img, zoomStr, cv::Point(xg, loc + yg), cv::FONT_HERSHEY_PLAIN, size,
                                    cv::Scalar(255, 255, 255), tkn, cv::LINE_AA);
                    }

                    writer << img;

                    const float progressRatio = (static_cast<float>(maxNumber) - currentFrameNumber) / (
                                                    static_cast<float>(maxNumber) + overZoom);
                    const float spentSec = Utilities::getCurrentTime() - startSec;
                    float remainedSec = (1 - progressRatio) / progressRatio * spentSec;
                    const auto remainedTime = std::chrono::duration_cast<std::chrono::seconds>(
                        std::chrono::duration<float>(remainedSec));
                    auto hms = std::chrono::hh_mm_ss(remainedTime);

                    window.barRatio = progressRatio;
                    window.barText = std::format("Processing... {:2f}% [{:%H:%M:%S}]", progressRatio * 100, hms);
                    InvalidateRect(window.videoWindow, nullptr, FALSE);
                }
                writer.release();
                if (IsWindowVisible(window.videoWindow)) {
                    PostMessage(window.videoWindow, WM_CLOSE, 0, 0);
                }
            });
        window.messageLoop();
    }

    void VideoWindow::messageLoop() {
        MSG msg;

        while (GetMessage(&msg, nullptr, 0, 0) != 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
