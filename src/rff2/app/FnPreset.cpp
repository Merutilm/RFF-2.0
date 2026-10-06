//
// Created by Merutilm on 7/14/26.
//

#include "FnPreset.hpp"

#include "../preset/calc/approx/ClcApproxPresets.hpp"
#include "../preset/calc/compress/ClcCompressPresets.hpp"
#include "../preset/calc/sync/ClcSyncPresets.hpp"
#include "../preset/render/compute/RndComputePresets.hpp"
#include "../preset/render/display/RndDisplayPresets.hpp"
#include "../preset/resolution/ResolutionPresets.hpp"
#include "../preset/shader/bloom/ShdBloomPresets.hpp"
#include "../preset/shader/color/ShdColorPresets.hpp"
#include "../preset/shader/fog/ShdFogPresets.hpp"
#include "../preset/shader/palette/ShdPalettePresets.hpp"
#include "../preset/shader/stripe/ShdStripePresets.hpp"
#include "../preset/shader/surface/ShdSurfacePresets.hpp"

namespace merutilm::rff2 {


    void FnPreset::calculation(RFF2 &app) {
        ImGui::SeparatorText("Calculation");
        ImGui::Indent();
        beginPresetExecutor("Approximation");
        addPresetExecutor(app, ClcApproxPresets::UltraFast());
        addPresetExecutor(app, ClcApproxPresets::Fast());
        addPresetExecutor(app, ClcApproxPresets::Normal());
        addPresetExecutor(app, ClcApproxPresets::Best());
        addPresetExecutor(app, ClcApproxPresets::UltraBest());
        addPresetExecutor(app, ClcApproxPresets::LightSpirals());
        addPresetExecutor(app, ClcApproxPresets::DenseSpirals());
        addPresetExecutor(app, ClcApproxPresets::ExtremelyDenseSpirals());

        nextPresetExecutor("Compression");
        addPresetExecutor(app, ClcCompressPresets::None());
        addPresetExecutor(app, ClcCompressPresets::Stable());
        addPresetExecutor(app, ClcCompressPresets::MoreStable());
        addPresetExecutor(app, ClcCompressPresets::UltraStable());

        nextPresetExecutor("Synchronization");
        addPresetExecutor(app, ClcSyncPresets::Fast());
        addPresetExecutor(app, ClcSyncPresets::Normal());
        addPresetExecutor(app, ClcSyncPresets::Best());
        endPresetExecutor();

        ImGui::Unindent();
    }
    void FnPreset::render(RFF2 &app) {
        ImGui::SeparatorText("Render");
        ImGui::Indent();
        beginPresetExecutor("Display");
        addPresetExecutor(app, RndDisplayPresets::Potato());
        addPresetExecutor(app, RndDisplayPresets::Low());
        addPresetExecutor(app, RndDisplayPresets::Medium());
        addPresetExecutor(app, RndDisplayPresets::High());
        addPresetExecutor(app, RndDisplayPresets::Ultra());
        addPresetExecutor(app, RndDisplayPresets::Extreme());

        nextPresetExecutor("Compute Shader");
        addPresetExecutor(app, RndComputePresets::None());
        addPresetExecutor(app, RndComputePresets::General());
        addPresetExecutor(app, RndComputePresets::LightZoomSpirals());
        addPresetExecutor(app, RndComputePresets::DeepZoomSpirals());
        endPresetExecutor();

        ImGui::Unindent();
    }
    void FnPreset::resolution(RFF2 &app) {
        beginPresetExecutor("Resolution");
        ImGui::Indent();
        addPresetExecutor(app, ResolutionPresets::L1());
        addPresetExecutor(app, ResolutionPresets::L2());
        addPresetExecutor(app, ResolutionPresets::L3());
        addPresetExecutor(app, ResolutionPresets::L4());
        addPresetExecutor(app, ResolutionPresets::L5());
        endPresetExecutor();
        ImGui::Unindent();
    }
    void FnPreset::shader(RFF2 &app) {
        ImGui::SeparatorText("Shader");
        ImGui::Indent();
        beginPresetExecutor("Palette");
        addPresetExecutor(app, ShdPalettePresets::Classic1());
        addPresetExecutor(app, ShdPalettePresets::Classic2());
        addPresetExecutor(app, ShdPalettePresets::Azure());
        addPresetExecutor(app, ShdPalettePresets::Cinematic());
        addPresetExecutor(app, ShdPalettePresets::Desert());
        addPresetExecutor(app, ShdPalettePresets::Flame());
        addPresetExecutor(app, ShdPalettePresets::LongRandom64());
        addPresetExecutor(app, ShdPalettePresets::LongRainbow7());
        addPresetExecutor(app, ShdPalettePresets::Rainbow());

        nextPresetExecutor("Stripe");
        addPresetExecutor(app, ShdStripePresets::Disabled());
        addPresetExecutor(app, ShdStripePresets::SlowAnimated());
        addPresetExecutor(app, ShdStripePresets::FastAnimated());
        addPresetExecutor(app, ShdStripePresets::Smooth());
        addPresetExecutor(app, ShdStripePresets::SmoothTranslucent());

        nextPresetExecutor("Surface");
        addPresetExecutor(app, ShdSurfacePresets::Disabled());
        addPresetExecutor(app, ShdSurfacePresets::HighContrast());
        addPresetExecutor(app, ShdSurfacePresets::Reflective());
        addPresetExecutor(app, ShdSurfacePresets::Translucent());
        addPresetExecutor(app, ShdSurfacePresets::Reversed());
        addPresetExecutor(app, ShdSurfacePresets::Micro());
        addPresetExecutor(app, ShdSurfacePresets::Nano());

        nextPresetExecutor("Color");
        addPresetExecutor(app, ShdColorPresets::Disabled());
        addPresetExecutor(app, ShdColorPresets::WeakContrast());
        addPresetExecutor(app, ShdColorPresets::HighContrast());
        addPresetExecutor(app, ShdColorPresets::Dull());
        addPresetExecutor(app, ShdColorPresets::Vivid());

        nextPresetExecutor("Fog");
        addPresetExecutor(app, ShdFogPresets::Disabled());
        addPresetExecutor(app, ShdFogPresets::Low());
        addPresetExecutor(app, ShdFogPresets::Medium());
        addPresetExecutor(app, ShdFogPresets::High());
        addPresetExecutor(app, ShdFogPresets::Ultra());

        nextPresetExecutor("Bloom");
        addPresetExecutor(app, ShdBloomPresets::Disabled());
        addPresetExecutor(app, ShdBloomPresets::Highlighted());
        addPresetExecutor(app, ShdBloomPresets::HighlightedStrong());
        addPresetExecutor(app, ShdBloomPresets::Weak());
        addPresetExecutor(app, ShdBloomPresets::Normal());
        addPresetExecutor(app, ShdBloomPresets::Strong());
        endPresetExecutor();
        ImGui::Unindent();
    }
} // namespace merutilm::rff2
