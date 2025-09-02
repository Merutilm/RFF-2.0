//
// Created by Merutilm on 2025-06-08.
//

#include "IOUtilities.h"

#include <filesystem>

#include "Utilities.h"
#include "../data/ApproxTableCache.h"


namespace merutilm::rff2 {
    std::unique_ptr<std::filesystem::path> IOUtilities::ioFileDialog(const std::wstring_view title,
                                                                     const std::wstring_view desc,
                                                                     const char type,
                                                                     const std::wstring_view extension) {
        OPENFILENAMEW fn;
        ZeroMemory(&fn, sizeof(fn));

        auto display = std::format(L"{}(*.{})", desc, extension);
        auto pattern = std::vector<wchar_t>();
        pattern.insert(pattern.end(), display.begin(), display.end());
        pattern.push_back(L'\0');

        const auto filter = std::format(L"*.{}", extension);
        pattern.insert(pattern.end(), filter.begin(), filter.end());
        pattern.push_back(L'\0');
        pattern.push_back(L'\0');
        wchar_t fileNameBuffer[MAX_PATH];
        fileNameBuffer[0] = L'\0';
        fn.lStructSize = sizeof(OPENFILENAME);
        fn.lpstrFile = fileNameBuffer;
        fn.lpstrFilter = pattern.data();
        fn.nMaxFile = MAX_PATH;
        fn.lpstrFile[0] = '\0';
        fn.lpstrTitle = title.data();
        fn.Flags = OFN_PATHMUSTEXIST;
        const std::wstring end = std::format(L".{}", extension.data());

        switch (type) {
            case OPEN_FILE: {
                fn.Flags |= OFN_FILEMUSTEXIST;
                if (GetOpenFileNameW(&fn)) {
                    std::wstring result = fn.lpstrFile;
                    if (!Utilities::endsWith(result, end)) {
                        result.append(end);
                    }
                    return std::make_unique<std::filesystem::path>(result);
                }
                break;
            }
            case SAVE_FILE: {
                fn.Flags |= OFN_OVERWRITEPROMPT;
                if (GetSaveFileNameW(&fn)) {
                    std::wstring result = fn.lpstrFile;
                    if (!Utilities::endsWith(result, end)) {
                        result.append(end);
                    }
                    return std::make_unique<std::filesystem::path>(result);
                }
                break;
            }
            default: break;
        }
        return nullptr;
    }

    std::unique_ptr<std::filesystem::path> IOUtilities::ioDirectoryDialog(const std::wstring_view title) {
        BROWSEINFOW bi = {};
        bi.lpszTitle = title.data();
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

        if (const LPITEMIDLIST item = SHBrowseForFolderW(&bi)) {
            char path[MAX_PATH];
            SHGetPathFromIDList(item, path);
            CoTaskMemFree(item);
            return std::make_unique<std::filesystem::path>(path);
        }
        return nullptr;
    }

    std::wstring IOUtilities::fileNameFormat(const unsigned int n, const std::wstring_view extension) {
        return std::format(L"{:04d}.{}", n, extension);
    }

    std::filesystem::path IOUtilities::generateFileName(const std::filesystem::path &dir, const std::wstring_view extension) {
        unsigned int n = 0;
        std::filesystem::path p = dir;
        do {
            ++n;
            p = dir / fileNameFormat(n, extension);
        } while (std::filesystem::exists(p));
        return p;
    }

    uint32_t IOUtilities::fileNameCount(const std::filesystem::path &dir, const std::wstring_view extension) {
        unsigned int n = 0;
        std::filesystem::path p = dir;
        do {
            ++n;
            p = dir / fileNameFormat(n, extension);
        } while (std::filesystem::exists(p));
        return n - 1;
    }
}