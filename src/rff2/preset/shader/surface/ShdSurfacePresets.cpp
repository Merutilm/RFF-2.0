//
// Created by Merutilm on 2025-05-28.
//
#include "ShdSurfacePresets.hpp"


namespace merutilm::rff2::ShdSurfacePresets {

    std::string Disabled::getName() const { return "Disabled"; }

    ShdSurfaceSettings Disabled::genSurface() const {
        return ShdSurfaceSettings{.depth = 0,
                                  .shadowBrightness = 0,
                                  .shadowOpacity = 1.0f,
                                  .lightZenith = 30,
                                  .lightAzimuth = 135,
                                  .lightSharpness = 64,
                                  .lightStrength = 1,
                                  .distortionStrength = 0,
                                  .reflectionRatio = 0.04f,
                                  .refractionRatio = 1.5f,
                                  .waveFrequency = 1,
                                  .waveSpeed = 1,
                                  .waveStrength = 0};
    }

    std::string HighContrast::getName() const { return "High Contrast"; }

    ShdSurfaceSettings HighContrast::genSurface() const {
        return ShdSurfaceSettings{.depth = 300,
                                  .shadowBrightness = 0,
                                  .shadowOpacity = 1.0f,
                                  .lightZenith = 0,
                                  .lightAzimuth = 135,
                                  .lightSharpness = 64,
                                  .lightStrength = 1,
                                  .distortionStrength = 0,
                                  .reflectionRatio = 0.04f,
                                  .refractionRatio = 1.5f,
                                  .waveFrequency = 1,
                                  .waveSpeed = 1,
                                  .waveStrength = 0};
    }

    std::string Reflective::getName() const { return "Reflective"; }

    ShdSurfaceSettings Reflective::genSurface() const {
        return ShdSurfaceSettings{.depth = 300,
                                  .shadowBrightness = 0.5f,
                                  .shadowOpacity = 1.0f,
                                  .lightZenith = 0,
                                  .lightAzimuth = 135,
                                  .lightSharpness = 64,
                                  .lightStrength = 1,
                                  .distortionStrength = 0,
                                  .reflectionRatio = 0.04f,
                                  .refractionRatio = 1.5f,
                                  .waveFrequency = 1,
                                  .waveSpeed = 1,
                                  .waveStrength = 0};
    }


    std::string Translucent::getName() const { return "Translucent"; }

    ShdSurfaceSettings Translucent::genSurface() const {
        return ShdSurfaceSettings{.depth = 300,
                                  .shadowBrightness = 0,
                                  .shadowOpacity = 0.5f,
                                  .lightZenith = 0,
                                  .lightAzimuth = 135,
                                  .lightSharpness = 64,
                                  .lightStrength = 1,
                                  .distortionStrength = 0,
                                  .reflectionRatio = 0.04f,
                                  .refractionRatio = 1.5f,
                                  .waveFrequency = 1,
                                  .waveSpeed = 1,
                                  .waveStrength = 0};
    }

    std::string Reversed::getName() const { return "Reversed"; }

    ShdSurfaceSettings Reversed::genSurface() const {
        return ShdSurfaceSettings{.depth = -300,
                                  .shadowBrightness = 0,
                                  .shadowOpacity = 0.5f,
                                  .lightZenith = 0,
                                  .lightAzimuth = 135,
                                  .lightSharpness = 64,
                                  .lightStrength = 1,
                                  .distortionStrength = 0,
                                  .reflectionRatio = 0.04f,
                                  .refractionRatio = 1.5f,
                                  .waveFrequency = 1,
                                  .waveSpeed = 1,
                                  .waveStrength = 0};
    }

    std::string Micro::getName() const { return "Micro"; }

    ShdSurfaceSettings Micro::genSurface() const {
        return ShdSurfaceSettings{.depth = 3,
                                  .shadowBrightness = 0,
                                  .shadowOpacity = 0.5f,
                                  .lightZenith = 0,
                                  .lightAzimuth = 135,
                                  .lightSharpness = 64,
                                  .lightStrength = 1,
                                  .distortionStrength = 0,
                                  .reflectionRatio = 0.04f,
                                  .refractionRatio = 1.5f,
                                  .waveFrequency = 1,
                                  .waveSpeed = 1,
                                  .waveStrength = 0};
    }

    std::string Nano::getName() const { return "Nano"; }

    ShdSurfaceSettings Nano::genSurface() const {
        return ShdSurfaceSettings{.depth = 0.003f,
                                  .shadowBrightness = 0,
                                  .shadowOpacity = 0.5f,
                                  .lightZenith = 0,
                                  .lightAzimuth = 135,
                                  .lightSharpness = 64,
                                  .lightStrength = 1,
                                  .distortionStrength = 0,
                                  .reflectionRatio = 0.04f,
                                  .refractionRatio = 1.5f,
                                  .waveFrequency = 1,
                                  .waveSpeed = 1,
                                  .waveStrength = 0};
    }
} // namespace merutilm::rff2::ShdSurfacePresets
