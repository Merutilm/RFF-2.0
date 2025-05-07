//
// Created by Merutilm on 2025-05-07.
//

#pragma once
#include <memory>

#include "../settings/Settings.h"
#include <windows.h>

#include "../value/PointDouble.h"


class RFFRenderer {

    Settings settings;
    bool canMovePosUsingDrag = false;
    int mouseX = 0;
    int mouseY = 0;

public:
    RFFRenderer();

    ~RFFRenderer() = default;
    RFFRenderer(const RFFRenderer&) = delete;
    RFFRenderer& operator=(const RFFRenderer&) = delete;
    RFFRenderer(RFFRenderer&&) noexcept = delete;
    RFFRenderer& operator=(RFFRenderer&&) = delete;

private:

    static Settings initSettings();

    static POINT getScreenMousePosition(HWND hwnd);

public:
    void runAction(HWND hwnd, UINT message, WPARAM wParam);

    PointDouble offsetConversion(HWND wnd, int mx, int my) const;

    double getDivisor() const;

    static int getWidth(HWND wnd);

    static int getHeight(HWND wnd);
};
