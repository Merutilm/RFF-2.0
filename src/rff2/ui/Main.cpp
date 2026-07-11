#include <memory>

#ifndef NDEBUG
#include <fstream>
#endif

// #include "vulkan_helper/util/GraphicsContextWindowProc.hpp"
#include <filesystem>


#include "../util/profiler.hpp"
#include "RFFApplication.hpp"
#include "SettingsWindow.hpp"
#include "VideoWindow.hpp"


#ifndef NDEBUG

void counter(const std::filesystem::path &path, uint32_t *lines) {
    if (std::filesystem::is_directory(path)) {
        for (std::filesystem::directory_iterator it(path); it != std::filesystem::directory_iterator(); ++it) {
            auto child = it->path();
            counter(child, lines);
        }
    } else if (path.string().ends_with(".cpp") || path.string().ends_with(".hpp")) {

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

#ifndef NDEBUG
    countLines();
#endif
    Application::start<RFFApplication>({.framerate = 60,
                                     .name = "RFF 2.0",
                                     .icon = "../res/icon.png",
                                     .widthInfo = {.min = 100,
                                                   .max = INT_MAX,
                                                   .first = 1280},
                                     .heightInfo = {.min = 100,
                                                    .max = INT_MAX,
                                                    .first = 720},
                                     .resizable = true,
                                     .filedrop = false});

    return 0;
}
