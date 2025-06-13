#include <memory>

#include "RFFGL.h"
#include "RFFMasterWindow.h"
#include "RFFSettingsWindow.h"
#include "assert.h"
#include "RFFVideoWindow.h"
#include "../calc/dex_exp.h"


void registerClasses() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);

    WNDCLASSEX masterWindowClass = wc;
    masterWindowClass.lpszClassName = RFF::Win32::CLASS_MASTER_WINDOW;
    masterWindowClass.lpfnWndProc = RFFMasterWindow::masterWindowProc;
    masterWindowClass.hIcon = (HICON) LoadImage(nullptr, RFF::Win32::ICON_DEFAULT_PATH, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_DEFAULTSIZE);
    assert(RegisterClassEx(&masterWindowClass));

    WNDCLASSEX videoWindowClass = wc;
    videoWindowClass.lpszClassName = RFF::Win32::CLASS_VIDEO_WINDOW;
    videoWindowClass.lpfnWndProc = RFFVideoWindow::videoWindowProc;
    videoWindowClass.hIcon = masterWindowClass.hIcon;
    assert(RegisterClassEx(&videoWindowClass));


    WNDCLASSEX settingsWindowClass = wc;
    settingsWindowClass.cbSize = sizeof(WNDCLASSEX);
    settingsWindowClass.lpszClassName = RFF::Win32::CLASS_SETTINGS_WINDOW;
    settingsWindowClass.lpfnWndProc = RFFSettingsWindow::settingsWindowProc;
    settingsWindowClass.hbrBackground = CreateSolidBrush(RFF::Win32::COLOR_LABEL_BACKGROUND);
    assert(RegisterClassEx(&settingsWindowClass));

    WNDCLASSEX progressClass = wc;
    progressClass.lpszClassName = RFF::Win32::CLASS_PROGRESS;
    progressClass.lpfnWndProc = DefWindowProc;
    assert(RegisterClassEx(&progressClass));

    WNDCLASSEX videoRenderWindowClass = wc;
    videoRenderWindowClass.lpszClassName = RFF::Win32::CLASS_VIDEO_RENDER_WINDOW;
    videoRenderWindowClass.lpfnWndProc = DefWindowProc;
    assert(RegisterClassEx(&videoRenderWindowClass));

    WNDCLASSEX renderSceneClass = wc;
    renderSceneClass.lpszClassName = RFF::Win32::CLASS_RENDER_SCENE;
    renderSceneClass.lpfnWndProc = RFFMasterWindow::renderSceneProc;
    assert(RegisterClassEx(&renderSceneClass));

}


int main() {
    registerClasses();
    RFFGL::initGL();
    const auto wnd = std::make_unique<RFFMasterWindow>();
    wnd->renderLoop();
    return 0;
}
