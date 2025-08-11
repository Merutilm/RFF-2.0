//
// Created by Merutilm on 2025-05-13.
//

#include "SettingsWindow.h"
#include "../constants/Constants.hpp"


namespace merutilm::rff2 {
    SettingsWindow::SettingsWindow(const std::wstring_view &name) {
        window = CreateWindowExW(Constants::Win32::STYLE_EX_SETTINGS_WINDOW, Constants::Win32::CLASS_SETTINGS_WINDOW,
                                name.data(),
                                WS_SYSMENU, 0, 0,
                                Constants::Win32::INIT_SETTINGS_WINDOW_WIDTH, 0, nullptr, nullptr, nullptr, nullptr);


        SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR) this);

        ShowWindow(window, SW_SHOW);
        UpdateWindow(window);
        font = reinterpret_cast<LPARAM>(CreateFont(Constants::Win32::FONT_SIZE, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE,
                                                   ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                                   CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS,
                                                   Constants::Win32::FONT_DEFAULT));
    }

    SettingsWindow::~SettingsWindow() {
        if (IsWindow(window)) {
            DestroyWindow(window);
        }
    }


    int SettingsWindow::getFixedNameWidth() const {
        RECT rect;
        GetClientRect(window, &rect);
        return (rect.right - rect.left) / Constants::Win32::SETTINGS_LABEL_WIDTH_DIVISOR;
    }

    int SettingsWindow::getFixedValueWidth() const {
        RECT rect;
        GetClientRect(window, &rect);
        return (Constants::Win32::SETTINGS_LABEL_WIDTH_DIVISOR - 1) * (rect.right - rect.left) /
               Constants::Win32::SETTINGS_LABEL_WIDTH_DIVISOR - Constants::Win32::GAP_SETTINGS_INPUT;
    }

    void SettingsWindow::adjustWindowHeight() const {
        RECT rect = {
            0, 0, Constants::Win32::INIT_SETTINGS_WINDOW_WIDTH,
            getYOffset() + Constants::Win32::SETTINGS_INPUT_HEIGHT + Constants::Win32::GAP_SETTINGS_INPUT,
        };
        AdjustWindowRectEx(&rect, Constants::Win32::STYLE_SETTINGS_WINDOW, FALSE,
                           Constants::Win32::STYLE_EX_SETTINGS_WINDOW);

        SetWindowPos(window, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
    }

    int SettingsWindow::getIndex(const HWND wnd) {
        return GetDlgCtrlID(wnd) - Constants::Win32::ID_OPTIONS & 0x00ff;
    }

    int SettingsWindow::getRadioIndex(const HWND wnd) {
        const int offset = GetDlgCtrlID(wnd) - Constants::Win32::ID_OPTIONS;
        return offset / Constants::Win32::ID_OPTIONS_RADIO;
    }

    bool SettingsWindow::checkIndex(const int index) const {
        return index >= 0 && index < callbacks.size();
    }

    int SettingsWindow::getYOffset() const {
        return elements * (Constants::Win32::SETTINGS_INPUT_HEIGHT + Constants::Win32::GAP_SETTINGS_INPUT) +
               Constants::Win32::GAP_SETTINGS_INPUT;
    }

    void SettingsWindow::createLabel(const std::string_view &settingsName, const std::string_view descriptionTitle,
                                     const std::string_view descriptionDetail, const int nw) {
        const HWND text = CreateWindowEx(0, WC_STATIC, settingsName.data(),
                                         Constants::Win32::STYLE_LABEL, 0,
                                         getYOffset(), nw,
                                         Constants::Win32::SETTINGS_INPUT_HEIGHT, window, nullptr, nullptr, nullptr);
        const HWND tooltip = CreateWindowEx(Constants::Win32::STYLE_EX_TOOLTIP, TOOLTIPS_CLASS, "",
                                            Constants::Win32::STYLE_TOOLTIP,
                                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, window, nullptr,
                                            nullptr, nullptr);

        TOOLINFO toolInfo = {};
        toolInfo.cbSize = sizeof(toolInfo);
        toolInfo.hwnd = text;
        toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
        toolInfo.uId = reinterpret_cast<UINT_PTR>(text);
        toolInfo.lpszText = LPSTR(descriptionDetail.data());

        SendMessage(tooltip, TTM_SETMAXTIPWIDTH, 0, Constants::Win32::INIT_SETTINGS_WINDOW_WIDTH); //enable line break
        SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM) &toolInfo);
        SendMessage(tooltip, TTM_SETTITLE, 0, (LPARAM) descriptionTitle.data());
        SendMessage(tooltip, WM_SETFONT, font, TRUE);
        SendMessage(text, WM_SETFONT, font, TRUE);

        createdChildWindows.push_back(tooltip);
        createdChildWindows.push_back(text);
    }


    LRESULT SettingsWindow::settingsWindowProc(const HWND window, const UINT message, const WPARAM wParam,
                                               const LPARAM lParam) {
        SettingsWindow &wnd = *reinterpret_cast<SettingsWindow *>(GetWindowLongPtr(window, GWLP_USERDATA));

        switch (message) {
            case WM_COMMAND: {
                const auto editor = reinterpret_cast<HWND>(lParam);
                if (
                    const int index = getIndex(editor);
                    wnd.checkIndex(index) &&
                    LOWORD(wParam) != 0
                ) {
                    if (HIWORD(wParam) == CBN_SELCHANGE) {
                        const auto combobox = (HWND) lParam;
                        const auto selectedIndex = static_cast<int>(SendMessage(combobox, CB_GETCURSEL, 0, 0));
                        std::any &value = (*wnd.enumValues[index])[selectedIndex];
                        (*wnd.callbacks[index])(value);
                        wnd.references[index] = value;
                    }
                    if (HIWORD(wParam) == BN_CLICKED) {
                        const int radioIndex = getRadioIndex(editor);
                        std::any &value = (*wnd.enumValues[index])[radioIndex];
                        (*wnd.callbacks[index])(value);
                        wnd.references[index] = value;
                    }
                }

                return 0;
            }
            case WM_CLOSE: {
                for (const auto hwnd: wnd.createdChildWindows) DestroyWindow(hwnd);
                DestroyWindow(window);
                wnd.windowCloseFunction();
                return 0;
            }
            case WM_DESTROY: {
                DeleteObject(reinterpret_cast<HGDIOBJ>(wnd.font));
                PostQuitMessage(0);
                return 0;
            }
            case WM_CTLCOLOREDIT: {
                const auto hdcEdit = (HDC) wParam;
                const auto hwndEdit = (HWND) lParam;
                const int index = getIndex(hwndEdit);

                if (!wnd.checkIndex(index)) {
                    return DefWindowProc(window, message, wParam, lParam);
                }

                if (wnd.error[index]) {
                    SetTextColor(hdcEdit, Constants::Win32::COLOR_TEXT_ERROR);
                } else if (wnd.edited[index]) {
                    SetTextColor(hdcEdit, Constants::Win32::COLOR_TEXT_EDITED);
                } else if (wnd.modified[index]) {
                    SetTextColor(hdcEdit, Constants::Win32::COLOR_TEXT_MODIFIED);
                } else {
                    SetTextColor(hdcEdit, Constants::Win32::COLOR_TEXT_DEFAULT);
                }
                return (INT_PTR) GetStockObject(WHITE_BRUSH);
            }

            case WM_CTLCOLORSTATIC: {
                const auto hwndStatic = (HWND) lParam;
                return IsWindowEnabled(hwndStatic)
                           ? (INT_PTR) GetStockObject(WHITE_BRUSH)
                           : DefWindowProc(window, message, wParam, lParam);
            }
            default: return DefWindowProc(window, message, wParam, lParam);
        }
    }

    LRESULT SettingsWindow::textFieldProc(const HWND window, const UINT message, const WPARAM wParam,
                                          const LPARAM lParam,
                                          [[maybe_unused]] UINT_PTR uIdSubclass,
                                          [[maybe_unused]] DWORD_PTR dwRefData) {
        auto &wnd = *reinterpret_cast<SettingsWindow *>(GetWindowLongPtr(window, GWLP_USERDATA));

        if (message == WM_KEYDOWN) {
            const int index = getIndex(window);
            if (wnd.checkIndex(index)) {
                wnd.edited[index] = true;
            }
            if (wParam == VK_ESCAPE && wnd.checkIndex(index)) {
                SetWindowText(window, (*wnd.unparsers[index])(wnd.references[index]).data());
            }
            if (wParam == VK_RETURN && wnd.checkIndex(index)) {
                const int length = GetWindowTextLength(window) + 1; //include NULL character
                std::string buf(length, '\0');
                GetWindowText(window, buf.data(), length);

                const HDC hdc = GetDC(window);
                try {
                    if (std::any value = (*wnd.parsers[index])(buf);
                        (*wnd.validConditions[index])(value)
                    ) {
                        (*wnd.callbacks[index])(value);
                        wnd.references[index] = value;
                        wnd.modified[index] = true;
                        wnd.edited[index] = false;
                    } else {
                        wnd.callError(index);
                    }
                } catch (std::invalid_argument &) {
                    wnd.callError(index);
                }
                SetWindowText(window, wnd.currValueToString(index).data());
                ReleaseDC(window, hdc);
            }
        }

        return DefSubclassProc(window, message, wParam, lParam);
    }

    void SettingsWindow::callError(const int index) {
        error[index] = true;
        MessageBox(window, "Invalid value!!", "Error", MB_OK | MB_ICONERROR);
        error[index] = false;
    }


    std::string SettingsWindow::currValueToString(const int index) const {
        return (*unparsers[index])(references[index]);
    }


    void SettingsWindow::setWindowCloseFunction(std::function<void()> &&function) {
        this->windowCloseFunction = std::move(function);
    }
}
