//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../Presets.h"

namespace merutilm::rff2::CalculationPresets {
    struct UltraFast final : public Presets::CalculationPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceSyncSettings genRefSync() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct Fast final : public Presets::CalculationPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceSyncSettings genRefSync() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct Normal final : public Presets::CalculationPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceSyncSettings genRefSync() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct Best final : public Presets::CalculationPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceSyncSettings genRefSync() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct UltraBest final : public Presets::CalculationPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceSyncSettings genRefSync() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct Stable final : public Presets::CalculationPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceSyncSettings genRefSync() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct MoreStable final : public Presets::CalculationPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceSyncSettings genRefSync() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct UltraStable final : public Presets::CalculationPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceSyncSettings genRefSync() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
}
