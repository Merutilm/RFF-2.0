//
// Created by Merutilm on 9/27/26.
//

#include "RFFShaderBinary.hpp"

#include "../preset/shader/bloom/ShdBloomPresets.hpp"
#include "../preset/shader/color/ShdColorPresets.hpp"
#include "../preset/shader/fog/ShdFogPresets.hpp"
#include "../preset/shader/palette/ShdPalettePresets.hpp"
#include "../preset/shader/stripe/ShdStripePresets.hpp"
#include "../preset/shader/surface/ShdSurfacePresets.hpp"
#include "vulkan_helper/base/vkh.hpp"

namespace merutilm::rff2 {

    const RFFShaderBinary RFFShaderBinary::DEFAULT =
            RFFShaderBinary{ShaderSettings{.palette = ShdPalettePresets::Classic1().genPalette(),
                                           .stripe = ShdStripePresets::Disabled().genStripe(),
                                           .surface = ShdSurfacePresets::Disabled().genSurface(),
                                           .color = ShdColorPresets::Disabled().genColor(),
                                           .fog = ShdFogPresets::Disabled().genFog(),
                                           .bloom = ShdBloomPresets::Disabled().genBloom(),
                                           .noiseReduction = {true, 2, 0.1f},
                                           .fractal3D = {false, 85, 0, 1, 0, 10.f}

            }};

    RFFShaderBinary::RFFShaderBinary(ShaderSettings shaderSettings) : shaderSettings(std::move(shaderSettings)) {
        static_assert(RFFBinaryRequirements<RFFShaderBinary>);
    }

    RFFShaderBinary RFFShaderBinary::read(std::ifstream &in) {

        const uint32_t version = readVersion(in);
        assert(version > 0);

        ShaderSettings shaderSettings;

        readPaletteSettings(in, shaderSettings.palette, version);
        readStripeSettings(in, shaderSettings.stripe);
        readSurfaceSettings(in, shaderSettings.surface, version);
        readColorSettings(in, shaderSettings.color);
        readFogSettings(in, shaderSettings.fog);
        readBloomSettings(in, shaderSettings.bloom);
        readNoiseReductionSettings(in, shaderSettings.noiseReduction);
        readFractal3DSettings(in, shaderSettings.fractal3D);

        return RFFShaderBinary(std::move(shaderSettings));
    }

    void RFFShaderBinary::readPaletteSettings(std::ifstream &in, ShdPaletteSettings &palette, uint32_t version) {

        uint64_t len;
        IOUtilities::readAndDecode(in, &len);

        palette.colors.resize(len);
        IOUtilities::readAndDecode(in, &palette.colors);
        if (version <= 3) {
            palette.interiorColor = glm::vec4{0, 0, 0, 1};
        } else {
            IOUtilities::readAndDecode(in, &palette.interiorColor);
        }
        IOUtilities::readAndDecode(in, &palette.iterationColoring);
        IOUtilities::readAndDecode(in, &palette.singleIterationColoring);
        IOUtilities::readAndDecode(in, &palette.iterationInterval);
        IOUtilities::readAndDecode(in, &palette.offsetRatio);
        IOUtilities::readAndDecode(in, &palette.animationSpeed);
    }

    void RFFShaderBinary::readStripeSettings(std::ifstream &in, ShdStripeSettings &stripe) {

        IOUtilities::readAndDecode(in, &stripe.stripeType);
        IOUtilities::readAndDecode(in, &stripe.firstInterval);
        IOUtilities::readAndDecode(in, &stripe.secondInterval);
        IOUtilities::readAndDecode(in, &stripe.opacity);
        IOUtilities::readAndDecode(in, &stripe.offset);
        IOUtilities::readAndDecode(in, &stripe.animationSpeed);
        IOUtilities::readAndDecode(in, &stripe.iterationColoring);
    }

