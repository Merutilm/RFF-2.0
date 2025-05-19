#include <memory>

#include "RFFGL.h"
#include "RFFMasterWindow.h"
#include "RFFSettingsWindow.h"
#include "../calc/dex_exp.h"


int main() {
    RFFGL::initGL();
    RFFSettingsWindow::initClass();
    const auto wnd = std::make_unique<RFFMasterWindow>();
    wnd->renderLoop();
    return 0;
}
