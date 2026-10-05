#pragma once

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
        float waveSpeed;
        float waveStrength;
    };
}
