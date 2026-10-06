#pragma once
#include <glm/glm.hpp>
namespace merutilm::rff2 {
    struct ShdSurfaceSettings {
        float depth;
        float shadowBrightness;
        float shadowOpacity;

        float lightZenith;
        float lightAzimuth;
        float lightSharpness;
        float lightStrength;

        float distortionStrength;
        float reflectionRatio;
        float refractionRatio;

        float waveFrequency;
        glm::vec2 waveOffset;
        float waveSpeed;
        float waveStrength;
    };
}
