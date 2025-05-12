#include <glad_wgl.h>
#include "RFFMasterWindow.h"

#include <iostream>
#include <windows.h>
#include "RFFConstants.h"
#include <assert.h>
#include <commctrl.h>



LRESULT CALLBACK RFFMasterWindow::WndMasterProc(const HWND masterWindow, const UINT message, const WPARAM wParam,
                                          const LPARAM lParam) {

    const auto wnd = reinterpret_cast<RFFMasterWindow *>(GetWindowLongPtr(masterWindow, GWLP_USERDATA));

    switch (message) {
        case WM_SIZE: {
            RECT rect;
            GetClientRect(masterWindow, &rect);
            rect.bottom -= wnd->statusHeight;
            wnd->adjustClient(rect);
            wnd->renderer.requestResize();
            wnd->renderer.requestRecompute();
            return 0;
        }
        case WM_CLOSE:
            wnd->exit();
            return 0;
        default:
            return DefWindowProc(masterWindow, message, wParam, lParam);
    }
}

LRESULT CALLBACK RFFMasterWindow::WndRendererProc(const HWND renderWindow, const UINT message, const WPARAM wParam,
                                          const LPARAM lParam) {
    const auto wnd = reinterpret_cast<RFFMasterWindow *>(GetWindowLongPtr(renderWindow, GWLP_USERDATA));
    wnd->renderer.runAction(message, wParam);
    return DefWindowProc(renderWindow, message, wParam, lParam);
}

RFFMasterWindow::RFFMasterWindow() : renderer(RFFRenderScene()) {
    initWindow();
}

UINT_PTR RFFMasterWindow::castMenu(HMENU menu) {
    return reinterpret_cast<UINT_PTR>(menu);
}

void RFFMasterWindow::initMenu(const HMENU hMenubar) {
    //TODO : create menu class and implement here
    std::string menus[] = {
        "File", "Fractal", "Image", "Shader", "Preset", "Video", "Explore"
    };

    for (const auto &menu: menus) {
        const HMENU hMenu = CreateMenu();
        AppendMenu(hMenubar, MF_POPUP, castMenu(hMenu), menu.c_str());
    }
}


void RFFMasterWindow::initWindow() {


    SetProcessDPIAware();

    const HMENU hMenubar = CreateMenu();
    initMenu(hMenubar);


    createMasterWindow(WS_OVERLAPPEDWINDOW | WS_SYSMENU, hMenubar);
    createRenderWindow();
    createStatusBar();

    statusHeight = 0;
    if (statusBar) {
        RECT statusRect;
        GetWindowRect(statusBar, &statusRect);
        statusHeight = statusRect.bottom - statusRect.top;
    }

    running = true;

    initClientSize(RFFConstants::Win32::INIT_RENDER_SCENE_WIDTH, RFFConstants::Win32::INIT_RENDER_SCENE_HEIGHT);
    ShowWindow(masterWindow, SW_SHOW);
    UpdateWindow(masterWindow);
}


void RFFMasterWindow::initClientSize(const int width, const int height) const {
    const RECT rect = { 0, 0, width, height};
    RECT adjusted = rect;
    AdjustWindowRect(&adjusted,  WS_OVERLAPPEDWINDOW | WS_SYSMENU, true);

    SetWindowPos(masterWindow, nullptr, 0, 0, adjusted.right - adjusted.left,  adjusted.bottom - adjusted.top + statusHeight, SWP_NOMOVE | SWP_NOZORDER);
    adjustClient(rect);
}

void RFFMasterWindow::adjustClient(const RECT &rect) const {

    SetWindowPos(renderWindow, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
    SetWindowPos(statusBar, nullptr, 0, rect.bottom - rect.top, rect.right - rect.left, statusHeight, SWP_NOZORDER);

    auto rightEdges = std::array<int, RFFConstants::Status::LENGTH>{};

    const int statusBarWidth = rect.right / RFFConstants::Status::LENGTH;
    int rightEdge = statusBarWidth;
    for (int i = 0; i < RFFConstants::Status::LENGTH; i++) {
        rightEdges[i] = rightEdge;
        rightEdge += statusBarWidth;
    }

    SendMessage(statusBar, SB_SETPARTS, RFFConstants::Status::LENGTH, (LPARAM) rightEdges.data());

}

void RFFMasterWindow::refreshStatusBar() const {
    for (int i = 0; i < RFFConstants::Status::LENGTH; ++i) {
        SendMessage(statusBar, SB_SETTEXT, i, (LPARAM)TEXT(statusMessages[i].data()));
    }
}

void RFFMasterWindow::createStatusBar() {

    statusBar = CreateWindowEx(
        0,
        STATUSCLASSNAME,
        nullptr,
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | WS_CLIPCHILDREN,
        0, 0, 0, 0,
        masterWindow,
        nullptr,
        nullptr,
        nullptr);


}
void RFFMasterWindow::createMasterWindow(const DWORD style, const HMENU hMenubar) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpszClassName = RFFConstants::Win32::CLASS_MASTER_WINDOW;
    wc.lpfnWndProc = WndMasterProc;

    assert(RegisterClassEx(&wc));
    masterWindow = CreateWindowEx(
            0,
             wc.lpszClassName,
             wc.lpszClassName,
            style,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,nullptr, hMenubar, nullptr, nullptr
    );

    SetWindowLongPtr(masterWindow, GWLP_USERDATA, (LONG_PTR) this);

}

void RFFMasterWindow::createRenderWindow() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpszClassName = RFFConstants::Win32::CLASS_RENDER_SCENE;
    wc.lpfnWndProc = WndRendererProc;

    assert(RegisterClassEx(&wc));
    renderWindow = CreateWindowEx(
       0,
       wc.lpszClassName,
       wc.lpszClassName,
       WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPSIBLINGS,
       CW_USEDEFAULT,
       CW_USEDEFAULT,
       CW_USEDEFAULT,
       CW_USEDEFAULT, masterWindow, nullptr, nullptr, nullptr);

    if (!masterWindow) {
        std::cout << "Failed to create window!\n";
        return;
    }

    SetWindowLongPtr(renderWindow, GWLP_USERDATA, (LONG_PTR) this);


    hdc = GetDC(renderWindow);

    const int pixelFormatAttrib[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0
    };
    int pixelFormat = 0;
    UINT numFormats = 0;
    wglChoosePixelFormatARB(hdc, pixelFormatAttrib, nullptr, 1, &pixelFormat, &numFormats);
    assert(numFormats);

    PIXELFORMATDESCRIPTOR pfd = {
    };
    DescribePixelFormat(hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);
    constexpr int openglAttrib[]{
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    context = wglCreateContextAttribsARB(hdc, nullptr, openglAttrib);
    assert(context);
}

void RFFMasterWindow::renderLoop() {
    renderer.configure(renderWindow, hdc, context, &statusMessages);
    MSG msg;

    while (running) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }

        refreshStatusBar();
        renderer.renderGL();
        Sleep(1000 / RFFConstants::Win32::INIT_RENDER_SCENE_FPS);
    }
    destroy();
}

void RFFMasterWindow::destroy() const {
    wglDeleteContext(context);
    DeleteDC(hdc);
    DestroyWindow(renderWindow);
    DestroyWindow(masterWindow);
}

void RFFMasterWindow::exit() {
    running = false;
}
