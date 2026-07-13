//
// Created by Merutilm on 2025-05-14.
//

#pragma once

namespace merutilm::rff2 {
    class RFFApplication;
    struct FnFile {
        static void saveMap(RFFApplication &app);
        static void saveImage(RFFApplication &app);
        static void saveLocation(RFFApplication &app);
        static void loadMap(RFFApplication &app);
        static void loadLocation(RFFApplication &app);
    };
}
