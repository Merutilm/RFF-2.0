//
// Created by Merutilm on 2025-05-08.
//

#include "RFFMap.h"

#include <filesystem>
#include <fstream>

#include "../ui/IOUtilities.h"
#include "../ui/Constants.h"
#include "../ui/Utilities.h"

namespace merutilm::rff {
    RFFMap::RFFMap(const float logZoom, const uint64_t period, const uint64_t maxIteration,
                                  Matrix<double> iterations) : logZoom(logZoom), period(period), maxIteration(maxIteration),
                                                               iterations(std::move(iterations)) {
    }


    bool RFFMap::hasData() const {
        return iterations.getWidth() > 0;
    }

    RFFMap RFFMap::readByID(const std::filesystem::path& open, const uint32_t id) {
        return read(open / IOUtilities::fileNameFormat(id, Constants::Extension::MAP));
    }

    RFFMap RFFMap::read(const std::filesystem::path &path) {
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
        return RFFMap(z, p, m, Matrix(w, h, i));
    }

    void RFFMap::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportRFM(IOUtilities::generateFileName(dir, Constants::Extension::MAP));
    }

    void RFFMap::exportRFM(const std::filesystem::path &path) const {
        if (std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc); out.is_open()) {
            IOUtilities::encodeAndWrite(out, iterations.getWidth());
            IOUtilities::encodeAndWrite(out, iterations.getHeight());
            IOUtilities::encodeAndWrite(out, logZoom);
            IOUtilities::encodeAndWrite(out, period);
            IOUtilities::encodeAndWrite(out, maxIteration);
            IOUtilities::encodeAndWrite(out, iterations.getCanvas());
            out.close();
        } else {
            Utilities::log("ERROR : Cannot save file");
        }
    }


    float RFFMap::getLogZoom() const {
        return logZoom;
    }

    uint64_t RFFMap::getPeriod() const {
        return period;
    }

    uint64_t RFFMap::getMaxIteration() const {
        return maxIteration;
    }

    const Matrix<double> &RFFMap::getMatrix() const {
        return iterations;
    }
}