//
// Created by Merutilm on 2025-06-23.
//

#include "RFFStaticMap.h"

#include "../ui/IOUtilities.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core/mat.hpp"

namespace merutilm::rff {


    const RFFStaticMap RFFStaticMap::DEFAULT_MAP = RFFStaticMap(0, 0, 0);

    RFFStaticMap::RFFStaticMap(const float logZoom, const uint32_t width, const uint32_t height) : RFFMap(logZoom), width(width), height(height) {

    }

    bool RFFStaticMap::hasData() const {
        return width > 0 && height > 0;
    }


    RFFStaticMap RFFStaticMap::read(const std::filesystem::path &path) {
        if (!std::filesystem::exists(path)) {
            return DEFAULT_MAP;
        }
        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in.is_open()) {
            return DEFAULT_MAP;
        }

        float lz;
        IOUtilities::readAndDecode(in, &lz);
        uint32_t w;
        IOUtilities::readAndDecode(in, &w);
        uint32_t h;
        IOUtilities::readAndDecode(in, &h);
        return RFFStaticMap(lz, w, h);
    }

    RFFStaticMap RFFStaticMap::readByID(const std::filesystem::path& dir, const uint32_t id) {
        return read(dir / IOUtilities::fileNameFormat(id, Constants::Extension::STATIC_MAP));
    }
    cv::Mat RFFStaticMap::loadImageByID(const std::filesystem::path &dir, const uint32_t id) {
        return cv::imread((dir / IOUtilities::fileNameFormat(id, Constants::Extension::IMAGE)).string(), cv::IMREAD_COLOR_RGB);
    }


    void RFFStaticMap::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportFile(IOUtilities::generateFileName(dir, Constants::Extension::STATIC_MAP));
    }

    void RFFStaticMap::exportFile(const std::filesystem::path &path) const {
        if (std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc); out.is_open()) {
            IOUtilities::encodeAndWrite(out, getLogZoom());
            IOUtilities::encodeAndWrite(out, getWidth());
            IOUtilities::encodeAndWrite(out, getHeight());
            out.close();
        } else {
            Utilities::log("ERROR : Cannot save file");
        }
    }

    uint32_t RFFStaticMap::getWidth() const {
        return width;
    }
    uint32_t RFFStaticMap::getHeight() const {
        return height;
    }


}
