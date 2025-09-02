//
// Created by Merutilm on 2025-05-08.
//

#include "RFFDynamicMapBinary.h"

#include <filesystem>
#include <fstream>

#include "../../vulkan_helper/util/BufferImageUtils.hpp"
#include "../../vulkan_helper/core/logger.hpp"
#include "../ui/IOUtilities.h"
#include "../constants/Constants.hpp"

namespace merutilm::rff2 {

    inline const RFFDynamicMapBinary RFFDynamicMapBinary::DEFAULT = RFFDynamicMapBinary(0, 0, 0, Matrix<double>(0, 0));

    RFFDynamicMapBinary::RFFDynamicMapBinary(const float logZoom, const uint64_t period, const uint64_t maxIteration,
                                  Matrix<double> iterations) : RFFBinary(logZoom), period(period), maxIteration(maxIteration),
                                                               iterations(std::move(iterations)) {
    }


    bool RFFDynamicMapBinary::hasData() const {
        return iterations.getWidth() > 0;
    }


    RFFDynamicMapBinary RFFDynamicMapBinary::read(const std::filesystem::path &path) {
        if (!std::filesystem::exists(path)) {
            return DEFAULT;
        }
        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in.is_open()) {
            return DEFAULT;
        }

        uint16_t w;
        IOUtilities::readAndDecode(in, &w);
        uint16_t h;
        IOUtilities::readAndDecode(in, &h);
        float z;
        IOUtilities::readAndDecode(in, &z);
        uint64_t p;
        IOUtilities::readAndDecode(in, &p);
        uint64_t m;
        IOUtilities::readAndDecode(in, &m);
        auto i = std::vector<double>(w * h);
        IOUtilities::readAndDecode(in, &i);
        return RFFDynamicMapBinary(z, p, m, Matrix(w, h, i));
    }

    RFFDynamicMapBinary RFFDynamicMapBinary::readByID(const std::filesystem::path& dir, const uint32_t id) {
        return read(dir / IOUtilities::fileNameFormat(id, Constants::Extension::DYNAMIC_MAP));
    }


    void RFFDynamicMapBinary::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportFile(IOUtilities::generateFileName(dir, Constants::Extension::DYNAMIC_MAP));
    }

    void RFFDynamicMapBinary::exportFile(const std::filesystem::path &path) const {
        if (std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc); out.is_open()) {
            IOUtilities::encodeAndWrite(out, iterations.getWidth());
            IOUtilities::encodeAndWrite(out, iterations.getHeight());
            IOUtilities::encodeAndWrite(out, getLogZoom());
            IOUtilities::encodeAndWrite(out, period);
            IOUtilities::encodeAndWrite(out, maxIteration);
            IOUtilities::encodeAndWrite(out, iterations.getCanvas());
            out.close();
        } else {
            vkh::logger::w_log(L"ERROR : Cannot save file");
        }
    }


    uint64_t RFFDynamicMapBinary::getPeriod() const {
        return period;
    }

    uint64_t RFFDynamicMapBinary::getMaxIteration() const {
        return maxIteration;
    }

    const Matrix<double> &RFFDynamicMapBinary::getMatrix() const {
        return iterations;
    }
}
