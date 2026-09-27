//
// Created by Merutilm on 2025-06-23.
//

#include "RFFStaticMapBinary.hpp"

#include "../app/IOUtilities.h"
#include "../constants/FileConstants.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgcodecs.hpp"
#include "vulkan_helper/base/logger.hpp"

namespace merutilm::rff2 {


    const RFFStaticMapBinary RFFStaticMapBinary::DEFAULT = RFFStaticMapBinary(0, 0, 0);

    RFFStaticMapBinary::RFFStaticMapBinary(const double logZoom, const uint32_t width, const uint32_t height) :
        RFFMapBinary(logZoom), width(width), height(height) {
        static_assert(RFFBinaryRequirements<RFFStaticMapBinary>);
    }


    RFFStaticMapBinary RFFStaticMapBinary::read(std::ifstream &in) {
        float v;
        const uint32_t version = readVersion(in, reinterpret_cast<std::byte *>(&v));

        double lz;
        if (version == 0) {
            lz = v;
        } else {
            IOUtilities::readAndDecode(in, &lz);
        }

        uint32_t w;
        IOUtilities::readAndDecode(in, &w);
        uint32_t h;
        IOUtilities::readAndDecode(in, &h);
        return RFFStaticMapBinary(lz, w, h);
    }

    RFFStaticMapBinary RFFStaticMapBinary::readByID(const std::filesystem::path &dir, const uint32_t id) {
        return importFile<RFFStaticMapBinary>(dir / IOUtilities::fileNameFormat(id, Constants::File::EXT_STATIC_MAP));
    }
    cv::Mat RFFStaticMapBinary::loadImageByID(const std::filesystem::path &dir, const uint32_t id) {
        cv::Mat result = cv::imread((dir / IOUtilities::fileNameFormat(id, Constants::File::EXT_IMAGE)).string(),
                                    cv::IMREAD_UNCHANGED);
        return result;
    }


    void RFFStaticMapBinary::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportFile(*this, IOUtilities::generateFilename(dir, Constants::File::EXT_STATIC_MAP, nullptr));
    }

    void RFFStaticMapBinary::write(std::ofstream &out) const {
        IOUtilities::encodeAndWrite(out, logZoom);
        IOUtilities::encodeAndWrite(out, width);
        IOUtilities::encodeAndWrite(out, height);
    }


} // namespace merutilm::rff2
