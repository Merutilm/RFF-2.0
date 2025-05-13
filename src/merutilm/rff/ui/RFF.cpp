#include <memory>

#include "RFFGL.h"
#include "RFFMasterWindow.h"
#include "RFFSettingsWindow.h"



int main() {
    RFFGL::initGL();
    RFFSettingsWindow::initClass();
    const auto wnd = std::make_unique<RFFMasterWindow>();
    wnd->renderLoop();
    return 0;
}
