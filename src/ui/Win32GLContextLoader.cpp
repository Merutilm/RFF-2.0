//
// Created by Merutilm on 2025-05-07.
//

#include <glad_wgl.h>
#include "Win32GLContextLoader.h"

#include <assert.h>
#include <windows.h>
#include <assert.h>

#include "glad.h"
#include "Constants.h"

namespace merutilm::rff {

    void Win32GLContextLoader::initGL() {
        SetProcessDPIAware();

        WNDCLASSEX glDummyClass = {};
        glDummyClass.cbSize = sizeof(WNDCLASSEX);
        glDummyClass.lpszClassName = Constants::Win32::CLASS_GL_DUMMY;
        glDummyClass.lpfnWndProc = DefWindowProc;
        glDummyClass.style = CS_OWNDC;
        RegisterClassEx(&glDummyClass);

        const auto dummyWnd = CreateWindowEx(
            0,
            Constants::Win32::CLASS_GL_DUMMY,
            Constants::Win32::CLASS_GL_DUMMY,
            WS_OVERLAPPEDWINDOW | WS_SYSMENU,
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


    void Win32GLContextLoader::createContext(const HDC hdc, HGLRC *target) {
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

        *target = wglCreateContextAttribsARB(hdc, nullptr, openglAttrib);
        assert(*target);

    }
}