    void RFFShaderBinary::readSurfaceSettings(std::ifstream &in, ShdSurfaceSettings &surface, uint32_t version) {

        IOUtilities::readAndDecode(in, &surface.depth);
        IOUtilities::readAndDecode(in, &surface.shadowBrightness);
        IOUtilities::readAndDecode(in, &surface.shadowOpacity);
        IOUtilities::readAndDecode(in, &surface.lightZenith);
        IOUtilities::readAndDecode(in, &surface.lightAzimuth);

        if (version <= 1) {
            surface.lightSharpness = 64;
            surface.lightStrength = 0;
            surface.distortionStrength = 0;
            surface.reflectionRatio = 0.04f;
            surface.refractionRatio = 1.5f;
            surface.waveFrequency = 1;
        } else {
            IOUtilities::readAndDecode(in, &surface.lightSharpness);
            IOUtilities::readAndDecode(in, &surface.lightStrength);
            IOUtilities::readAndDecode(in, &surface.distortionStrength);
            IOUtilities::readAndDecode(in, &surface.reflectionRatio);
            IOUtilities::readAndDecode(in, &surface.refractionRatio);
            IOUtilities::readAndDecode(in, &surface.waveFrequency);
        }

        if (version <= 2) {
            surface.waveOffset = {};
        } else {
            IOUtilities::readAndDecode(in, &surface.waveOffset);
        }

        if (version <= 1) {
            surface.waveSpeed = 1;
            surface.waveStrength = 0;
        } else {
            IOUtilities::readAndDecode(in, &surface.waveSpeed);
            IOUtilities::readAndDecode(in, &surface.waveStrength);
        }
    }

    void RFFShaderBinary::readColorSettings(std::ifstream &in, ShdColorSettings &color) {

        IOUtilities::readAndDecode(in, &color.gamma);
        IOUtilities::readAndDecode(in, &color.exposure);
        IOUtilities::readAndDecode(in, &color.hue);
        IOUtilities::readAndDecode(in, &color.saturation);
        IOUtilities::readAndDecode(in, &color.brightness);
        IOUtilities::readAndDecode(in, &color.contrast);
    }

    void RFFShaderBinary::readFogSettings(std::ifstream &in, ShdFogSettings &fog) {

        IOUtilities::readAndDecode(in, &fog.radius);
        IOUtilities::readAndDecode(in, &fog.opacity);
    }

    void RFFShaderBinary::readBloomSettings(std::ifstream &in, ShdBloomSettings &bloom) {

        IOUtilities::readAndDecode(in, &bloom.threshold);
        IOUtilities::readAndDecode(in, &bloom.radius);
        IOUtilities::readAndDecode(in, &bloom.softness);
        IOUtilities::readAndDecode(in, &bloom.intensity);
    }

    void RFFShaderBinary::readNoiseReductionSettings(std::ifstream &in,

                                                     ShdNoiseReductionSettings &noiseReduction) {

        IOUtilities::readAndDecode(in, &noiseReduction.use);
        IOUtilities::readAndDecode(in, &noiseReduction.similarCountThreshold);
        IOUtilities::readAndDecode(in, &noiseReduction.differenceThreshold);
    }

    void RFFShaderBinary::readFractal3DSettings(std::ifstream &in, ShdFractal3DSettings &fractal3d) {

        IOUtilities::readAndDecode(in, &fractal3d.use);
        IOUtilities::readAndDecode(in, &fractal3d.altitude);
        IOUtilities::readAndDecode(in, &fractal3d.rotation);
        IOUtilities::readAndDecode(in, &fractal3d.distance);
        IOUtilities::readAndDecode(in, &fractal3d.baseIteration);
        IOUtilities::readAndDecode(in, &fractal3d.depthDivisor);
    }


