//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include <string>
#include <array>
#include "../settings/MPASettings.h"
#include "../settings/ReferenceCompressionSettings.h"
#include "../settings/RenderSettings.h"
#include "../settings/CalculationSettings.h"
#include "../settings/PaletteSettings.h"
#include "../settings/StripeSettings.h"
#include "../settings/SlopeSettings.h"
#include "../settings/ColorSettings.h"
#include "../settings/FogSettings.h"
#include "../settings/BloomSettings.h"


namespace merutilm::rff2 {
    struct Preset {
        virtual ~Preset() = default;

        virtual std::string getName() const = 0;
    };


    namespace Presets {
        struct CalculationPresets : public Preset {
            ~CalculationPresets() override = default;

            virtual MPASettings mpaSettings() const = 0;

            virtual ReferenceCompressionSettings referenceCompressionSettings() const = 0;
        };

        struct RenderPresets : public Preset {
            ~RenderPresets() override = default;

            virtual RenderSettings renderSettings() const = 0;
        };

        struct ResolutionPresets : public Preset {
            ~ResolutionPresets() override = default;

            virtual std::array<int, 2> getResolution() const = 0;
        };

        struct ShaderPreset : public Preset {
            ~ShaderPreset() override = default;
        };

        namespace ShaderPresets {
            struct PalettePreset : public ShaderPreset {
                ~PalettePreset() override = default;

                virtual PaletteSettings paletteSettings() const = 0;
            };

            struct StripePreset : public ShaderPreset {
                ~StripePreset() override = default;

                virtual StripeSettings stripeSettings() const = 0;
            };

            struct SlopePreset : public ShaderPreset {
                ~SlopePreset() override = default;

                virtual SlopeSettings slopeSettings() const = 0;
            };

            struct ColorPreset : public ShaderPreset {
                ~ColorPreset() override = default;

                virtual ColorSettings colorSettings() const = 0;
            };

            struct FogPreset : public ShaderPreset {
                ~FogPreset() override = default;

                virtual FogSettings fogSettings() const = 0;
            };

            struct BloomPreset : public ShaderPreset {
                ~BloomPreset() override = default;

                virtual BloomSettings bloomSettings() const = 0;
            };
        }
    }
}