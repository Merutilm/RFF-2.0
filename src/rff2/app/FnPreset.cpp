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
#include "../preset/shader/slope/ShdSlopePresets.hpp"
#include "../preset/shader/stripe/ShdStripePresets.hpp"

namespace merutilm::rff2 {


    void FnPreset::calculation(RFF2 &app) {
        if (ImGui::TreeNode("Calculation")) {
            if (ImGui::TreeNode("Approximation")) {


                addPresetExecutor(app, ClcApproxPresets::UltraFast());
                addPresetExecutor(app, ClcApproxPresets::Fast());
                addPresetExecutor(app, ClcApproxPresets::Normal());
                addPresetExecutor(app, ClcApproxPresets::Best());
                addPresetExecutor(app, ClcApproxPresets::UltraBest());
                addPresetExecutor(app, ClcApproxPresets::LightSpirals());
                addPresetExecutor(app, ClcApproxPresets::DenseSpirals());
                addPresetExecutor(app, ClcApproxPresets::ExtremelyDenseSpirals());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Compression")) {
                addPresetExecutor(app, ClcCompressPresets::None());
                addPresetExecutor(app, ClcCompressPresets::Stable());
                addPresetExecutor(app, ClcCompressPresets::MoreStable());
                addPresetExecutor(app, ClcCompressPresets::UltraStable());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Synchronization")) {
                addPresetExecutor(app, ClcSyncPresets::Fast());
                addPresetExecutor(app, ClcSyncPresets::Normal());
                addPresetExecutor(app, ClcSyncPresets::Best());

                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
    void FnPreset::render(RFF2 &app) {
        if (ImGui::TreeNode("Render")) {
            if (ImGui::TreeNode("Display")) {
                addPresetExecutor(app, RndDisplayPresets::Potato());
                addPresetExecutor(app, RndDisplayPresets::Low());
                addPresetExecutor(app, RndDisplayPresets::Medium());
                addPresetExecutor(app, RndDisplayPresets::High());
                addPresetExecutor(app, RndDisplayPresets::Ultra());
                addPresetExecutor(app, RndDisplayPresets::Extreme());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Compute Shader")) {

                addPresetExecutor(app, RndComputePresets::None());
                addPresetExecutor(app, RndComputePresets::General());
                addPresetExecutor(app, RndComputePresets::LightZoomSpirals());
                addPresetExecutor(app, RndComputePresets::DeepZoomSpirals());
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
    void FnPreset::resolution(RFF2 &app) {
        if (ImGui::TreeNode("Resolution")) {

            addPresetExecutor(app, ResolutionPresets::L1());
            addPresetExecutor(app, ResolutionPresets::L2());
            addPresetExecutor(app, ResolutionPresets::L3());
            addPresetExecutor(app, ResolutionPresets::L4());
            addPresetExecutor(app, ResolutionPresets::L5());
            ImGui::TreePop();
        }
    }
    void FnPreset::shader(RFF2 &app) {
        if (ImGui::TreeNode("Shader")) {
            if (ImGui::TreeNode("Palette")) {
                addPresetExecutor(app, ShdPalettePresets::Classic1());
                addPresetExecutor(app, ShdPalettePresets::Classic2());
                addPresetExecutor(app, ShdPalettePresets::Azure());
                addPresetExecutor(app, ShdPalettePresets::Cinematic());
                addPresetExecutor(app, ShdPalettePresets::Desert());
                addPresetExecutor(app, ShdPalettePresets::Flame());
                addPresetExecutor(app, ShdPalettePresets::LongRandom64());
                addPresetExecutor(app, ShdPalettePresets::LongRainbow7());
                addPresetExecutor(app, ShdPalettePresets::Rainbow());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Stripe")) {
                addPresetExecutor(app, ShdStripePresets::Disabled());
                addPresetExecutor(app, ShdStripePresets::SlowAnimated());
                addPresetExecutor(app, ShdStripePresets::FastAnimated());
                addPresetExecutor(app, ShdStripePresets::Smooth());
                addPresetExecutor(app, ShdStripePresets::SmoothTranslucent());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Slope")) {
                addPresetExecutor(app, ShdSlopePresets::Disabled());
                addPresetExecutor(app, ShdSlopePresets::NoReflection());
                addPresetExecutor(app, ShdSlopePresets::Reflective());
                addPresetExecutor(app, ShdSlopePresets::Translucent());
                addPresetExecutor(app, ShdSlopePresets::Reversed());
                addPresetExecutor(app, ShdSlopePresets::Micro());
                addPresetExecutor(app, ShdSlopePresets::Nano());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Color")) {
                addPresetExecutor(app, ShdColorPresets::Disabled());
                addPresetExecutor(app, ShdColorPresets::WeakContrast());
                addPresetExecutor(app, ShdColorPresets::HighContrast());
                addPresetExecutor(app, ShdColorPresets::Dull());
                addPresetExecutor(app, ShdColorPresets::Vivid());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Fog")) {
                addPresetExecutor(app, ShdFogPresets::Disabled());
                addPresetExecutor(app, ShdFogPresets::Low());
                addPresetExecutor(app, ShdFogPresets::Medium());
                addPresetExecutor(app, ShdFogPresets::High());
                addPresetExecutor(app, ShdFogPresets::Ultra());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Bloom")) {
                addPresetExecutor(app, ShdBloomPresets::Disabled());
                addPresetExecutor(app, ShdBloomPresets::Highlighted());
                addPresetExecutor(app, ShdBloomPresets::HighlightedStrong());
                addPresetExecutor(app, ShdBloomPresets::Weak());
                addPresetExecutor(app, ShdBloomPresets::Normal());
                addPresetExecutor(app, ShdBloomPresets::Strong());
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
} // namespace merutilm::rff2
