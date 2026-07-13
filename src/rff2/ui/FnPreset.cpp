//
// Created by Merutilm on 7/14/26.
//

#include "FnPreset.hpp"

#include "../preset/calc/CalculationPresets.h"
#include "../preset/render/RenderPresets.h"
#include "../preset/resolution/ResolutionPresets.h"
#include "../preset/shader/bloom/ShdBloomPresets.h"
#include "../preset/shader/color/ShdColorPresets.h"
#include "../preset/shader/fog/ShdFogPresets.h"
#include "../preset/shader/palette/ShdPalettePresets.h"
#include "../preset/shader/slope/ShdSlopePresets.h"
#include "../preset/shader/stripe/ShdStripePresets.h"

namespace merutilm::rff2 {


    void FnPreset::calculation(RFFApplication &app) {
        if (ImGui::TreeNode("Calculation")) {
            addPresetExecutor(app, CalculationPresets::UltraFast());
            addPresetExecutor(app, CalculationPresets::Fast());
            addPresetExecutor(app, CalculationPresets::Normal());
            addPresetExecutor(app, CalculationPresets::Best());
            addPresetExecutor(app, CalculationPresets::UltraBest());
            addPresetExecutor(app, CalculationPresets::Stable());
            addPresetExecutor(app, CalculationPresets::MoreStable());
            addPresetExecutor(app, CalculationPresets::UltraStable());
            ImGui::TreePop();
        }
    }
    void FnPreset::render(RFFApplication &app) {
        if (ImGui::TreeNode("Render")) {
            addPresetExecutor(app, RenderPresets::Potato());
            addPresetExecutor(app, RenderPresets::Low());
            addPresetExecutor(app, RenderPresets::Medium());
            addPresetExecutor(app, RenderPresets::High());
            addPresetExecutor(app, RenderPresets::Ultra());
            addPresetExecutor(app, RenderPresets::Extreme());
            ImGui::TreePop();
        }
    }
    void FnPreset::resolution(RFFApplication &app) {
        if (ImGui::TreeNode("Resolution")) {

            addPresetExecutor(app, ResolutionPresets::L1());
            addPresetExecutor(app, ResolutionPresets::L2());
            addPresetExecutor(app, ResolutionPresets::L3());
            addPresetExecutor(app, ResolutionPresets::L4());
            addPresetExecutor(app, ResolutionPresets::L5());
            ImGui::TreePop();
        }
    }
    void FnPreset::shader(RFFApplication &app) {
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
                addPresetExecutor(app, BloomPresets::Disabled());
                addPresetExecutor(app, BloomPresets::Highlighted());
                addPresetExecutor(app, BloomPresets::HighlightedStrong());
                addPresetExecutor(app, BloomPresets::Weak());
                addPresetExecutor(app, BloomPresets::Normal());
                addPresetExecutor(app, BloomPresets::Strong());
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }
} // namespace merutilm::rff2
