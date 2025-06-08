//
// Created by Merutilm on 2025-06-08.
//

#include "IOUtilities.h"

#include <iostream>

#include "RFFUtilities.h"
#include "../data/ApproxTableCache.h"


std::string IOUtilities::ioFileDialog(const std::string &title, const std::string &desc, const char type,
                                      std::vector<std::string> &&extensions) {
    OPENFILENAME fn;
    ZeroMemory(&fn, sizeof(fn));

    auto ext = std::move(extensions);
    for (auto &extension: ext) {
        extension = "*." + extension;
    }

    auto display = desc + "(" + RFFUtilities::joinString(",", ext) + ")";
    auto filter = RFFUtilities::joinString(";", ext);
    auto pattern = std::vector<char>();
    pattern.insert(pattern.end(), display.begin(), display.end());
    pattern.push_back('\0');
    pattern.insert(pattern.end(), filter.begin(), filter.end());
    pattern.push_back('\0');
    pattern.push_back('\0');
    char fileNameBuffer[MAX_PATH];
    fileNameBuffer[0] = '\0';
    fn.lStructSize = sizeof(OPENFILENAME);
    fn.lpstrFile = fileNameBuffer;
    fn.lpstrFilter = pattern.data();
    fn.nMaxFile = MAX_PATH;
    fn.lpstrFile[0] = '\0';
    fn.lpstrTitle = title.data();
    fn.Flags = OFN_PATHMUSTEXIST;

    switch (type) {
        case OPEN_FILE: {
            fn.Flags |= OFN_FILEMUSTEXIST;
            if (GetOpenFileName(&fn)) return fn.lpstrFile;
            break;
        }
        case SAVE_FILE: {
            fn.Flags |= OFN_OVERWRITEPROMPT;
            if (GetSaveFileName(&fn)) return fn.lpstrFile;
            break;
        }
        default: break;
    }
    return "";
}

std::string IOUtilities::ioDirectoryDialog(const std::string &title) {
    BROWSEINFO bi = {};
    bi.lpszTitle = title.data();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    if (const LPITEMIDLIST item = SHBrowseForFolder(&bi)) {
        char path[MAX_PATH];
        SHGetPathFromIDList(item, path);
        CoTaskMemFree(item);
        return path;
    }
    return "";
}
