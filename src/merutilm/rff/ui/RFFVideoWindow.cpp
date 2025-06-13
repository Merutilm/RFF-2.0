//
// Created by Merutilm on 2025-06-12.
//


#include "RFFVideoWindow.h"

#include "IOUtilities.h"
#include "../io/RFFMap.h"
#include "opencv2/videoio.hpp"
#include <commctrl.h>

#include "RFFGL.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

RFFVideoWindow::RFFVideoWindow(const uint16_t width, const uint16_t height) : scene(RFFVideoRenderScene()) {
    videoWindow = CreateWindowEx(0,
                                 RFF::Win32::CLASS_VIDEO_WINDOW,
                                 "Preview video",
                                 WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, nullptr, nullptr,
                                 nullptr, nullptr);

    renderWindow = CreateWindowEx(0, RFF::Win32::CLASS_VIDEO_RENDER_WINDOW, "",
                                  WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS, CW_USEDEFAULT,
                                  CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, videoWindow, nullptr, nullptr, nullptr);

    bar = CreateWindowEx(0, RFF::Win32::CLASS_PROGRESS, "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS,
                         CW_USEDEFAULT,
                         CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT, videoWindow, nullptr, nullptr, nullptr);


    SendMessage(bar, PBM_SETRANGE, 0, MAKELPARAM(0, 10000));
    SendMessage(bar, PBM_SETSTEP, 1, 0);
    SetWindowLongPtr(videoWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(bar));

    setClientSize(width, height);
    UpdateWindow(videoWindow);
    ShowWindow(videoWindow, SW_SHOW);
    hdc = GetDC(renderWindow);

    RFFGL::createContext(hdc, &context);
    scene.configure(renderWindow, hdc, context);
}


void RFFVideoWindow::setClientSize(const int width, const int height) const {
    const RECT rect = {0, 0, width, height};
    RECT adjusted = rect;
    AdjustWindowRect(&adjusted, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, false);

    SetWindowPos(videoWindow, nullptr, 0, 0, adjusted.right - adjusted.left,
                 adjusted.bottom - adjusted.top + RFF::Win32::PROGRESS_BAR_HEIGHT, SWP_NOMOVE | SWP_NOZORDER);
    SetWindowPos(renderWindow, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                 SWP_NOZORDER);
    SetWindowPos(bar, nullptr, 0, rect.bottom - rect.top, rect.right - rect.left, RFF::Win32::PROGRESS_BAR_HEIGHT,
                 SWP_NOZORDER);
}

