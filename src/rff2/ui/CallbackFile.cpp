//
// Created by Merutilm on 2025-05-14.
//

#ifdef _WIN32
#include "CallbackFile.hpp"

#include "../constants/Constants.hpp"
#include "../io/RFFLocationBinary.h"
#include "IOUtilities.h"
#include "SettingsMenuGenerator.hpp"


namespace merutilm::rff2 {

    std::function<void()> CallbackFile::fnSaveMap(AppRenderManager &arm) {
        return [&arm] {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_DYNAMIC_MAP, IOUtilities::SAVE_FILE,
                                                        Constants::File::EXT_DYNAMIC_MAP);
            if (path == nullptr) {
                return;
            }
            arm.generateMap().exportFile(*path);
        };
    }
    std::function<void()> CallbackFile::fnSaveImage(AppRenderManager &arm) {
        return [&arm] { arm.getRequests().requestCreateImage(); };
    }
    std::function<void()> CallbackFile::fnSaveLocation(AppRenderManager &arm) {
        return [&arm] {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_LOCATION, IOUtilities::SAVE_FILE,
                                                        Constants::File::EXT_LOCATION);
            if (path == nullptr) {
                return;
            }
            auto settings = arm.getSettings().fractal; // clone the settings
            auto &center = settings.reference.center;
            RFFLocationBinary(settings.general.logZoom, center.real.to_string(), center.imag.to_string(), settings.perturb.maxIteration)
                    .exportFile(*path);
        };
    }
    std::function<void()> CallbackFile::fnLoadMap(AppRenderManager &arm) {
        return [&arm] {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_DYNAMIC_MAP, IOUtilities::OPEN_FILE,
                                                        Constants::File::EXT_DYNAMIC_MAP);
            if (path == nullptr) {
                return;
            }
            arm.overwriteMatrixFromMap(RFFDynamicMapBinary::read(*path));
        };
    }

    std::function<void()> CallbackFile::fnLoadLocation(AppRenderManager &arm) {
        return [&arm] {
            const auto path = IOUtilities::ioFileDialog(Constants::File::DESC_LOCATION, IOUtilities::OPEN_FILE,
                                                        Constants::File::EXT_LOCATION);
            if (path == nullptr) {
                return;
            }
            const RFFLocationBinary location = RFFLocationBinary::read(*path);

            arm.getSettings().fractal.reference.center = fixed_point_complex_i1(
                    location.getReal(), location.getImag(), Perturbator::logZoomToExp10(location.getLogZoom()));
            arm.getSettings().fractal.general.logZoom = location.getLogZoom();
            arm.getSettings().fractal.perturb.maxIteration = location.getMaxIteration();
            arm.getRequests().requestRecompute();
        };
    }
} // namespace merutilm::rff2
#endif