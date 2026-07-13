//
// Created by Merutilm on 2025-05-14.
//

#include "FnFile.hpp"

#include "../constants/Constants.hpp"
#include "../io/RFFLocationBinary.h"
#include "../ui/RFFApplication.hpp"
#include "IOUtilities.h"
#include "imgui.h"

namespace merutilm::rff2 {

    void FnFile::saveMap(RFFApplication &app) {
        if (ImGui::Button("Save Map", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_DYNAMIC_MAP, IOUtilities::SAVE_FILE,
                                                        Constants::File::EXT_DYNAMIC_MAP);
            if (path == nullptr) {
                return;
            }
            app.generateMap().exportFile(*path);
        }
    }
    void FnFile::saveImage(RFFApplication &app) {
        if (ImGui::Button("Save Image", ImVec2(-FLT_MIN, 0))) {
            app.getRequests().requestCreateImage();
        }
    }
    void FnFile::saveLocation(RFFApplication &app) {

        if (ImGui::Button("Save Location", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_LOCATION, IOUtilities::SAVE_FILE,
                   Constants::File::EXT_LOCATION);
            if (path == nullptr) {
                return;
            }
            auto settings = app.getSettings().fractal; // clone the settings
            auto &center = settings.reference.center;
            RFFLocationBinary(settings.general.logZoom, center.real.to_string(), center.imag.to_string(),
                              settings.perturb.maxIteration)
                    .exportFile(*path);
        }
    }
    void FnFile::loadMap(RFFApplication &app) {

        if (ImGui::Button("Load Map", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_DYNAMIC_MAP, IOUtilities::OPEN_FILE,
                        Constants::File::EXT_DYNAMIC_MAP);
            if (path == nullptr) {
                return;
            }
            app.overwriteMatrixFromMap(RFFDynamicMapBinary::read(*path));
        }
    }

    void FnFile::loadLocation(RFFApplication &app) {


        if (ImGui::Button("Load Location", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_LOCATION, IOUtilities::OPEN_FILE,
                   Constants::File::EXT_LOCATION);
            if (path == nullptr) {
                return;
            }
            const RFFLocationBinary location = RFFLocationBinary::read(*path);

            app.getSettings().fractal.reference.center = fixed_point_complex_i1(
                    location.getReal(), location.getImag(), Perturbator::logZoomToExp10(location.getLogZoom()));
            app.getSettings().fractal.general.logZoom = location.getLogZoom();
            app.getSettings().fractal.perturb.maxIteration = location.getMaxIteration();
            app.getRequests().requestRecompute();
        }
    }
} // namespace merutilm::rff2
