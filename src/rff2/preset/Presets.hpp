//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include <array>
#include <string>
#include "../settings/FrtMPASettings.h"
#include "../settings/FrtReferenceCompSettings.h"
#include "../settings/FrtReferenceSyncSettings.hpp"
#include "../settings/RenderSettings.h"
#include "../settings/ShdBloomSettings.h"
#include "../settings/ShdColorSettings.h"
#include "../settings/ShdFogSettings.h"
#include "../settings/ShdPaletteSettings.h"
#include "../settings/ShdStripeSettings.h"
#include "../settings/ShdSurfaceSettings.h"


namespace merutilm::rff2 {
    struct Preset {
        virtual ~Preset() = default;

        [[nodiscard]] virtual std::string getName() const = 0;
    };


    namespace Presets {
        struct CalculationPreset : Preset {
            ~CalculationPreset() override = default;
        };

        namespace CalculationPresets {
            struct ReferenceSyncPreset : CalculationPreset {
                ~ReferenceSyncPreset() override = default;

                [[nodiscard]] virtual FrtReferenceSyncSettings genRefSync() const = 0;

            };
            struct ApproxPreset : CalculationPreset {
                ~ApproxPreset() override = default;

                [[nodiscard]] virtual FrtMPASettings genMPA() const = 0;
            };
            struct CompressPreset : CalculationPreset {
                ~CompressPreset() override = default;

                [[nodiscard]] virtual FrtReferenceCompSettings genRefComp() const = 0;
            };
        }
        struct RenderPreset : Preset {
            ~RenderPreset() override = default;
        };
        namespace RenderPresets {
            struct DisplayPreset : RenderPreset {
                ~DisplayPreset() override = default;

                [[nodiscard]] virtual RndDisplaySettings genDisplay() const = 0;
            };


            struct ComputeShaderPreset : RenderPreset {
                ~ComputeShaderPreset() override = default;

                [[nodiscard]] virtual RndComputeShader genComputeShader() const = 0;
            };
        }

        struct ResolutionPreset : Preset {
            ~ResolutionPreset() override = default;

            [[nodiscard]] virtual std::array<int, 2> genResolution() const = 0;
        };

        struct ShaderPreset : Preset {
            ~ShaderPreset() override = default;
        };

        namespace ShaderPresets {
            struct PalettePreset : ShaderPreset {
                ~PalettePreset() override = default;

                [[nodiscard]] virtual ShdPaletteSettings genPalette() const = 0;
            };

            struct StripePreset : ShaderPreset {
                ~StripePreset() override = default;

                [[nodiscard]] virtual ShdStripeSettings genStripe() const = 0;
            };

            struct SurfacePreset : ShaderPreset {
                ~SurfacePreset() override = default;

                [[nodiscard]] virtual ShdSurfaceSettings genSurface() const = 0;
            };

            struct ColorPreset : ShaderPreset {
                ~ColorPreset() override = default;

                [[nodiscard]] virtual ShdColorSettings genColor() const = 0;
            };

            struct FogPreset : ShaderPreset {
                ~FogPreset() override = default;

                [[nodiscard]] virtual ShdFogSettings genFog() const = 0;
            };

            struct BloomPreset : ShaderPreset {
                ~BloomPreset() override = default;

                [[nodiscard]] virtual ShdBloomSettings genBloom() const = 0;
            };
        }
    }
}