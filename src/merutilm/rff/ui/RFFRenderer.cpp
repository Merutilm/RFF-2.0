//
// Created by Merutilm on 2025-05-07.
//

#include "RFFRenderer.h"

#include <cmath>
#include <iostream>

#include "RFFConstants.h"
#include "../calc/Perturbator.h"
#include "../value/PointDouble.h"


RFFRenderer::RFFRenderer() : settings(initSettings()) {
}


Settings RFFRenderer::initSettings() {
    return Settings{
        .calculationSettings = CalculationSettings{
            .center = Center("-0.85", "0", -20),
            .logZoom = 2,
            .maxIteration = 3000,
            .bailout = 2,
            .decimalizeIterationMethod = DecimalizeIterationMethod::LOG_LOG,
            .mpaSettings = MPASettings{
                .minSkipReference = 4,
                .maxMultiplierBetweenLevel = 2,
                .epsilonPower = 4,
                .mpaSelectionMethod = MPASelectionMethod::HIGHEST,
                .mpaCompressionMethod = MPACompressionMethod::NO_COMPRESSION
            },
            .referenceCompressionSettings = ReferenceCompressionSettings{
                .compressCriteria = 0,
                .compressionThresholdPower = 0
            },
            .reuseReferenceMethod = ReuseReferenceMethod::DISABLED,
            .autoIteration = true,
            .absoluteIterationMode = false
        },
        .renderSettings = {},
        .shaderSettings = {},
        .videoSettings = {}
    };
}

POINT RFFRenderer::getScreenMousePosition(const HWND hwnd) {
    POINT mousePos;
    GetCursorPos(&mousePos);
    POINT wndPos;
    ScreenToClient(hwnd, &wndPos);
    return POINT(mousePos.x + wndPos.x, mousePos.y + wndPos.y);
}


void RFFRenderer::runAction(const HWND hwnd, const UINT message, const WPARAM wParam) {
    switch (message) {
        case WM_LBUTTONDOWN: {
            canMovePosUsingDrag = true;
            const auto [x, y] = getScreenMousePosition(hwnd);
            mouseX = x;
            mouseY = y;
            break;
        }
        case WM_LBUTTONUP: {
            canMovePosUsingDrag = false;
            break;
        }
        case WM_MOUSEMOVE: {
            if (canMovePosUsingDrag) {
                const auto [x, y] = getScreenMousePosition(hwnd);
                const int dx = x - mouseX;
                const int dy = y - mouseY;
                const double logZoom = settings.calculationSettings.logZoom;
                settings.calculationSettings.center.addCenterDouble(dx / getDivisor(), dy / getDivisor(), Perturbator::logZoomToExp10(logZoom));
                std::cout << dx << " " << dy << "\n" << std::flush;
            }
            break;
        }
        case WM_MOUSEWHEEL: {
            const int value = GET_WHEEL_DELTA_WPARAM(wParam);
            constexpr float increment = RFFConstants::Render::ZOOM_INTERVAL;
            settings.calculationSettings.logZoom += value > 0 ? increment : -increment;
            settings.calculationSettings.logZoom = std::max(RFFConstants::Render::ZOOM_MIN,
                                                            settings.calculationSettings.logZoom);
            break;
        }
        default: {
            //noop
        }
    }
}


PointDouble RFFRenderer::offsetConversion(const HWND wnd, const int mx, const int my) const {
    return PointDouble(
        (mx - getWidth(wnd) / 2.0) / getDivisor(),
        (my - getHeight(wnd) / 2.0) / getDivisor()
    );
}

double RFFRenderer::getDivisor() const {
    return pow(10, settings.calculationSettings.logZoom);
}

int RFFRenderer::getWidth(const HWND wnd) {
    RECT rect;
    GetWindowRect(wnd, &rect);
    return rect.right - rect.left;
}

int RFFRenderer::getHeight(const HWND wnd) {
    RECT rect;
    GetWindowRect(wnd, &rect);
    return rect.bottom - rect.top;
}
