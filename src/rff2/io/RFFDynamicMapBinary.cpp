//
// Created by Merutilm on 2025-05-08.
//

#include "RFFDynamicMapBinary.h"

#include <filesystem>
#include <fstream>

#include "../app/IOUtilities.h"
#include "../constants/Constants.hpp"
#include "vulkan_helper/base/logger.hpp"
#include "vulkan_helper/util/BufferImageUtils.hpp"

namespace merutilm::rff2 {

    inline const RFFDynamicMapBinary RFFDynamicMapBinary::DEFAULT = RFFDynamicMapBinary(0, 0, 0, std::vector<double>(), 0, 0);

    RFFDynamicMapBinary::RFFDynamicMapBinary(const double logZoom, const uint64_t period, const uint64_t maxIteration,
                                  std::vector<double> iterations, const uint16_t width, const uint16_t height) : RFFBinary(logZoom), period(period), maxIteration(maxIteration),
                                                               iterations(std::move(iterations)), width(width), height(height) {
    }


    bool RFFDynamicMapBinary::hasData() const {
        return width > 0;
    }


    RFFDynamicMapBinary RFFDynamicMapBinary::read(const std::filesystem::path &path) {
        if (!std::filesystem::exists(path)) {
            return DEFAULT;
        }
        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in.is_open()) {
            return DEFAULT;
        }

        uint32_t wh;
        const uint32_t version = readVersion(in, reinterpret_cast<std::byte *>(&wh));
        uint16_t w;
        uint16_t h;
        double lz;
        if (version == 0) {
            w = static_cast<uint16_t>(wh & 0xffffU);
            h = static_cast<uint16_t>((wh >> 16u) & 0xffffU);
            float z;
            IOUtilities::readAndDecode(in, &z);
            lz = z;
        }else {
            IOUtilities::readAndDecode(in, &w);
            IOUtilities::readAndDecode(in, &h);
            IOUtilities::readAndDecode(in, &lz);
        }

        uint64_t p;
        IOUtilities::readAndDecode(in, &p);
        uint64_t m;
        IOUtilities::readAndDecode(in, &m);
        auto i = std::vector<double>(w * h);
        IOUtilities::readAndDecode(in, &i);
        return RFFDynamicMapBinary{lz, p, m, i, w, h};
    }

    RFFDynamicMapBinary RFFDynamicMapBinary::readByID(const std::filesystem::path& dir, const uint32_t id) {
        return read(dir / IOUtilities::fileNameFormat(id, Constants::File::EXT_DYNAMIC_MAP));
    }


    void RFFDynamicMapBinary::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportFile(IOUtilities::generateFilename(dir, Constants::File::EXT_DYNAMIC_MAP, nullptr));
    }

    void RFFDynamicMapBinary::exportFile(const std::filesystem::path &path) const {
        if (std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc); out.is_open()) {
            IOUtilities::encodeAndWrite(out, VERSION);
            IOUtilities::encodeAndWrite(out, width);
            IOUtilities::encodeAndWrite(out, height);
            IOUtilities::encodeAndWrite(out, logZoom);
            IOUtilities::encodeAndWrite(out, period);
            IOUtilities::encodeAndWrite(out, maxIteration);
            IOUtilities::encodeAndWrite(out, iterations);
            out.close();
        } else {
            vkh::logger::log("ERROR : Cannot save file");
        }
    }

}
