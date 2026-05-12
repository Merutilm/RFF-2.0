//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include <array>
#include <string>
#include "../settings/FrtMPASettings.h"
#include "../settings/FrtReferenceSyncSettings.hpp"
#include "../settings/FrtReferenceCompSettings.h"
#include "../settings/RenderSettings.h"
#include "../settings/ShdBloomSettings.h"
#include "../settings/ShdColorSettings.h"
#include "../settings/ShdFogSettings.h"
#include "../settings/ShdPaletteSettings.h"
#include "../settings/ShdSlopeSettings.h"
#include "../settings/ShdStripeSettings.h"


namespace merutilm::rff2 {
    struct Preset {
        virtual ~Preset() = default;

        virtual std::wstring getName() const = 0;
    };


    namespace Presets {
        struct CalculationPreset : public Preset {
            ~CalculationPreset() override = default;

            virtual FrtReferenceSyncSettings genRefSync() const = 0;

            virtual FrtMPASettings genMPA() const = 0;

            virtual FrtReferenceCompSettings genRefComp() const = 0;
        };

        struct RenderPreset : public Preset {
            ~RenderPreset() override = default;

            virtual RenderSettings genRender() const = 0;
        };

        struct ResolutionPreset : public Preset {
            ~ResolutionPreset() override = default;

            virtual std::array<int, 2> genResolution() const = 0;
        };

        struct ShaderPreset : public Preset {
            ~ShaderPreset() override = default;
        };

        namespace ShaderPresets {
            struct PalettePreset : public ShaderPreset {
                ~PalettePreset() override = default;

                virtual ShdPaletteSettings genPalette() const = 0;
            };

            struct StripePreset : public ShaderPreset {
                ~StripePreset() override = default;

                virtual ShdStripeSettings genStripe() const = 0;
            };

            struct SlopePreset : public ShaderPreset {
                ~SlopePreset() override = default;

                virtual ShdSlopeSettings genSlope() const = 0;
            };

            struct ColorPreset : public ShaderPreset {
                ~ColorPreset() override = default;

                virtual ShdColorSettings genColor() const = 0;
            };

            struct FogPreset : public ShaderPreset {
                ~FogPreset() override = default;

                virtual ShdFogSettings genFog() const = 0;
            };

            struct BloomPreset : public ShaderPreset {
                ~BloomPreset() override = default;

                virtual ShdBloomSettings genBloom() const = 0;
            };
        }
    }
}