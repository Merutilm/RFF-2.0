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
        struct CalculationPreset : public Preset {
            ~CalculationPreset() override = default;
        };

        namespace CalculationPresets {
            struct ReferenceSyncPreset : public CalculationPreset {
                ~ReferenceSyncPreset() override = default;

                [[nodiscard]] virtual FrtReferenceSyncSettings genRefSync() const = 0;

            };
            struct ApproxPreset : public CalculationPreset {
                ~ApproxPreset() override = default;

                [[nodiscard]] virtual FrtMPASettings genMPA() const = 0;
            };
            struct CompressPreset : public CalculationPreset {
                ~CompressPreset() override = default;

                [[nodiscard]] virtual FrtMPASettings genMPA() const = 0;

                [[nodiscard]] virtual FrtReferenceCompSettings genRefComp() const = 0;
            };
        }
        struct RenderPreset : public Preset {
            ~RenderPreset() override = default;
        };
        namespace RenderPresets {
            struct DisplayPreset : public RenderPreset {
                ~DisplayPreset() override = default;

                [[nodiscard]] virtual RndDisplaySettings genDisplay() const = 0;
            };


            struct ComputeShaderPreset : public RenderPreset {
                ~ComputeShaderPreset() override = default;

                [[nodiscard]] virtual RndComputeShader genComputeShader() const = 0;
            };
        }

        struct ResolutionPreset : public Preset {
            ~ResolutionPreset() override = default;

            [[nodiscard]] virtual std::array<int, 2> genResolution() const = 0;
        };

        struct ShaderPreset : public Preset {
            ~ShaderPreset() override = default;
        };

        namespace ShaderPresets {
            struct PalettePreset : public ShaderPreset {
                ~PalettePreset() override = default;

                [[nodiscard]] virtual ShdPaletteSettings genPalette() const = 0;
            };

            struct StripePreset : public ShaderPreset {
                ~StripePreset() override = default;

                [[nodiscard]] virtual ShdStripeSettings genStripe() const = 0;
            };

            struct SurfacePreset : public ShaderPreset {
                ~SurfacePreset() override = default;

                [[nodiscard]] virtual ShdSurfaceSettings genSurface() const = 0;
            };

            struct ColorPreset : public ShaderPreset {
                ~ColorPreset() override = default;

                [[nodiscard]] virtual ShdColorSettings genColor() const = 0;
            };

            struct FogPreset : public ShaderPreset {
                ~FogPreset() override = default;

                [[nodiscard]] virtual ShdFogSettings genFog() const = 0;
            };

            struct BloomPreset : public ShaderPreset {
                ~BloomPreset() override = default;

                [[nodiscard]] virtual ShdBloomSettings genBloom() const = 0;
            };
        }
    }
}