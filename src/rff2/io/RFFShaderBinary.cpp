//
// Created by Merutilm on 9/27/26.
//

#include "RFFShaderBinary.hpp"

#include "../preset/shader/bloom/ShdBloomPresets.hpp"
#include "../preset/shader/color/ShdColorPresets.hpp"
#include "../preset/shader/fog/ShdFogPresets.hpp"
#include "../preset/shader/palette/ShdPalettePresets.hpp"
#include "../preset/shader/slope/ShdSlopePresets.hpp"
#include "../preset/shader/stripe/ShdStripePresets.hpp"
#include "vulkan_helper/base/vkh.hpp"

namespace merutilm::rff2 {

    const RFFShaderBinary RFFShaderBinary::DEFAULT =
            RFFShaderBinary{ShaderSettings{.palette = ShdPalettePresets::Classic1().genPalette(),
                                           .stripe = ShdStripePresets::Disabled().genStripe(),
                                           .slope = ShdSlopePresets::Disabled().genSlope(),
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
        auto &palette = shaderSettings.palette;

        uint64_t len;
        IOUtilities::readAndDecode(in, &len);
        palette.colors.resize(len);
        IOUtilities::readAndDecode(in, &palette.colors);
        IOUtilities::readAndDecode(in, &palette.iterationColoring);
        IOUtilities::readAndDecode(in, &palette.singleIterationColoring);
        IOUtilities::readAndDecode(in, &palette.iterationInterval);
        IOUtilities::readAndDecode(in, &palette.offsetRatio);
        IOUtilities::readAndDecode(in, &palette.animationSpeed);

        auto &stripe = shaderSettings.stripe;
        IOUtilities::readAndDecode(in, &stripe.stripeType);
        IOUtilities::readAndDecode(in, &stripe.firstInterval);
        IOUtilities::readAndDecode(in, &stripe.secondInterval);
        IOUtilities::readAndDecode(in, &stripe.opacity);
        IOUtilities::readAndDecode(in, &stripe.offset);
        IOUtilities::readAndDecode(in, &stripe.animationSpeed);
        IOUtilities::readAndDecode(in, &stripe.iterationColoring);

        auto &slope = shaderSettings.slope;
        IOUtilities::readAndDecode(in, &slope.depth);
        IOUtilities::readAndDecode(in, &slope.reflectionRatio);
        IOUtilities::readAndDecode(in, &slope.opacity);
        IOUtilities::readAndDecode(in, &slope.zenith);
        IOUtilities::readAndDecode(in, &slope.azimuth);

        auto &color = shaderSettings.color;
        IOUtilities::readAndDecode(in, &color.gamma);
        IOUtilities::readAndDecode(in, &color.exposure);
        IOUtilities::readAndDecode(in, &color.hue);
        IOUtilities::readAndDecode(in, &color.saturation);
        IOUtilities::readAndDecode(in, &color.brightness);
        IOUtilities::readAndDecode(in, &color.contrast);

        auto &fog = shaderSettings.fog;
        IOUtilities::readAndDecode(in, &fog.radius);
        IOUtilities::readAndDecode(in, &fog.opacity);

        auto &bloom = shaderSettings.bloom;
        IOUtilities::readAndDecode(in, &bloom.threshold);
        IOUtilities::readAndDecode(in, &bloom.radius);
        IOUtilities::readAndDecode(in, &bloom.softness);
        IOUtilities::readAndDecode(in, &bloom.intensity);

        auto &noiseReduction = shaderSettings.noiseReduction;
        IOUtilities::readAndDecode(in, &noiseReduction.use);
        IOUtilities::readAndDecode(in, &noiseReduction.similarCountThreshold);
        IOUtilities::readAndDecode(in, &noiseReduction.differenceThreshold);

        auto &fractal3d = shaderSettings.fractal3D;
        IOUtilities::readAndDecode(in, &fractal3d.use);
        IOUtilities::readAndDecode(in, &fractal3d.altitude);
        IOUtilities::readAndDecode(in, &fractal3d.rotation);
        IOUtilities::readAndDecode(in, &fractal3d.distance);
        IOUtilities::readAndDecode(in, &fractal3d.baseIteration);
        IOUtilities::readAndDecode(in, &fractal3d.depthDivisor);


        return RFFShaderBinary(std::move(shaderSettings));

    }

    void RFFShaderBinary::write(std::ofstream &out) const {

        const auto &palette = shaderSettings.palette;
        IOUtilities::encodeAndWrite(out, palette.colors.size());
        IOUtilities::encodeAndWrite(out, palette.colors);
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

        const auto &slope = shaderSettings.slope;
        IOUtilities::encodeAndWrite(out, slope.depth);
        IOUtilities::encodeAndWrite(out, slope.reflectionRatio);
        IOUtilities::encodeAndWrite(out, slope.opacity);
        IOUtilities::encodeAndWrite(out, slope.zenith);
        IOUtilities::encodeAndWrite(out, slope.azimuth);

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