    void RFFShaderBinary::write(std::ofstream &out) const {

        const auto &palette = shaderSettings.palette;
        IOUtilities::encodeAndWrite(out, palette.colors.size());
        IOUtilities::encodeAndWrite(out, palette.colors);
        IOUtilities::encodeAndWrite(out, palette.interiorColor);
        IOUtilities::encodeAndWrite(out, palette.iterationColoring);
        IOUtilities::encodeAndWrite(out, palette.singleIterationColoring);
        IOUtilities::encodeAndWrite(out, palette.iterationInterval);
        IOUtilities::encodeAndWrite(out, palette.offsetRatio);
        IOUtilities::encodeAndWrite(out, palette.animationSpeed);

        const auto &stripe = shaderSettings.stripe;
        IOUtilities::encodeAndWrite(out, stripe.stripeType);
        IOUtilities::encodeAndWrite(out, stripe.firstInterval);
        IOUtilities::encodeAndWrite(out, stripe.secondInterval);
        IOUtilities::encodeAndWrite(out, stripe.opacity);
        IOUtilities::encodeAndWrite(out, stripe.offset);
        IOUtilities::encodeAndWrite(out, stripe.animationSpeed);
        IOUtilities::encodeAndWrite(out, stripe.iterationColoring);

        const auto &surface = shaderSettings.surface;
        IOUtilities::encodeAndWrite(out, surface.depth);
        IOUtilities::encodeAndWrite(out, surface.shadowBrightness);
        IOUtilities::encodeAndWrite(out, surface.shadowOpacity);
        IOUtilities::encodeAndWrite(out, surface.lightZenith);
        IOUtilities::encodeAndWrite(out, surface.lightAzimuth);
        IOUtilities::encodeAndWrite(out, surface.lightSharpness);
        IOUtilities::encodeAndWrite(out, surface.lightStrength);
        IOUtilities::encodeAndWrite(out, surface.distortionStrength);
        IOUtilities::encodeAndWrite(out, surface.reflectionRatio);
        IOUtilities::encodeAndWrite(out, surface.refractionRatio);
        IOUtilities::encodeAndWrite(out, surface.waveFrequency);
        IOUtilities::encodeAndWrite(out, surface.waveOffset);
        IOUtilities::encodeAndWrite(out, surface.waveSpeed);
        IOUtilities::encodeAndWrite(out, surface.waveStrength);


        const auto &color = shaderSettings.color;
        IOUtilities::encodeAndWrite(out, color.gamma);
        IOUtilities::encodeAndWrite(out, color.exposure);
        IOUtilities::encodeAndWrite(out, color.hue);
        IOUtilities::encodeAndWrite(out, color.saturation);
        IOUtilities::encodeAndWrite(out, color.brightness);
        IOUtilities::encodeAndWrite(out, color.contrast);

        const auto &fog = shaderSettings.fog;
        IOUtilities::encodeAndWrite(out, fog.radius);
        IOUtilities::encodeAndWrite(out, fog.opacity);

        const auto &bloom = shaderSettings.bloom;
        IOUtilities::encodeAndWrite(out, bloom.threshold);
        IOUtilities::encodeAndWrite(out, bloom.radius);
        IOUtilities::encodeAndWrite(out, bloom.softness);
        IOUtilities::encodeAndWrite(out, bloom.intensity);

        const auto &noiseReduction = shaderSettings.noiseReduction;
        IOUtilities::encodeAndWrite(out, noiseReduction.use);
        IOUtilities::encodeAndWrite(out, noiseReduction.similarCountThreshold);
        IOUtilities::encodeAndWrite(out, noiseReduction.differenceThreshold);

        const auto &fractal3d = shaderSettings.fractal3D;
        IOUtilities::encodeAndWrite(out, fractal3d.use);
        IOUtilities::encodeAndWrite(out, fractal3d.altitude);
        IOUtilities::encodeAndWrite(out, fractal3d.rotation);
        IOUtilities::encodeAndWrite(out, fractal3d.distance);
        IOUtilities::encodeAndWrite(out, fractal3d.baseIteration);
        IOUtilities::encodeAndWrite(out, fractal3d.depthDivisor);
    }

} // namespace merutilm::rff2
