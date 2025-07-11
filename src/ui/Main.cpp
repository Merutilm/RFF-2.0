#include <memory>

#include "WGLContextLoader.h"
#include "MainWindow.h"
#include "SettingsWindow.h"
#include "assert.h"
#include "VideoWindow.h"
#include "../calc/dex_exp.h"
#include "../opengl/GLShaderLoader.h"

void registerClasses() {
    using namespace merutilm::rff2::Constants::Win32;
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);

    WNDCLASSEX masterWindowClass = wc;
    masterWindowClass.lpszClassName = CLASS_MASTER_WINDOW;
    masterWindowClass.lpfnWndProc = merutilm::rff2::MainWindow::masterWindowProc;
    masterWindowClass.hIcon = static_cast<HICON>(LoadImage(
    GetModuleHandle(nullptr),
    MAKEINTRESOURCE(1),
    IMAGE_ICON,
    32, 32,
    LR_DEFAULTCOLOR));
    assert(RegisterClassEx(&masterWindowClass));

    WNDCLASSEX videoWindowClass = wc;
    videoWindowClass.lpszClassName = CLASS_VIDEO_WINDOW;
    videoWindowClass.lpfnWndProc = merutilm::rff2::VideoWindow::videoWindowProc;
    videoWindowClass.hIcon = masterWindowClass.hIcon;
    assert(RegisterClassEx(&videoWindowClass));


    WNDCLASSEX settingsWindowClass = wc;
    settingsWindowClass.cbSize = sizeof(WNDCLASSEX);
    settingsWindowClass.lpszClassName = CLASS_SETTINGS_WINDOW;
    settingsWindowClass.lpfnWndProc = merutilm::rff2::SettingsWindow::settingsWindowProc;
    settingsWindowClass.hbrBackground = CreateSolidBrush(COLOR_LABEL_BACKGROUND);
    assert(RegisterClassEx(&settingsWindowClass));

    WNDCLASSEX videoRenderWindowClass = wc;
    videoRenderWindowClass.lpszClassName = CLASS_VIDEO_RENDER_WINDOW;
    videoRenderWindowClass.lpfnWndProc = DefWindowProc;
    assert(RegisterClassEx(&videoRenderWindowClass));

    WNDCLASSEX renderSceneClass = wc;
    renderSceneClass.lpszClassName = CLASS_RENDER_SCENE;
    renderSceneClass.lpfnWndProc = merutilm::rff2::MainWindow::renderSceneProc;
    assert(RegisterClassEx(&renderSceneClass));

}

int main() {
    using namespace merutilm::rff2;
    registerClasses();
    WGLContextLoader::initGL();
    const auto wnd = std::make_unique<MainWindow>();
    wnd->renderLoop();
    return 0;
}
