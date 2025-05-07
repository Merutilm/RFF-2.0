//
// Created by Merutilm on 2025-05-07.
//
#pragma once
#include <Windows.h>

#include "RFFRenderer.h"
#include "../settings/Settings.h"


class RFFRenderWindow {

    RFFRenderer renderer;

    HWND window;
    HDC hdc;
    HGLRC context;
    bool running = false;

    static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
    RFFRenderWindow();

    ~RFFRenderWindow() = default;

    RFFRenderWindow(const RFFRenderWindow& other) = delete;

    RFFRenderWindow& operator=(const RFFRenderWindow& other) = delete;

    RFFRenderWindow(RFFRenderWindow&& other) noexcept = delete;

    RFFRenderWindow& operator=(RFFRenderWindow&& other) noexcept = delete;

    static void initMenu(HMENU hMenubar);

    static UINT_PTR castMenu(HMENU menu);

    void initWindow();

    void renderLoop();

    void makeContextCurrent() const;

    void swap() const;

    void exit();
};


