//
// Created by Merutilm on 7/14/26.
//

#include "FnPreset.hpp"

#include "../util/ImGuiUtils.hpp"
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
        ImGuiUtils::BeginSettings("Calculation");
        if (ImGuiUtils::BeginPresetExecutor("Approximation")) {
            ImGuiUtils::AddPresetExecutor(app, ClcApproxPresets::UltraFast());
            ImGuiUtils::AddPresetExecutor(app, ClcApproxPresets::Fast());
            ImGuiUtils::AddPresetExecutor(app, ClcApproxPresets::Normal());
            ImGuiUtils::AddPresetExecutor(app, ClcApproxPresets::Best());
            ImGuiUtils::AddPresetExecutor(app, ClcApproxPresets::UltraBest());
            ImGuiUtils::EndPresetExecutor();
        }


        if(ImGuiUtils::BeginPresetExecutor("Compression")) {
            ImGuiUtils::AddPresetExecutor(app, ClcCompressPresets::None());
            ImGuiUtils::AddPresetExecutor(app, ClcCompressPresets::SemiCompressed());
            ImGuiUtils::AddPresetExecutor(app, ClcCompressPresets::Compressed());
            ImGuiUtils::AddPresetExecutor(app, ClcCompressPresets::MoreCompressed());
            ImGuiUtils::EndPresetExecutor();
        }
        

        if (ImGuiUtils::BeginPresetExecutor("Synchronization")) {
            ImGuiUtils::AddPresetExecutor(app, ClcSyncPresets::Fast());
            ImGuiUtils::AddPresetExecutor(app, ClcSyncPresets::Normal());
            ImGuiUtils::AddPresetExecutor(app, ClcSyncPresets::Best());
            ImGuiUtils::EndPresetExecutor();
        }
        ImGuiUtils::EndSettings();
    }
    void FnPreset::render(RFF2 &app) {
        ImGuiUtils::BeginSettings("Render");
        if (ImGuiUtils::BeginPresetExecutor("Display")) {
            ImGuiUtils::AddPresetExecutor(app, RndDisplayPresets::Potato());
            ImGuiUtils::AddPresetExecutor(app, RndDisplayPresets::Low());
            ImGuiUtils::AddPresetExecutor(app, RndDisplayPresets::Medium());
            ImGuiUtils::AddPresetExecutor(app, RndDisplayPresets::High());
            ImGuiUtils::AddPresetExecutor(app, RndDisplayPresets::Ultra());
            ImGuiUtils::AddPresetExecutor(app, RndDisplayPresets::Extreme());
            ImGuiUtils::EndPresetExecutor();
        }
        
        if (ImGuiUtils::BeginPresetExecutor("Compute Shader")) {
            ImGuiUtils::AddPresetExecutor(app, RndComputePresets::None());
            ImGuiUtils::AddPresetExecutor(app, RndComputePresets::General());
            ImGuiUtils::AddPresetExecutor(app, RndComputePresets::LightZoomSpirals());
            ImGuiUtils::AddPresetExecutor(app, RndComputePresets::DeepZoomSpirals());
            ImGuiUtils::EndPresetExecutor();
        }
        ImGuiUtils::EndSettings();
    }
    void FnPreset::resolution(RFF2 &app) {
        if (ImGuiUtils::BeginPresetExecutor("Resolution")) {
            ImGuiUtils::AddPresetExecutor(app, ResolutionPresets::L1());
            ImGuiUtils::AddPresetExecutor(app, ResolutionPresets::L2());
            ImGuiUtils::AddPresetExecutor(app, ResolutionPresets::L3());
            ImGuiUtils::AddPresetExecutor(app, ResolutionPresets::L4());
            ImGuiUtils::AddPresetExecutor(app, ResolutionPresets::L5());
            ImGuiUtils::EndPresetExecutor();
        }
    }
    void FnPreset::shader(RFF2 &app) {
        ImGuiUtils::BeginSettings("Shader");
        if (ImGuiUtils::BeginPresetExecutor("Palette")) {
            ImGuiUtils::AddPresetExecutor(app, ShdPalettePresets::Classic1());
            ImGuiUtils::AddPresetExecutor(app, ShdPalettePresets::Classic2());
            ImGuiUtils::AddPresetExecutor(app, ShdPalettePresets::Azure());
            ImGuiUtils::AddPresetExecutor(app, ShdPalettePresets::Cinematic());
            ImGuiUtils::AddPresetExecutor(app, ShdPalettePresets::Desert());
            ImGuiUtils::AddPresetExecutor(app, ShdPalettePresets::Flame());
            ImGuiUtils::AddPresetExecutor(app, ShdPalettePresets::LongRandom64());
            ImGuiUtils::AddPresetExecutor(app, ShdPalettePresets::LongRainbow7());
            ImGuiUtils::AddPresetExecutor(app, ShdPalettePresets::Rainbow());
            ImGuiUtils::EndPresetExecutor();
        }
        if(ImGuiUtils::BeginPresetExecutor("Stripe")) {
            ImGuiUtils::AddPresetExecutor(app, ShdStripePresets::Disabled());
            ImGuiUtils::AddPresetExecutor(app, ShdStripePresets::SlowAnimated());
            ImGuiUtils::AddPresetExecutor(app, ShdStripePresets::FastAnimated());
            ImGuiUtils::AddPresetExecutor(app, ShdStripePresets::Smooth());
            ImGuiUtils::AddPresetExecutor(app, ShdStripePresets::SmoothTranslucent());
            ImGuiUtils::EndPresetExecutor();
        }
        if(ImGuiUtils::BeginPresetExecutor("Surface")) {
            ImGuiUtils::AddPresetExecutor(app, ShdSurfacePresets::Disabled());
            ImGuiUtils::AddPresetExecutor(app, ShdSurfacePresets::HighContrast());
            ImGuiUtils::AddPresetExecutor(app, ShdSurfacePresets::Reflective());
            ImGuiUtils::AddPresetExecutor(app, ShdSurfacePresets::Translucent());
            ImGuiUtils::AddPresetExecutor(app, ShdSurfacePresets::Reversed());
            ImGuiUtils::AddPresetExecutor(app, ShdSurfacePresets::Micro());
            ImGuiUtils::AddPresetExecutor(app, ShdSurfacePresets::Nano());
            ImGuiUtils::EndPresetExecutor();
        }
        if(ImGuiUtils::BeginPresetExecutor("Color")) {
            ImGuiUtils::AddPresetExecutor(app, ShdColorPresets::Disabled());
            ImGuiUtils::AddPresetExecutor(app, ShdColorPresets::WeakContrast());
            ImGuiUtils::AddPresetExecutor(app, ShdColorPresets::HighContrast());
            ImGuiUtils::AddPresetExecutor(app, ShdColorPresets::Dull());
            ImGuiUtils::AddPresetExecutor(app, ShdColorPresets::Vivid());
            ImGuiUtils::EndPresetExecutor();
        }
        if(ImGuiUtils::BeginPresetExecutor("Fog")) {
            ImGuiUtils::AddPresetExecutor(app, ShdFogPresets::Disabled());
            ImGuiUtils::AddPresetExecutor(app, ShdFogPresets::Low());
            ImGuiUtils::AddPresetExecutor(app, ShdFogPresets::Medium());
            ImGuiUtils::AddPresetExecutor(app, ShdFogPresets::High());
            ImGuiUtils::AddPresetExecutor(app, ShdFogPresets::Ultra());
            ImGuiUtils::EndPresetExecutor();
        }
        if(ImGuiUtils::BeginPresetExecutor("Bloom")) {
            ImGuiUtils::AddPresetExecutor(app, ShdBloomPresets::Disabled());
            ImGuiUtils::AddPresetExecutor(app, ShdBloomPresets::Highlighted());
            ImGuiUtils::AddPresetExecutor(app, ShdBloomPresets::HighlightedStrong());
            ImGuiUtils::AddPresetExecutor(app, ShdBloomPresets::Weak());
            ImGuiUtils::AddPresetExecutor(app, ShdBloomPresets::Normal());
            ImGuiUtils::AddPresetExecutor(app, ShdBloomPresets::Strong());
            ImGuiUtils::EndPresetExecutor();
        }
        ImGuiUtils::EndSettings();
    }
} // namespace merutilm::rff2
