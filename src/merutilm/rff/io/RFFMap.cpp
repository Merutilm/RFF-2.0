//
// Created by Merutilm on 2025-05-08.
//

#include "RFFMap.h"

#include <filesystem>
#include <fstream>

#include "../ui/IOUtilities.h"
#include "../ui/RFF.h"
#include "../ui/RFFUtilities.h"


RFFMap::RFFMap(const float logZoom, const uint64_t period, const uint64_t maxIteration,
               Matrix<double> iterations) : logZoom(logZoom), period(period), maxIteration(maxIteration),
                                            iterations(std::move(iterations)) {
}

RFFMap RFFMap::defaultRFFMap() {
    return RFFMap(0, 0, 0, Matrix<double>(0, 0));
};


bool RFFMap::hasData() const {
    return iterations.width > 0;
}

RFFMap RFFMap::read(const std::filesystem::path &path) {
    if (!std::filesystem::exists(path)) {
        return defaultRFFMap();
    }
    std::ifstream in(path, std::ios::in | std::ios::binary);

    if (!in.is_open()) {
        return defaultRFFMap();
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
    exportRFM(IOUtilities::generateFileName(dir, RFF::Extension::MAP));
}

void RFFMap::exportRFM(const std::filesystem::path &path) const {
    if (std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc); out.is_open()) {
        IOUtilities::encodeAndWrite(out, iterations.getWidth());
        IOUtilities::encodeAndWrite(out, iterations.getHeight());
        IOUtilities::encodeAndWrite(out, logZoom);
        IOUtilities::encodeAndWrite(out, period);
        IOUtilities::encodeAndWrite(out, maxIteration);
        IOUtilities::encodeAndWrite(out, iterations.canvas);
        out.close();
    } else {
        RFFUtilities::log("ERROR : Cannot save file");
    }
}
