//
// Created by Merutilm on 2025-05-08.
//

#include "RFFDynamicMapBinary.hpp"

#include <filesystem>
#include <fstream>

#include "../app/IOUtilities.h"
#include "../constants/Constants.hpp"
#include "vulkan_helper/base/logger.hpp"
#include "vulkan_helper/util/BufferImageUtils.hpp"

namespace merutilm::rff2 {

    inline const RFFDynamicMapBinary RFFDynamicMapBinary::DEFAULT =
            RFFDynamicMapBinary(0, 0, 0, std::vector<double>(), 0, 0);

    RFFDynamicMapBinary::RFFDynamicMapBinary(const double logZoom, const uint64_t period, const uint64_t maxIteration,
                                             std::vector<double> iterations, const uint16_t width,
                                             const uint16_t height) :
        RFFMapBinary(logZoom), period(period), maxIteration(maxIteration), iterations(std::move(iterations)),
        width(width), height(height) {
        static_assert(RFFBinaryRequirements<RFFDynamicMapBinary>);
    }


    RFFDynamicMapBinary RFFDynamicMapBinary::read(std::ifstream &in) {

        uint32_t wh = 0;
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
        } else {
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

    RFFDynamicMapBinary RFFDynamicMapBinary::readByID(const std::filesystem::path &dir, const uint32_t id) {
        return importFile<RFFDynamicMapBinary>(dir / IOUtilities::fileNameFormat(id, Constants::File::EXT_DYNAMIC_MAP));
    }


    void RFFDynamicMapBinary::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportFile(*this, IOUtilities::generateFilename(dir, Constants::File::EXT_DYNAMIC_MAP, nullptr));
    }

    void RFFDynamicMapBinary::write(std::ofstream &out) const {
        IOUtilities::encodeAndWrite(out, width);
        IOUtilities::encodeAndWrite(out, height);
        IOUtilities::encodeAndWrite(out, logZoom);
        IOUtilities::encodeAndWrite(out, period);
        IOUtilities::encodeAndWrite(out, maxIteration);
        IOUtilities::encodeAndWrite(out, iterations);
    }

} // namespace merutilm::rff2
