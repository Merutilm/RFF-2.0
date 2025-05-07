//
// Created by Merutilm on 2025-05-07.
//

#include <glad_wgl.h>
#include "RFFGL.h"

#include <assert.h>
#include <windows.h>
#include <assert.h>

#include "glad.h"
#include "RFFConstants.h"

void RFFGL::initGL() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpszClassName = "Dummy";
    wc.lpfnWndProc = DefWindowProc;
    wc.style = CS_OWNDC;

    assert(RegisterClassEx(&wc));

    SetProcessDPIAware();
    constexpr DWORD style = WS_OVERLAPPEDWINDOW | WS_SYSMENU;

    const auto dummyWnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        "RFF 2.0",
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT, nullptr, nullptr, nullptr, nullptr);

    const auto dummyDC = GetDC(dummyWnd);

    PIXELFORMATDESCRIPTOR pfd = {};

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cAlphaBits = 8;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;


    const auto pf = ChoosePixelFormat(dummyDC, &pfd);
    SetPixelFormat(dummyDC, pf, &pfd);

    const auto context = wglCreateContext(dummyDC);
    assert(context);

    wglMakeCurrent(dummyDC, context);
    assert(gladLoadWGL(dummyDC) && gladLoadGL());


    wglMakeCurrent(dummyDC, nullptr);
    wglDeleteContext(context);
    ReleaseDC(dummyWnd, dummyDC);
    DestroyWindow(dummyWnd);
}
