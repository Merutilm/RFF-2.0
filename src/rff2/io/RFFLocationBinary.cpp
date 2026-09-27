//
// Created by Merutilm on 2025-06-25.
//

#include "RFFLocationBinary.hpp"

#include <utility>

#include "../app/IOUtilities.h"
#include "../constants/FileConstants.hpp"
#include "vulkan_helper/base/logger.hpp"

namespace merutilm::rff2 {
    inline const RFFLocationBinary RFFLocationBinary::DEFAULT = RFFLocationBinary(0, "", "", 0);

    RFFLocationBinary::RFFLocationBinary(const double logZoom, std::string real, std::string imag,
                                         const uint64_t maxIteration) :
        logZoom(logZoom), real(std::move(real)), imag(std::move(imag)), maxIteration(maxIteration) {
        static_assert(RFFBinaryRequirements<RFFLocationBinary>);
    }


    RFFLocationBinary RFFLocationBinary::read(std::ifstream &in) {
        float v;
        const uint32_t version = readVersion(in, reinterpret_cast<std::byte *>(&v));

        double lz;
        if (version == 0) {
            lz = v;
        } else {
            IOUtilities::readAndDecode(in, &lz);
        }

        uint64_t max;
        IOUtilities::readAndDecode(in, &max);
        uint64_t len;
        IOUtilities::readAndDecode(in, &len);
        std::vector<char> re(len + 1);
        IOUtilities::readAndDecode(in, len, re.data());
        IOUtilities::readAndDecode(in, &len);
        std::vector<char> im(len + 1);
        IOUtilities::readAndDecode(in, len, im.data());

        re.push_back('\0');
        im.push_back('\0');

        std::string r = re.data();
        std::string i = im.data();

        return RFFLocationBinary{lz, std::move(r), std::move(i), max};
    }


    void RFFLocationBinary::exportAsKeyframe(const std::filesystem::path &dir) const {
        exportFile(*this, IOUtilities::generateFilename(dir, Constants::File::EXT_LOCATION, nullptr));
    }


    void RFFLocationBinary::write(std::ofstream &out) const {
        uint64_t len = 0;
        IOUtilities::encodeAndWrite(out, logZoom);
        IOUtilities::encodeAndWrite(out, maxIteration);
        len = real.length();
        IOUtilities::encodeAndWrite(out, len);
        IOUtilities::encodeAndWrite(out, real.data(), real.length());
        len = imag.length();
        IOUtilities::encodeAndWrite(out, len);
        IOUtilities::encodeAndWrite(out, imag.data(), imag.length());
    }


} // namespace merutilm::rff2
