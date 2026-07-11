//
// Created by Merutilm on 2025-06-08.
//

#include "IOUtilities.h"

#include <filesystem>
#include <nfd.hpp>

#include "Utilities.h"


namespace merutilm::rff2 {
    std::unique_ptr<std::filesystem::path> IOUtilities::ioFileDialog(const std::string_view desc, const char type,
                                                                     const std::string_view extension) {

        NFD::UniquePath outPath;

        const nfdfilteritem_t filterItem[] = {
            { desc.data(), extension.data() }
        };

        nfdresult_t result;

        switch (type)
        {
            case OPEN_FILE:
                result = NFD::OpenDialog(
                    outPath,
                    filterItem,
                    1,
                    nullptr);
                break;

            case SAVE_FILE:
                result = NFD::SaveDialog(
                    outPath,
                    filterItem,
                    1,
                    nullptr,
                    nullptr);
                break;

            default:
                return nullptr;
        }

        if (result != NFD_OKAY)
            return nullptr;

        std::filesystem::path path(outPath.get());

        if (!path.has_extension())
            path.replace_extension(extension);

        return std::make_unique<std::filesystem::path>(std::move(path));
    }

    std::unique_ptr<std::filesystem::path> IOUtilities::ioDirectoryDialog(const std::string_view title) {
        NFD::UniquePath outPath;

        const nfdresult_t result = NFD::PickFolder(outPath);

        if (result != NFD_OKAY)
            return nullptr;

        return std::make_unique<std::filesystem::path>(outPath.get());
    }

    std::string IOUtilities::fileNameFormat(const unsigned int n, const std::string_view extension) {
        return std::format("{:04d}.{}", n, extension);
    }


    std::filesystem::path IOUtilities::generateFilename(const std::filesystem::path &dir, const std::string_view extension, uint32_t *cnt = nullptr) {
        unsigned int n = 0;
        std::filesystem::path p = dir;
        do {
            ++n;
            p = dir / fileNameFormat(n, extension);
        } while (std::filesystem::exists(p));
        if (cnt) *cnt = n;
        return p;
    }
}