#include <memory>

#include "RFFGL.h"
#include "RFFMasterWindow.h"


// void doSomething() {
//     WNDCLASSEX wc = {};
//
//     SetProcessDPIAware();
//     wc.cbSize = sizeof(WNDCLASSEX);
//     wc.lpszClassName = "Options";
//     wc.lpfnWndProc = DefWindowProc;
//     RegisterClassEx(&wc);
//     DWORD style = WS_SYSMENU | WS_VISIBLE;
//
//     HWND wnd = CreateWindowEx(
//         NULL, wc.lpszClassName, "window", style, 0, 0, 400, 400, nullptr, nullptr, nullptr, nullptr
//     );
//
//
//     HWND wnd2 = CreateWindowEx(NULL, "COMBOBOX", "Set", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | CBS_DROPDOWNLIST, 10, 10, 100, 400, wnd, (HMENU)1000, (HINSTANCE)GetWindowLongPtr(wnd, GWLP_HINSTANCE), nullptr);
//     SendMessage(wnd2, CB_ADDSTRING, 10, (LPARAM)"APPLE");
//     SendMessage(wnd2, CB_SETCURSEL, 0, 0);
//
//     ShowWindow(wnd, SW_SHOW);
//     UpdateWindow(wnd);
// }


int main() {
    // doSomething();
    RFFGL::initGL();
    const auto wnd = std::make_unique<RFFMasterWindow>();
    wnd->renderLoop();
    return 0;
}