LRESULT CALLBACK RFFVideoWindow::videoWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    const auto bar = reinterpret_cast<HWND>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch (message) {
        case WM_CTLCOLORSTATIC: {
            const auto hdc = reinterpret_cast<HDC>(wParam);
            if (const auto hwndCtl = reinterpret_cast<HWND>(lParam);
                hwndCtl == bar) {
                constexpr COLORREF background = RFF::Win32::COLOR_PROGRESS_BACKGROUND_D;
                SetBkColor(hdc, background);
                SetTextColor(hdc, RFF::Win32::COLOR_PROGRESS_TEXT_D);
                return reinterpret_cast<LRESULT>(CreateSolidBrush(background));
            }
            break;
        }
        default: break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void RFFVideoWindow::createVideo(const Settings &settings, const std::filesystem::path &open,
                                 const std::filesystem::path &save) {
    const auto &[defaultZoomIncrement] = settings.videoSettings.dataSettings;
    const auto &[overZoom, showText, mps] = settings.videoSettings.animationSettings;
    const auto &[fps, bitrate] = settings.videoSettings.exportSettings;

    auto frameInterval = mps / fps;
    RFFMap targetMap = RFFMap::readByID(open, 1);
    if (!targetMap.hasData()) {
        MessageBox(nullptr, "Cannot create video. There is no samples in the directory", "Export failed",
                   MB_ICONERROR | MB_OK);
        return;
    }

    const Matrix<double> &targetMatrix = targetMap.getMatrix();
    uint16_t imgWidth = targetMatrix.getWidth();
    uint16_t imgHeight = targetMatrix.getHeight();
    cv::VideoWriter writer;

    writer.open(save.string(), CV_FOURCC('a', 'v', 'c', '1'), fps, cv::Size(imgWidth, imgHeight));
    if (!writer.isOpened()) {
        MessageBox(nullptr, "Cannot open file!!", "Export failed", MB_ICONERROR | MB_OK);
        return;
    }

    uint32_t maxNumber = IOUtilities::fileNameCount(open, RFF::Extension::MAP);
    double minNumber = -overZoom;
    auto currentFrameNumber = static_cast<float>(maxNumber);
    double currentSec = 0;
    uint16_t cw = std::min(imgWidth, static_cast<uint16_t>(1280));
    auto ch = static_cast<uint16_t>(static_cast<uint32_t>(cw) * imgHeight / imgWidth);


    auto window = RFFVideoWindow(imgWidth, imgHeight);
    window.scene.getRenderer().reloadSize(cw, ch, imgWidth, imgHeight);

    float startSec = RFFUtilities::getTime();

    while (currentFrameNumber > minNumber) {
        currentFrameNumber -= frameInterval;
        currentSec += 1 / fps;
        RFFMap zoomed = RFFMap::DEFAULT_MAP;
        RFFMap normal = RFFMap::DEFAULT_MAP;
        if (currentFrameNumber < 1) {
            normal = RFFMap::readByID(open, 1);
            zoomed = RFFMap::DEFAULT_MAP;
        } else {
            auto f1 = static_cast<uint32_t>(currentFrameNumber);
            uint32_t f2 = f1 + 1;
            zoomed = RFFMap::readByID(open, f1);
            normal = RFFMap::readByID(open, f2);
        }

        if (!IsWindowVisible(window.videoWindow)) {
            break;
        } {
            window.scene.makeContextCurrent();
            window.scene.setMap(currentFrameNumber, std::move(normal), std::move(zoomed));
            window.scene.applyCurrentMap();
            window.scene.applyColor(settings);
            window.scene.getRenderer().setTime((float) currentSec);
            window.scene.renderGL();
        }

        float zoom = window.scene.calculateZoom(defaultZoomIncrement);

        if (showText) {
            // Graphics2D g = window.panel.getImage().createGraphics();
            // int xg = 20;
            // int yg = 10;
            // int size = imgWidth / 40;
            // int off = size / 15;
            // g.setFont(new Font(Font.SERIF, Font.BOLD, size));
            // String zoomStr = String.format("Zoom : %.6fE%d", Math.pow(10, zoom % 1), (int) zoom);
            // g.setColor(new Color(0, 0, 0, 128));
            // g.drawString(zoomStr, xg + off, size + yg + off);
            // g.setColor(new Color(255, 255, 255, 128));
            // g.drawString(zoomStr, xg, size + yg);
        }

        //window.setImage(window.panel.getImage());
        const cv::Mat &img = window.scene.getCurrentImage();
        writer << img;
        // Java2DFrameConverter.copy(window.img, f);
        // recorder.record(f, avutil.AV_PIX_FMT_ABGR);
        // double progressRatio = (maxNumber - currentFrameNumber) / (maxNumber + overZoom());
        // long spent = System.currentTimeMillis() - startSec;
        // long remained = (long) ((1 - progressRatio) / progressRatio * spent);

        // window.panel.repaint();
        // window.bar.setValue((int) (progressRatio * 10000));
        // window.bar.setString(
        //     std::format("Processing... {:2f}% [{}]", progressRatio * 100, sdf.format(new Date(remained))));
    }
    if (IsWindowVisible(window.videoWindow)) {
        CloseWindow(window.videoWindow);
        DestroyWindow(window.videoWindow);
    }
    writer.release();
    MessageBox(nullptr, "Render Finished!", "Success", MB_OK | MB_ICONINFORMATION);
}
