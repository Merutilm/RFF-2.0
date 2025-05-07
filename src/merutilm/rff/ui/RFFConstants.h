
#pragma once
#include <cstdint>
#include <string>

#include "../precision/Center.h"
#include "../settings/Settings.h"

namespace RFFConstants {
    namespace WindowInit {
        constexpr static short INIT_WIDTH = 1280;
        constexpr static short INIT_HEIGHT = 720;
    }

    namespace AnimInit {
        constexpr static short INIT_FPS = 60;
    }

    namespace Render {
        constexpr static float ZOOM_MIN = 1.0f;
        constexpr static float ZOOM_INTERVAL = 0.235;
    }

    namespace Precision {
        static constexpr float LOG10_2 = 0.3010299956639811952137388947244930267681898814621085375f;
        static constexpr int DOUBLE_PRECISION = 52;
        static constexpr uint64_t SIGNUM_BIT = 0x8000000000000000LL;
        static constexpr uint64_t EXP0_BITS = 0x3ff0000000000000LL;
        static constexpr uint64_t DECIMAL_SIGNUM_BITS = 0x800fffffffffffffLL;
        static constexpr int PRECISION_ADDITION = 20;
    }


    namespace Parser {
        static constexpr std::string UNKNOWN = "UNKNOWN";
    }
}
