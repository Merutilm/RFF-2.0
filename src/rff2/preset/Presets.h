//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include <string>
#include <array>
#include "../attr/CalMPAAttribute.h"
#include "../attr/CalReferenceCompAttribute.h"
#include "../attr/RenderAttribute.h"
#include "../attr/CalcAttribute.h"
#include "../attr/ShdPaletteAttribute.h"
#include "../attr/ShdStripeAttribute.h"
#include "../attr/ShdSlopeAttribute.h"
#include "../attr/ShdColorAttribute.h"
#include "../attr/ShdFogAttribute.h"
#include "../attr/ShdBloomAttribute.h"


namespace merutilm::rff2 {
    struct Preset {
        virtual ~Preset() = default;

        virtual std::string getName() const = 0;
    };


    namespace Presets {
        struct CalculationPresets : public Preset {
            ~CalculationPresets() override = default;

            virtual CalMPAAttribute genMPA() const = 0;

            virtual CalReferenceCompAttribute genReferenceCompression() const = 0;
        };

        struct RenderPresets : public Preset {
            ~RenderPresets() override = default;

            virtual RenderAttribute genRender() const = 0;
        };

        struct ResolutionPresets : public Preset {
            ~ResolutionPresets() override = default;

            virtual std::array<int, 2> genResolution() const = 0;
        };

        struct ShaderPreset : public Preset {
            ~ShaderPreset() override = default;
        };

        namespace ShaderPresets {
            struct PalettePreset : public ShaderPreset {
                ~PalettePreset() override = default;

                virtual ShdPaletteAttribute genPalette() const = 0;
            };

            struct StripePreset : public ShaderPreset {
                ~StripePreset() override = default;

                virtual ShdStripeAttribute genStripe() const = 0;
            };

            struct SlopePreset : public ShaderPreset {
                ~SlopePreset() override = default;

                virtual ShdSlopeAttribute genSlope() const = 0;
            };

            struct ColorPreset : public ShaderPreset {
                ~ColorPreset() override = default;

                virtual ShdColorAttribute genColor() const = 0;
            };

            struct FogPreset : public ShaderPreset {
                ~FogPreset() override = default;

                virtual ShdFogAttribute genFog() const = 0;
            };

            struct BloomPreset : public ShaderPreset {
                ~BloomPreset() override = default;

                virtual ShdBloomAttribute genBloom() const = 0;
            };
        }
    }
}