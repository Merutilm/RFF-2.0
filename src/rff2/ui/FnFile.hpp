//
// Created by Merutilm on 2025-05-14.
//

#pragma once

namespace merutilm::rff2 {
    class RFF2;
    struct FnFile {
        static void saveMap(RFF2 &app);
        static void saveImage(RFF2 &app);
        static void saveLocation(RFF2 &app);
        static void loadMap(RFF2 &app);
        static void loadLocation(RFF2 &app);
    };
}
