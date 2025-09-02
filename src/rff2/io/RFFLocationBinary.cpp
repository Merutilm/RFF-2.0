//
// Created by Merutilm on 2025-06-25.
//

#include "RFFLocationBinary.h"

#include <utility>

#include "../../vulkan_helper/core/logger.hpp"
#include "../ui/IOUtilities.h"

namespace merutilm::rff2 {
    inline const RFFLocationBinary RFFLocationBinary::DEFAULT = RFFLocationBinary(0, "", "", 0);

    RFFLocationBinary::RFFLocationBinary(const float logZoom, std::string real, std::string imag,
                             const uint64_t maxIteration) : RFFBinary(logZoom), real(std::move(real)), imag(std::move(imag)),
                                                      maxIteration(maxIteration) {
    }

    RFFLocationBinary RFFLocationBinary::read(const std::filesystem::path &path) {
        if (!std::filesystem::exists(path)) {
            return DEFAULT;
        }
        std::ifstream in(path, std::ios::in | std::ios::binary);

        if (!in.is_open()) {
            return DEFAULT;
        }
        float logZoom;
        IOUtilities::readAndDecode(in, &logZoom);
        uint64_t maxIteration;
        IOUtilities::readAndDecode(in, &maxIteration);
        uint64_t len;
        IOUtilities::readAndDecode(in, &len);
        std::vector<char> re(len);
        IOUtilities::readAndDecode(in, len, re.data());
        IOUtilities::readAndDecode(in, &len);
        std::vector<char> im(len);
        IOUtilities::readAndDecode(in, len, im.data());

        re.push_back('\0');
        im.push_back('\0');

        std::string real = re.data();
        std::string imag = im.data();

        return RFFLocationBinary(logZoom, std::move(real), std::move(imag), maxIteration);
    }


    bool RFFLocationBinary::hasData() const {
        return !real.empty() && !imag.empty();
    }


    void RFFLocationBinary::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportFile(IOUtilities::generateFileName(dir, Constants::Extension::LOCATION));
    }


    void RFFLocationBinary::exportFile(const std::filesystem::path &path) const {
        if (std::ofstream out(path, std::ios::out | std::ios::binary | std::ios::trunc); out.is_open()) {
            uint64_t len = 0;
            IOUtilities::encodeAndWrite(out, getLogZoom());
            IOUtilities::encodeAndWrite(out, maxIteration);
            len = real.length();
            IOUtilities::encodeAndWrite(out, len);
            IOUtilities::encodeAndWrite(out, real.data(), real.length());
            len = imag.length();
            IOUtilities::encodeAndWrite(out, len);
            IOUtilities::encodeAndWrite(out, imag.data(), imag.length());
            out.close();
        } else {
            vkh::logger::w_log(L"ERROR : Cannot save file");
        }
    }


    const std::string &RFFLocationBinary::getReal() const {
        return real;
    }

    const std::string &RFFLocationBinary::getImag() const {
        return imag;
    }

    uint64_t RFFLocationBinary::getMaxIteration() const {
        return maxIteration;
    }
}
