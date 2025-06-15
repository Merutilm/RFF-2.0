#include <memory>

#include "GLLoader.h"
#include "MainWindow.h"
#include "SettingsWindow.h"
#include "assert.h"
#include "VideoWindow.h"
#include "../calc/dex_exp.h"


void registerClasses() {
    using namespace merutilm::rff::Constants::Win32;
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);

    WNDCLASSEX masterWindowClass = wc;
    masterWindowClass.lpszClassName = CLASS_MASTER_WINDOW;
    masterWindowClass.lpfnWndProc = merutilm::rff::MainWindow::masterWindowProc;
    masterWindowClass.hIcon = (HICON) LoadImage(nullptr, ICON_DEFAULT_PATH, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_DEFAULTSIZE);
    assert(RegisterClassEx(&masterWindowClass));

    WNDCLASSEX videoWindowClass = wc;
    videoWindowClass.lpszClassName = CLASS_VIDEO_WINDOW;
    videoWindowClass.lpfnWndProc = merutilm::rff::VideoWindow::videoWindowProc;
    videoWindowClass.hIcon = masterWindowClass.hIcon;
    assert(RegisterClassEx(&videoWindowClass));


    WNDCLASSEX settingsWindowClass = wc;
    settingsWindowClass.cbSize = sizeof(WNDCLASSEX);
    settingsWindowClass.lpszClassName = CLASS_SETTINGS_WINDOW;
    settingsWindowClass.lpfnWndProc = merutilm::rff::SettingsWindow::settingsWindowProc;
    settingsWindowClass.hbrBackground = CreateSolidBrush(COLOR_LABEL_BACKGROUND);
    assert(RegisterClassEx(&settingsWindowClass));

    WNDCLASSEX videoRenderWindowClass = wc;
    videoRenderWindowClass.lpszClassName = CLASS_VIDEO_RENDER_WINDOW;
    videoRenderWindowClass.lpfnWndProc = DefWindowProc;
    assert(RegisterClassEx(&videoRenderWindowClass));

    WNDCLASSEX renderSceneClass = wc;
    renderSceneClass.lpszClassName = CLASS_RENDER_SCENE;
    renderSceneClass.lpfnWndProc = merutilm::rff::MainWindow::renderSceneProc;
    assert(RegisterClassEx(&renderSceneClass));

}

void unregisterAll() {
    using namespace merutilm::rff::Constants::Win32;
    UnregisterClass(CLASS_MASTER_WINDOW, nullptr);
    UnregisterClass(CLASS_VIDEO_WINDOW, nullptr);
    UnregisterClass(CLASS_SETTINGS_WINDOW, nullptr);
    UnregisterClass(CLASS_VIDEO_RENDER_WINDOW, nullptr);
    UnregisterClass(CLASS_RENDER_SCENE, nullptr);
}


int main() {
    registerClasses();
    merutilm::rff::GLLoader::initGL();
    const auto wnd = std::make_unique<merutilm::rff::MainWindow>();
    wnd->renderLoop();
    unregisterAll();
    return 0;
}
