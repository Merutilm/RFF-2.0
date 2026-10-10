//
// Created by Merutilm on 2025-06-23.
//

#pragma once

#include "../app/IOUtilities.h"
#include <fstream>
#include <concepts>

#include "vulkan_helper/base/vkh.hpp"

namespace merutilm::rff2 {

    template <typename T> concept
    RFFBinaryRequirements = requires (const T &a, std::ofstream &out, std::ifstream &in)
    {
        {T::DEFAULT} -> std::same_as<const T&>;
        {a.write(out)} -> std::same_as<void>;
        {T::read(in)} -> std::same_as<T>;

    };

    struct RFFBinary {

        static constexpr uint32_t VERSION = 4;

        static uint32_t readVersion(std::ifstream &in, std::byte *raw = nullptr) {

            uint32_t version;
            IOUtilities::readAndDecode(in, &version);

            if (version > 1u << 23u) {
                //normalized minimum float bits (approximately 1e-308), version 0.
                if (raw) memcpy(raw, &version, sizeof(uint32_t));
                version = 0;
            }
            return version;
        }

        template<RFFBinaryRequirements B>
        static B importFile(const std::filesystem::path &path) {
            if (!std::filesystem::exists(path)) {
                return B::DEFAULT;
            }
            std::ifstream in(path, std::ios::in | std::ios::binary);

            if (!in.is_open()) {
                return B::DEFAULT;
            }
            B result = B::read(in);
            in.close();
            return result;
        }


        template<RFFBinaryRequirements B>
        static void exportFile(const B &b, const std::filesystem::path &path) {
            if (std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc); out.is_open()) {
                IOUtilities::encodeAndWrite(out, VERSION);
                b.write(out);
                out.close();
            } else {
                vkh::logger::log_err("ERROR : Cannot save file");
            }
        }

    };
}
