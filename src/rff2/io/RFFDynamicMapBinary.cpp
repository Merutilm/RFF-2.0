//
// Created by Merutilm on 2025-05-08.
//

#include "RFFDynamicMapBinary.hpp"

#include <filesystem>
#include <fstream>

#include "../app/IOUtilities.h"
#include "../constants/Constants.hpp"

namespace merutilm::rff2 {

    inline const RFFDynamicMapBinary RFFDynamicMapBinary::DEFAULT =
            RFFDynamicMapBinary(0, 0, 0, std::vector<double>(), 0, 0);

    RFFDynamicMapBinary::RFFDynamicMapBinary(const double logZoom, const itt::ptb period, const itt::ptb maxIteration,
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

        itt::ptb p = 0;
        itt::ptb m = 0;

        if (version <= 4) {
            uint64_t p64;
            uint64_t m64;
            IOUtilities::readAndDecode(in, &p64);
            IOUtilities::readAndDecode(in, &m64);
            p = static_cast<itt::ptb>(p64);
            m = static_cast<itt::ptb>(m64);
        }else {
            uint64_t pSize = 0;
            uint64_t mSize = 0;
            IOUtilities::readAndDecode(in, &pSize);
            std::vector<std::byte> pRaw(pSize);
            IOUtilities::readAndDecode(in, &pRaw);
            IOUtilities::readAndDecode(in, &mSize);
            std::vector<std::byte> mRaw(mSize);
            IOUtilities::readAndDecode(in, &mRaw);
            memcpy(&p, pRaw.data(), std::min(sizeof(itt::ptb), pSize));
            memcpy(&m, mRaw.data(), std::min(sizeof(itt::ptb), mSize));
        }
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
        IOUtilities::encodeAndWrite(out, sizeof(period));
        IOUtilities::encodeAndWrite(out, period);
        IOUtilities::encodeAndWrite(out, sizeof(maxIteration));
        IOUtilities::encodeAndWrite(out, maxIteration);
        IOUtilities::encodeAndWrite(out, iterations);
    }

} // namespace merutilm::rff2
