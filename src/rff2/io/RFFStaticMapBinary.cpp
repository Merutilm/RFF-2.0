//
// Created by Merutilm on 2025-06-23.
//

#include "RFFStaticMapBinary.h"

#include "../../vulkan_helper/core/logger.hpp"
#include "../ui/IOUtilities.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/mat.hpp"

namespace merutilm::rff2 {


    const RFFStaticMapBinary RFFStaticMapBinary::DEFAULT = RFFStaticMapBinary(0, 0, 0);

    RFFStaticMapBinary::RFFStaticMapBinary(const float logZoom, const uint32_t width, const uint32_t height) : RFFBinary(logZoom), width(width), height(height) {

    }

    bool RFFStaticMapBinary::hasData() const {
        return width > 0 && height > 0;
    }


    RFFStaticMapBinary RFFStaticMapBinary::read(const std::filesystem::path &path) {
        if (!std::filesystem::exists(path)) {
            return DEFAULT;
        }
        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in.is_open()) {
            return DEFAULT;
        }

        float lz;
        IOUtilities::readAndDecode(in, &lz);
        uint32_t w;
        IOUtilities::readAndDecode(in, &w);
        uint32_t h;
        IOUtilities::readAndDecode(in, &h);
        return RFFStaticMapBinary(lz, w, h);
    }

    RFFStaticMapBinary RFFStaticMapBinary::readByID(const std::filesystem::path& dir, const uint32_t id) {
        return read(dir / IOUtilities::fileNameFormat(id, Constants::Extension::STATIC_MAP));
    }
    cv::Mat RFFStaticMapBinary::loadImageByID(const std::filesystem::path &dir, const uint32_t id) {
        cv::Mat result = cv::imread((dir / IOUtilities::fileNameFormat(id, Constants::Extension::IMAGE)).string(), cv::IMREAD_UNCHANGED);
        return result;
    }


    void RFFStaticMapBinary::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportFile(IOUtilities::generateFileName(dir, Constants::Extension::STATIC_MAP));
    }

    void RFFStaticMapBinary::exportFile(const std::filesystem::path &path) const {
        if (std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc); out.is_open()) {
            IOUtilities::encodeAndWrite(out, getLogZoom());
            IOUtilities::encodeAndWrite(out, getWidth());
            IOUtilities::encodeAndWrite(out, getHeight());
            out.close();
        } else {
            vkh::logger::log("ERROR : Cannot save file");
        }
    }

    uint32_t RFFStaticMapBinary::getWidth() const {
        return width;
    }
    uint32_t RFFStaticMapBinary::getHeight() const {
        return height;
    }


}
