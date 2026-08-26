//
// Created by Merutilm on 2025-05-14.
//

#include "FnFile.hpp"

#include "../constants/Constants.hpp"
#include "../ui/RFF2.hpp"
#include "IOUtilities.h"
#include "imgui.h"

namespace merutilm::rff2 {

    void FnFile::saveMap(RFF2 &app) {
        if (ImGui::Button("Save Map", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_DYNAMIC_MAP, IOUtilities::SAVE_FILE,
                                                        Constants::File::EXT_DYNAMIC_MAP);
            if (path == nullptr) {
                return;
            }
            app.generateMap().exportFile(*path);
        }
    }
    void FnFile::saveImage(RFF2 &app) {
        if (ImGui::Button("Save Image", ImVec2(-FLT_MIN, 0))) {
            app.getRequests().requestCreateImage();
        }
    }
    void FnFile::saveLocation(RFF2 &app) {

        if (ImGui::Button("Save Location", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_LOCATION, IOUtilities::SAVE_FILE,
                   Constants::File::EXT_LOCATION);
            if (path == nullptr) {
                return;
            }
            app.saveCurrentLocation(*path);
        }
    }
    void FnFile::loadMap(RFF2 &app) {

        if (ImGui::Button("Load Map", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_DYNAMIC_MAP, IOUtilities::OPEN_FILE,
                        Constants::File::EXT_DYNAMIC_MAP);
            if (path == nullptr) {
                return;
            }
            app.overwriteMatrixFromMap(RFFDynamicMapBinary::read(*path));
        }
    }

    void FnFile::loadLocation(RFF2 &app) {


        if (ImGui::Button("Load Location", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_LOCATION, IOUtilities::OPEN_FILE,
                   Constants::File::EXT_LOCATION);
            if (path == nullptr) {
                return;
            }
            app.loadLocation(*path);
        }
    }
} // namespace merutilm::rff2
