//
// Created by Merutilm on 9/4/26.
//

#pragma once
#include "../../Presets.hpp"
namespace merutilm::rff2::ClcSyncPresets {
    struct Fast final : public Presets::CalculationPresets::ReferenceSyncPreset {
        [[nodiscard]] std::string getName() const override;
        FrtReferenceSyncSettings genRefSync() const override;
    };
    struct Normal final : public Presets::CalculationPresets::ReferenceSyncPreset {
        [[nodiscard]] std::string getName() const override;
        FrtReferenceSyncSettings genRefSync() const override;
    };
    struct Best final : public Presets::CalculationPresets::ReferenceSyncPreset {
        [[nodiscard]] std::string getName() const override;
        FrtReferenceSyncSettings genRefSync() const override;
    };
}
