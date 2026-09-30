#include "RFF2.hpp"

#ifndef NDEBUG

static void counter(const std::filesystem::path &path, uint32_t *cnt) {
    if (std::filesystem::is_directory(path)) {
        for (std::filesystem::directory_iterator it(path); it != std::filesystem::directory_iterator(); ++it) {
            auto child = it->path();
            counter(child, cnt);
        }
    } else if (path.string().ends_with(".cpp") || path.string().ends_with(".hpp")) {

        std::ifstream ifs(path);
        std::string v;
        while (std::getline(ifs, v)) {
            ++*cnt;
        }
    }
}

static void count() {
    uint32_t cnt = 0;
    counter(std::filesystem::path("../src"), &cnt);
    counter(std::filesystem::path("../include"), &cnt);
    std::cout << cnt << std::endl;
}

static void testCode() {
    using namespace merutilm::rff2;
    using namespace merutilm::vkh;

    fixed_point_decimal a("-0.96128094211304257740641040047580610611", -37);
    a.add_n(24);
    std::cout << a.to_string() << std::endl;
    a.add_n(9223372036854775807);
    std::cout << a.to_string() << std::endl;
    a.add_n(9223372036854775807);
    std::cout << a.to_string() << std::endl;
    a.add_n(-9223372036854775807);
    std::cout << a.to_string() << std::endl;
    a.add_n(-9223372036854775807);
    std::cout << a.to_string() << std::endl;
    a.add_n(-9223372036854775807);
    std::cout << a.to_string() << std::endl;

}
#endif

int main() {
    using namespace merutilm::rff2;
    using namespace merutilm::vkh;

#ifndef NDEBUG
    testCode();
    count();
#endif
    Application::start<RFF2>({.framerate = Constants::Render::INIT_FPS,
                              .name = "RFF 2.0",
                              .icon = "../res/icon.png",
                              .widthInfo = {.min = Constants::Render::MIN_WINDOW_WIDTH,
                                            .max = GLFW_DONT_CARE,
                                            .first = Constants::Render::INIT_WINDOW_WIDTH},
                              .heightInfo = {.min = Constants::Render::MIN_WINDOW_HEIGHT,
                                             .max = GLFW_DONT_CARE,
                                             .first = Constants::Render::INIT_WINDOW_HEIGHT},
                              .resizable = true,
                              .filedrop = false});

    return 0;
}
