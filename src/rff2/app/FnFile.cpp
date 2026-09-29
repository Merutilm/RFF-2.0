//
// Created by Merutilm on 2025-05-14.
//

#include "FnFile.hpp"

#include "../app/RFF2.hpp"
#include "../constants/Constants.hpp"
#include "IOUtilities.h"
#include "imgui.h"

namespace merutilm::rff2 {


    void FnFile::saveShader(RFF2 &app) {

        if (ImGui::Button("Save Shader", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_SHADER, IOUtilities::SAVE_FILE,
                        Constants::File::EXT_SHADER);
            if (path == nullptr) {
                return;
            }
            app.saveCurrentShader(*path);
        }
    }
    void FnFile::saveMap(RFF2 &app) {
        if (ImGui::Button("Save Map", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_DYNAMIC_MAP, IOUtilities::SAVE_FILE,
                                                        Constants::File::EXT_DYNAMIC_MAP);
            if (path == nullptr) {
                return;
            }
            RFFBinary::exportFile(app.generateMap(), *path);
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
    void FnFile::loadShader(RFF2 &app) {

        if (ImGui::Button("Load Shader", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_SHADER, IOUtilities::OPEN_FILE,
                        Constants::File::EXT_SHADER);
            if (path == nullptr) {
                return;
            }
            app.loadShader(*path);
        }
    }
    void FnFile::loadMap(RFF2 &app) {

        if (ImGui::Button("Load Map", ImVec2(-FLT_MIN, 0))) {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_DYNAMIC_MAP, IOUtilities::OPEN_FILE,
                        Constants::File::EXT_DYNAMIC_MAP);
            if (path == nullptr) {
                return;
            }
            app.overwriteMatrixFromMap(RFFBinary::importFile<RFFDynamicMapBinary>(*path));
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
    void FnFile::autoSaveBackup(RFF2 &app) {
        ImGui::Checkbox("Auto Save Backup", &app.getSettings().file.autoSaveBackup);
        Utilities::imguiHelpMarker("Automatically saves the backup for each calculation");
    }
} // namespace merutilm::rff2
