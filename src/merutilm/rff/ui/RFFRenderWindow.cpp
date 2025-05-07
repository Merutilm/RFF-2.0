#include <glad_wgl.h>
#include "RFFRenderWindow.h"

#include <iostream>
#include <windows.h>
#include "RFFConstants.h"
#include <assert.h>


LRESULT CALLBACK RFFRenderWindow::WndProc(const HWND hwnd, const UINT message, const WPARAM wParam,
                                          const LPARAM lParam) {
    const auto wnd = reinterpret_cast<RFFRenderWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    wnd->renderer.runAction(hwnd, message, wParam);

    switch (message) {
        case WM_CLOSE:
            wnd->exit();
            return 0;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

RFFRenderWindow::RFFRenderWindow() : renderer(RFFRenderer()) {
    initWindow();
}

UINT_PTR RFFRenderWindow::castMenu(HMENU menu) {
    return reinterpret_cast<UINT_PTR>(menu);
}

void RFFRenderWindow::initMenu(const HMENU hMenubar) {
    std::string menus[] = {
        "File", "Fractal", "Image", "Shader", "Preset", "Video", "Explore"
    };

    for (auto &menu: menus) {
        const HMENU hMenu = CreateMenu();
        AppendMenu(hMenubar, MF_POPUP, castMenu(hMenu), menu.c_str());
    }
}


void RFFRenderWindow::initWindow() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpszClassName = "RFF 2.0";
    wc.lpfnWndProc = WndProc;

    assert(RegisterClassEx(&wc));

    SetProcessDPIAware();

    RECT rect = {0, 0, RFFConstants::WindowInit::INIT_WIDTH, RFFConstants::WindowInit::INIT_HEIGHT};
    constexpr DWORD style = WS_OVERLAPPEDWINDOW | WS_SYSMENU;
    AdjustWindowRect(&rect, style, true);
    const HMENU hMenubar = CreateMenu();
    initMenu(hMenubar);

    window = CreateWindowEx(
        0,
        wc.lpszClassName,
        "RFF 2.0",
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top, nullptr, hMenubar, nullptr, nullptr);

    SetMenu(window, hMenubar);

    if (!window) {
        std::cout << "Failed to create window!\n";
        return;
    }

    SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR) this);


    hdc = GetDC(window);

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

    running = true;

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
}


void RFFRenderWindow::renderLoop() {
    while (running) {
        MSG msg;
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //Do GL Function here ! ! !
        makeContextCurrent();
        glClearColor(1.0, 0.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        swap();
        Sleep(1000 / RFFConstants::AnimInit::INIT_FPS);
    }
}

void RFFRenderWindow::makeContextCurrent() const {
    wglMakeCurrent(hdc, context);
}


void RFFRenderWindow::swap() const {
    wglSwapIntervalEXT(true);
    wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}


void RFFRenderWindow::exit() {
    running = false;
    wglDeleteContext(context);
    DestroyWindow(window);
}
