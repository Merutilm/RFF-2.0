#include <memory>

#ifndef NDEBUG
#include <fstream>
#endif

#include "Application.hpp"
#include "SettingsWindow.hpp"
#include "VideoWindow.hpp"
#include "../../vulkan_helper/util/GraphicsContextWindowProc.hpp"

void registerClasses() {
    using namespace merutilm::rff2;
    using namespace Constants::Win32;
    using namespace merutilm::vkh;
    WNDCLASSEXW wClass = {};
    wClass.cbSize = sizeof(WNDCLASSEXW);
    wClass.hInstance = GetModuleHandleW(nullptr);

    WNDCLASSEXW masterWindowClass = wClass;
    masterWindowClass.lpszClassName = CLASS_MASTER_WINDOW;
    masterWindowClass.lpfnWndProc = GraphicsContextWindowProc::WinProc;
    masterWindowClass.hIcon = static_cast<HICON>(LoadImageW(
    GetModuleHandleW(nullptr),
    MAKEINTRESOURCEW(1),
    IMAGE_ICON,
    32, 32,
    LR_DEFAULTCOLOR));
    if(!RegisterClassExW(&masterWindowClass)) throw exception_init("Failed to register class : Master Window");

    WNDCLASSEXW videoWindowClass = wClass;
    videoWindowClass.lpszClassName = CLASS_VIDEO_WINDOW;
    videoWindowClass.lpfnWndProc = VideoWindow::videoWindowProc;
    videoWindowClass.hIcon = masterWindowClass.hIcon;
    if(!RegisterClassExW(&videoWindowClass)) throw exception_init("Failed to register class : Video Window");

    WNDCLASSEXW settingsWindowClass = wClass;
    settingsWindowClass.lpszClassName = CLASS_SETTINGS_WINDOW;
    settingsWindowClass.lpfnWndProc = SettingsWindow::settingsWindowProc;
    settingsWindowClass.hbrBackground = CreateSolidBrush(COLOR_LABEL_BACKGROUND);
    if(!RegisterClassExW(&settingsWindowClass)) throw exception_init("Failed to register class : Settings Window");

    WNDCLASSEXW videoRenderWindowClass = wClass;
    videoRenderWindowClass.lpszClassName = CLASS_VIDEO_RENDER_WINDOW;
    videoRenderWindowClass.lpfnWndProc = DefWindowProcW;
    if (!RegisterClassExW(&videoRenderWindowClass)) throw exception_init("Failed to register class : Video Window");

    WNDCLASSEXW vkRenderSceneClass = wClass;
    vkRenderSceneClass.lpszClassName = CLASS_VK_RENDER_SCENE;
    vkRenderSceneClass.lpfnWndProc = RenderScene::renderSceneProc;
    if(!RegisterClassExW(&vkRenderSceneClass)) throw exception_init("Failed to register class : Video Scene");

}

#ifndef NDEBUG

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
#endif

int main() {
    using namespace merutilm::rff2;
    using namespace merutilm::vkh;
    registerClasses();
#ifndef NDEBUG
    countLines();
#endif
    const auto app = Application();
    app.start();
    return 0;
}
