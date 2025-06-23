//
// Created by Merutilm on 2025-05-08.
//

#include "RFFDynamicMap.h"

#include <filesystem>
#include <fstream>

#include "../ui/IOUtilities.h"
#include "../ui/Constants.h"
#include "../ui/Utilities.h"
#include "opencv2/imgcodecs.hpp"

namespace merutilm::rff {

    inline const RFFDynamicMap RFFDynamicMap::DEFAULT_MAP = RFFDynamicMap(0, 0, 0, Matrix<double>(0, 0));

    RFFDynamicMap::RFFDynamicMap(const float logZoom, const uint64_t period, const uint64_t maxIteration,
                                  Matrix<double> iterations) : RFFMap(logZoom), period(period), maxIteration(maxIteration),
                                                               iterations(std::move(iterations)) {
    }


    bool RFFDynamicMap::hasData() const {
        return iterations.getWidth() > 0;
    }


    RFFDynamicMap RFFDynamicMap::read(const std::filesystem::path &path) {
        if (!std::filesystem::exists(path)) {
            return DEFAULT_MAP;
        }
        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in.is_open()) {
            return DEFAULT_MAP;
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
        return RFFDynamicMap(z, p, m, Matrix(w, h, i));
    }

    RFFDynamicMap RFFDynamicMap::readByID(const std::filesystem::path& dir, const uint32_t id) {
        return read(dir / IOUtilities::fileNameFormat(id, Constants::Extension::DYNAMIC_MAP));
    }


    void RFFDynamicMap::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportFile(IOUtilities::generateFileName(dir, Constants::Extension::DYNAMIC_MAP));
    }

    void RFFDynamicMap::exportFile(const std::filesystem::path &path) const {
        if (std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc); out.is_open()) {
            IOUtilities::encodeAndWrite(out, iterations.getWidth());
            IOUtilities::encodeAndWrite(out, iterations.getHeight());
            IOUtilities::encodeAndWrite(out, getLogZoom());
            IOUtilities::encodeAndWrite(out, period);
            IOUtilities::encodeAndWrite(out, maxIteration);
            IOUtilities::encodeAndWrite(out, iterations.getCanvas());
            out.close();
        } else {
            Utilities::log("ERROR : Cannot save file");
        }
    }


    uint64_t RFFDynamicMap::getPeriod() const {
        return period;
    }

    uint64_t RFFDynamicMap::getMaxIteration() const {
        return maxIteration;
    }

    const Matrix<double> &RFFDynamicMap::getMatrix() const {
        return iterations;
    }
}
