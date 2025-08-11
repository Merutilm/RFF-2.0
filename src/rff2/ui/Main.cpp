#include <memory>
#include <fstream>

#include "Application.hpp"
#include "WGLContextLoader.h"
#include "GLMainWindow.h"
#include "SettingsWindow.h"
#include "VideoWindow.h"

void registerClasses() {
    using namespace merutilm::rff2::Constants::Win32;
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);

    WNDCLASSEXW masterWindowClass = wc;
    masterWindowClass.lpszClassName = CLASS_MASTER_WINDOW;
    masterWindowClass.lpfnWndProc = merutilm::vkh::GraphicsContextWindow::GraphicsContextWindowProc;
    masterWindowClass.hIcon = static_cast<HICON>(LoadImage(
    GetModuleHandle(nullptr),
    MAKEINTRESOURCE(1),
    IMAGE_ICON,
    32, 32,
    LR_DEFAULTCOLOR));
    assert(RegisterClassExW(&masterWindowClass));

    WNDCLASSEXW videoWindowClass = wc;
    videoWindowClass.lpszClassName = CLASS_VIDEO_WINDOW;
    videoWindowClass.lpfnWndProc = merutilm::rff2::VideoWindow::videoWindowProc;
    videoWindowClass.hIcon = masterWindowClass.hIcon;
    assert(RegisterClassExW(&videoWindowClass));

    WNDCLASSEXW settingsWindowClass = wc;
    settingsWindowClass.cbSize = sizeof(WNDCLASSEX);
    settingsWindowClass.lpszClassName = CLASS_SETTINGS_WINDOW;
    settingsWindowClass.lpfnWndProc = merutilm::rff2::SettingsWindow::settingsWindowProc;
    settingsWindowClass.hbrBackground = CreateSolidBrush(COLOR_LABEL_BACKGROUND);
    assert(RegisterClassExW(&settingsWindowClass));

    WNDCLASSEXW videoRenderWindowClass = wc;
    videoRenderWindowClass.lpszClassName = CLASS_VIDEO_RENDER_WINDOW;
    videoRenderWindowClass.lpfnWndProc = DefWindowProc;
    assert(RegisterClassExW(&videoRenderWindowClass));

    WNDCLASSEXW renderSceneClass = wc;
    renderSceneClass.lpszClassName = CLASS_GL_RENDER_SCENE;
    renderSceneClass.lpfnWndProc = merutilm::rff2::GLMainWindow::renderSceneProc;
    assert(RegisterClassExW(&renderSceneClass));

    WNDCLASSEXW vkRenderSceneClass = wc;
    vkRenderSceneClass.lpszClassName = CLASS_VK_RENDER_SCENE;
    vkRenderSceneClass.lpfnWndProc = DefWindowProc;
    assert(RegisterClassExW(&vkRenderSceneClass));

}

void counter(const std::filesystem::path &path, uint32_t *lines) {
    if (std::filesystem::is_directory(path)) {
        for (std::filesystem::directory_iterator it(path); it != std::filesystem::directory_iterator(); ++it) {
            auto child = it->path();
            counter(child, lines);
        }
    }else if (path.string().ends_with(".cpp") || path.string().ends_with(".hpp")){

        std::ifstream ifs(path);
        std::string v;
        while (std::getline(ifs, v)) {
            ++*lines;
        }
    }
}

void countLines() {
    const std::filesystem::path path("../src");
    uint32_t lines = 0;
    counter(path, &lines);
    std::cout << "Lines : " << lines << std::endl;

}

int main() {
    using namespace merutilm::rff2;
    using namespace merutilm::vkh;
    registerClasses();
    countLines();

    //WGLContextLoader::initGL();
    //const auto wnd = std::make_unique<GLMainWindow>();
    //wnd->renderLoop();
    auto app = Application();
    app.start();
    return 0;
}
