//
// Created by Merutilm on 2025-06-23.
//

#pragma once
#include <filesystem>

#include "../app/IOUtilities.h"

namespace merutilm::rff2 {
    struct RFFBinary {

        double logZoom;
        static constexpr uint32_t VERSION = 1;

        explicit RFFBinary(const double logZoom) : logZoom(logZoom) {
        }

        virtual ~RFFBinary() = default;

        [[nodiscard]] virtual bool hasData() const = 0;


        static uint32_t readVersion(std::ifstream &in, std::byte *raw) {

            uint32_t version;
            IOUtilities::readAndDecode(in, &version);

            if (version > 1u << 23u) {
                //normalized minimum float bits (approximately 1e-308), version 0.
                memcpy(raw, &version, sizeof(uint32_t));
                version = 0;
            }
            return version;
        }

        virtual void exportAsKeyframe(const std::filesystem::path &dir) const = 0;

        virtual void exportFile(const std::filesystem::path &path) const = 0;
    };
}
