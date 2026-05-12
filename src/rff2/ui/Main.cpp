#include <memory>

#ifndef NDEBUG
#include <fstream>
#endif

// #include "vulkan_helper/util/GraphicsContextWindowProc.hpp"
#include "../util/profiler.hpp"
#include "AppLauncher.hpp"
#include "SettingsWindow.hpp"
#include "VideoWindow.hpp"

void registerClasses() {
    using namespace merutilm::rff2;
    using namespace Constants::Win32;
    using namespace merutilm::vkh;
    WNDCLASSEXW wClass = {};
    wClass.cbSize = sizeof(WNDCLASSEXW);
    wClass.hInstance = GetModuleHandleW(nullptr);

    WNDCLASSEXW settingsWindowClass = wClass;
    settingsWindowClass.lpszClassName = CLASS_SETTINGS_WINDOW;
    settingsWindowClass.lpfnWndProc = SettingsWindow::settingsWindowProc;
    settingsWindowClass.hbrBackground = CreateSolidBrush(COLOR_LABEL_BACKGROUND);
    if(!RegisterClassExW(&settingsWindowClass)) throw exception_init("Failed to register class : Settings Window");

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
    uint32_t lines = 0;
    counter(std::filesystem::path("../src"), &lines);
    counter(std::filesystem::path("../include"), &lines);
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
    const auto app = AppLauncher();
    app.start();
    return 0;
}
