//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include <windows.h>
#include <commctrl.h>

namespace merutilm::rff2::Constants::Win32 {

        constexpr int INIT_RENDER_SCENE_WIDTH = 1280;
        constexpr int INIT_RENDER_SCENE_HEIGHT = 720;
        constexpr int MIN_WINDOW_WIDTH = 300;
        constexpr int MIN_WINDOW_HEIGHT = 300;
        constexpr float INIT_RENDER_SCENE_FPS = 60;
        constexpr int INIT_SETTINGS_WINDOW_WIDTH = 700;
        constexpr int PROGRESS_BAR_HEIGHT = 40;
        constexpr int SETTINGS_INPUT_HEIGHT = 30;
        constexpr int GAP_SETTINGS_INPUT = 15;
        constexpr int SETTINGS_LABEL_WIDTH_DIVISOR = 2;
        constexpr int MAX_AMOUNT_COMBOBOX = 7;
        constexpr auto CLASS_MASTER_WINDOW = L"RFF2MW";
        constexpr auto CLASS_SETTINGS_WINDOW = L"RFF2SW";
        constexpr auto CLASS_VIDEO_WINDOW = L"RFF2VW";
        constexpr auto CLASS_VIDEO_RENDER_WINDOW = L"RFF2VRW";
        constexpr auto CLASS_VK_RENDER_SCENE = L"RFF2VRS";
        constexpr auto FONT_DEFAULT = L"Segoe UI";
        constexpr int FONT_SIZE = 25;
        constexpr DWORD STYLE_EX_TOOLTIP = WS_EX_TOPMOST;
        constexpr DWORD STYLE_EX_SETTINGS_WINDOW = WS_EX_TOOLWINDOW | WS_EX_TOPMOST;
        constexpr DWORD STYLE_SETTINGS_WINDOW = WS_SYSMENU | WS_BORDER;
        constexpr DWORD STYLE_TOOLTIP = TTS_NOPREFIX | TTS_BALLOON | TTS_ALWAYSTIP;
        constexpr DWORD STYLE_LABEL = WS_CHILD | WS_VISIBLE | ES_CENTER | SS_NOTIFY;
        constexpr DWORD STYLE_RADIOBUTTON = WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON;
        constexpr DWORD STYLE_CHECKBOX = WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX;
        constexpr DWORD STYLE_TEXT_FIELD = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_AUTOHSCROLL;
        constexpr DWORD STYLE_COMBOBOX = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | ES_CENTER | CBS_DROPDOWNLIST |
                                         WS_VSCROLL;
        constexpr int ID_MENUS = 0x2000;
        constexpr int ID_OPTIONS = 0x1000;
        constexpr int ID_OPTIONS_RADIO = 0x0100;
        constexpr int ID_OPTIONS_CHECKBOX_FLAG = 0x0080;
        constexpr COLORREF COLOR_PROGRESS_BACKGROUND_PROG = RGB(40, 140, 40);
        constexpr COLORREF COLOR_PROGRESS_BACKGROUND_BACK = RGB(40, 40, 40);
        constexpr COLORREF COLOR_PROGRESS_TEXT_PROG = RGB(0, 0, 0);
        constexpr COLORREF COLOR_PROGRESS_TEXT_BACK = RGB(255, 255, 255);
        constexpr COLORREF COLOR_TEXT_ERROR = RGB(255, 0, 0);
        constexpr COLORREF COLOR_TEXT_EDITED = RGB(100, 0, 0);
        constexpr COLORREF COLOR_TEXT_MODIFIED = RGB(0, 110, 160);
        constexpr COLORREF COLOR_TEXT_DEFAULT = RGB(0, 0, 0);
        constexpr COLORREF COLOR_LABEL_BACKGROUND = RGB(255, 255, 255);
}
