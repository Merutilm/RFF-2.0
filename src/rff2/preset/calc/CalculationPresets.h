//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../Presets.h"
#include "../../attr/CalMPAAttribute.h"
#include "../../attr/CalReferenceCompAttribute.h"

namespace merutilm::rff2::CalculationPresets {
    struct UltraFast final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] CalMPAAttribute genMPA() const override;
        [[nodiscard]] CalReferenceCompAttribute genReferenceCompression() const override;
    };
    struct Fast final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] CalMPAAttribute genMPA() const override;
        [[nodiscard]] CalReferenceCompAttribute genReferenceCompression() const override;
    };
    struct Normal final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] CalMPAAttribute genMPA() const override;
        [[nodiscard]] CalReferenceCompAttribute genReferenceCompression() const override;
    };
    struct Best final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] CalMPAAttribute genMPA() const override;
        [[nodiscard]] CalReferenceCompAttribute genReferenceCompression() const override;
    };
    struct UltraBest final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] CalMPAAttribute genMPA() const override;
        [[nodiscard]] CalReferenceCompAttribute genReferenceCompression() const override;
    };
    struct Stable final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] CalMPAAttribute genMPA() const override;
        [[nodiscard]] CalReferenceCompAttribute genReferenceCompression() const override;
    };
    struct MoreStable final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] CalMPAAttribute genMPA() const override;
        [[nodiscard]] CalReferenceCompAttribute genReferenceCompression() const override;
    };
    struct UltraStable final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] CalMPAAttribute genMPA() const override;
        [[nodiscard]] CalReferenceCompAttribute genReferenceCompression() const override;
    };
}
