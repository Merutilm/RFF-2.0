//
// Created by Merutilm on 2025-06-25.
//

#pragma once

#include <string>

#include "RFFBinary.h"

namespace merutilm::rff2 {
    class RFFLocationBinary final : public RFFBinary{
        std::string real;
        std::string imag;
        uint64_t maxIteration;

        public:
        static const RFFLocationBinary DEFAULT;

        explicit RFFLocationBinary(float logZoom, std::string real, std::string imag, uint64_t maxIteration);

        [[nodiscard]] static RFFLocationBinary read(const std::filesystem::path &path);

        [[nodiscard]] bool hasData() const override;

        void exportAsKeyframe(const std::filesystem::path &dir) const override;

        void exportFile(const std::filesystem::path &path) const override;

        [[nodiscard]] const std::string &getReal() const;

        [[nodiscard]] const std::string &getImag() const;

        [[nodiscard]] uint64_t getMaxIteration() const;
    };
}
