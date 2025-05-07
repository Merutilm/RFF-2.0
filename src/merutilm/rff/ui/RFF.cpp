#include <memory>

#include "RFFGL.h"
#include "RFFRenderWindow.h"

int main() {
    RFFGL::initGL();
    const auto wnd = std::make_unique<RFFRenderWindow>();
    wnd->renderLoop();
    return 0;

}