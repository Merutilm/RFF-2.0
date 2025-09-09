//
// Created by Merutilm on 2025-09-06.
//

#include "VideoWindow.hpp"

#include "IOUtilities.h"
#include "../io/RFFDynamicMapBinary.h"
#include "../io/RFFStaticMapBinary.h"
#include "opencv2/opencv.hpp"

namespace merutilm::rff2 {
    VideoWindow::VideoWindow(vkh::EngineRef engine, const uint32_t width,
                             const uint32_t height) : EngineHandler(engine), width(width), height(height) {
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
                MessageBoxW(window.engine.getWindowContext(Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).getWindow().getWindowHandle(), L"Render Finished!", L"Done", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
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
        uint32_t imgWidth = 0;
        uint32_t imgHeight = 0;

        if (attr.video.dataAttribute.isStatic) {
            const RFFStaticMapBinary targetMap = RFFStaticMapBinary::readByID(open, 1);
            if (!targetMap.hasData()) {
                MessageBoxW(nullptr, L"Cannot create video. There is no samples in the directory", L"Export failed",
                           MB_ICONERROR | MB_OK);
                return;
            }

            imgWidth = targetMap.getWidth();
            imgHeight = targetMap.getHeight();
        } else {
            const RFFDynamicMapBinary targetMap = RFFDynamicMapBinary::readByID(open, 1);
            if (!targetMap.hasData()) {
                MessageBoxW(nullptr, L"Cannot create video. There is no samples in the directory", L"Export failed",
                           MB_ICONERROR | MB_OK);
                return;
            }

            const Matrix<double> &targetMatrix = targetMap.getMatrix();

            imgWidth = targetMatrix.getWidth();
            imgHeight = targetMatrix.getHeight();
        }


        const uint32_t cw = std::min(imgWidth, 1280u);
        const auto ch = cw * imgHeight / imgWidth;
        auto window = VideoWindow(engine, cw, ch);
        window.createScene(VkExtent2D{imgWidth, imgHeight}, attr);

        std::jthread thread(
        [&window, imgWidth, imgHeight, &attr, &open, &save] {
                const auto &[defaultZoomIncrement, isStatic] = attr.video.dataAttribute;
                const auto &[overZoom, showText, mps] = attr.video.animationAttribute;
                const auto &[fps, bitrate] = attr.video.exportAttribute;

                const auto frameInterval = mps / fps;
                const uint32_t maxNumber = isStatic
                                               ? IOUtilities::fileNameCount(open, Constants::Extension::STATIC_MAP)
                                               : IOUtilities::fileNameCount(open, Constants::Extension::DYNAMIC_MAP);
                const float minNumber = -overZoom;
                auto currentFrame = static_cast<float>(maxNumber);
                float currentSec = 0;
                uint32_t pf1 = UINT32_MAX;
                cv::VideoWriter writer;
                writer.open(save.string(), CV_FOURCC('a', 'v', 'c', '1'), fps, cv::Size(imgWidth, imgHeight));

                if (!writer.isOpened()) {
                    MessageBoxW(nullptr, L"Cannot open file!!", L"Export failed", MB_ICONERROR | MB_OK);
                    return;
                }
                const float startSec = Utilities::getCurrentTime();

                RFFDynamicMapBinary zoomedDynamic = RFFDynamicMapBinary::DEFAULT;
                RFFDynamicMapBinary normalDynamic = RFFDynamicMapBinary::DEFAULT;
                RFFStaticMapBinary zoomedStatic = RFFStaticMapBinary::DEFAULT;
                RFFStaticMapBinary normalStatic = RFFStaticMapBinary::DEFAULT;
                cv::Mat zoomedStaticImage = cv::Mat::zeros(imgHeight, imgWidth, CV_16UC4);
                cv::Mat normalStaticImage = cv::Mat::zeros(imgHeight, imgWidth, CV_16UC4);


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

                    cv::Mat img = cv::Mat::zeros(imgHeight, imgWidth, CV_8UC3);
                    window.scene->setStatic(isStatic);
                    window.scene->setCurrentFrame(currentFrame);

                    if (requiredRefresh) {
                        if (isStatic) {
                            window.scene->setMap(&normalStatic, &zoomedStatic);
                            window.scene->applyCurrentStaticImage(normalStaticImage, zoomedStaticImage);
                        } else {
                            window.scene->setMap(&normalDynamic, &zoomedDynamic);
                            window.scene->applyCurrentDynamicMap(normalDynamic, zoomedDynamic);
                        }
                    }
                    window.scene->setInfo(isStatic ? DBL_MAX : static_cast<double>(normalDynamic.getMaxIteration()), currentSec);
                    const float zoom = window.scene->calculateZoom(defaultZoomIncrement);

                    window.scene->renderOnce();
                    img = window.scene->generateImage();

                    if (showText) {
                        const int xg = std::max(1, static_cast<int>(imgWidth / 72));
                        const int yg = std::max(1, static_cast<int>(imgWidth / 192));
                        const int loc = std::max(1, static_cast<int>(imgWidth / 40));
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
        scene = nullptr;
        engine.detachWindowContext(Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX);
    }
}
