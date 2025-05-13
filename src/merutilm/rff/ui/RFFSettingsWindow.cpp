//
// Created by Merutilm on 2025-05-13.
//

#include "RFFSettingsWindow.h"
#include "RFFConstants.h"


RFFSettingsWindow::RFFSettingsWindow(const std::string_view &name) {
    window = CreateWindowEx(RFFConstants::Win32::STYLE_EX_SETTINGS_WINDOW, RFFConstants::Win32::CLASS_SETTINGS_WINDOW,
                            name.data(),
                            WS_SYSMENU, 0, 0,
                            RFFConstants::Win32::INIT_SETTINGS_WINDOW_WIDTH,
                            RFFConstants::Win32::INIT_SETTINGS_WINDOW_MIN_HEIGHT, nullptr, nullptr, nullptr, nullptr);


    SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR) this);

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    font = reinterpret_cast<LPARAM>(CreateFont(RFFConstants::Win32::FONT_SIZE, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE,
                                               ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                               CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS,
                                               RFFConstants::Win32::FONT_DEFAULT));
}


int RFFSettingsWindow::getFixedNameWidth() const {
    RECT rect;
    GetClientRect(window, &rect);
    return (rect.right - rect.left) / RFFConstants::Win32::SETTINGS_LABEL_WIDTH_DIVISOR;
}

int RFFSettingsWindow::getFixedValueWidth() const {
    RECT rect;
    GetClientRect(window, &rect);
    return (RFFConstants::Win32::SETTINGS_LABEL_WIDTH_DIVISOR - 1) * (rect.right - rect.left) /
           RFFConstants::Win32::SETTINGS_LABEL_WIDTH_DIVISOR - RFFConstants::Win32::GAP_SETTINGS_INPUT;
}

void RFFSettingsWindow::adjustWindowHeight() const {
    SetWindowPos(window, nullptr, 0, 0, RFFConstants::Win32::INIT_SETTINGS_WINDOW_WIDTH,
                 std::max<int>(RFFConstants::Win32::INIT_SETTINGS_WINDOW_MIN_HEIGHT,
                               getYOffset() + RFFConstants::Win32::HEIGHT_SETTINGS_INPUT), SWP_NOMOVE);
}

void RFFSettingsWindow::initClass() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpszClassName = RFFConstants::Win32::CLASS_SETTINGS_WINDOW;
    wc.lpfnWndProc = settingsProc;
    RegisterClassEx(&wc);
}

int RFFSettingsWindow::getIndex(const HWND wnd) {
    return GetDlgCtrlID(wnd) - RFFConstants::Win32::ID_OPTIONS & 0x00ff;
}

int RFFSettingsWindow::getRadioIndex(const HWND wnd) {
    const int offset = GetDlgCtrlID(wnd) - RFFConstants::Win32::ID_OPTIONS;
    return offset / RFFConstants::Win32::ID_OPTIONS_RADIO;
}

bool RFFSettingsWindow::checkIndex(const int index) const {
    return index >= 0 && index < callbacks.size();
}

int RFFSettingsWindow::getYOffset() const {
    return elements * (getInputHeight() + RFFConstants::Win32::GAP_SETTINGS_INPUT) +
           RFFConstants::Win32::GAP_SETTINGS_INPUT;
}

int RFFSettingsWindow::getInputHeight() {
    return RFFConstants::Win32::HEIGHT_SETTINGS_INPUT;
}

void RFFSettingsWindow::createLabel(const std::string_view &settingsName, const std::string_view descriptionTitle,
                                    const std::string_view descriptionDetail, const int nw) {
    const HWND text = CreateWindowEx(0, WC_STATIC, settingsName.data(),
                                     RFFConstants::Win32::STYLE_LABEL, 0,
                                     getYOffset(), nw,
                                     getInputHeight(), window, nullptr, nullptr, nullptr);
    const HWND tooltip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, "",
                                        WS_POPUP | TTS_NOPREFIX | TTS_BALLOON | TTS_ALWAYSTIP,
                                        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, window, nullptr,
                                        nullptr, nullptr);

    TOOLINFO toolInfo = {};
    toolInfo.cbSize = sizeof(toolInfo);
    toolInfo.hwnd = text;
    toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    toolInfo.uId = reinterpret_cast<UINT_PTR>(text);
    toolInfo.lpszText = LPSTR(descriptionDetail.data());

    SendMessage(tooltip, TTM_SETMAXTIPWIDTH, 0, RFFConstants::Win32::INIT_SETTINGS_WINDOW_WIDTH); //enable line break
    SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM) &toolInfo);
    SendMessage(tooltip, TTM_SETTITLE, 0, (LPARAM) descriptionTitle.data());
    SendMessage(tooltip, WM_SETFONT, font, TRUE);
    SendMessage(text, WM_SETFONT, font, TRUE);

    createdChildWindows.push_back(tooltip);
    createdChildWindows.push_back(text);
}


LRESULT RFFSettingsWindow::settingsProc(const HWND window, const UINT message, const WPARAM wParam,
                                        const LPARAM lParam) {
    RFFSettingsWindow &wnd = *reinterpret_cast<RFFSettingsWindow *>(GetWindowLongPtr(window, GWLP_USERDATA));

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
                    (*wnd.callbacks[index])((*wnd.valuesSel[index])[selectedIndex]);
                }
                if (HIWORD(wParam) == BN_CLICKED) {
                    const int radioIndex = getRadioIndex(editor);
                    (*wnd.callbacks[index])((*wnd.valuesSel[index])[radioIndex]);
                }
            }

            return 0;
        }
        case WM_CLOSE: {
            for (const auto hwnd: wnd.createdChildWindows) DestroyWindow(hwnd);
            DestroyWindow(window);
            wnd.windowCloseFunction(wnd);
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
                SetTextColor(hdcEdit, RFFConstants::Win32::COLOR_TEXT_ERROR);
            } else if (wnd.edited[index]) {
                SetTextColor(hdcEdit, RFFConstants::Win32::COLOR_TEXT_EDITED);
            } else if (wnd.modified[index]) {
                SetTextColor(hdcEdit, RFFConstants::Win32::COLOR_TEXT_MODIFIED);
            } else {
                SetTextColor(hdcEdit, RFFConstants::Win32::COLOR_TEXT_DEFAULT);
            }
            return (INT_PTR) GetStockObject(WHITE_BRUSH);
        }

        case WM_CTLCOLORSTATIC: {
            return (INT_PTR) GetStockObject(WHITE_BRUSH);
        }
        default: return DefWindowProc(window, message, wParam, lParam);
    }
}

LRESULT RFFSettingsWindow::textFieldProc(const HWND window, const UINT message, const WPARAM wParam,
                                         const LPARAM lParam,
                                         [[maybe_unused]] UINT_PTR uIdSubclass,
                                         [[maybe_unused]] DWORD_PTR dwRefData) {
    auto &wnd = *reinterpret_cast<RFFSettingsWindow *>(GetWindowLongPtr(window, GWLP_USERDATA));

    if (message == WM_KEYDOWN) {
        const int index = getIndex(window);
        if (wnd.checkIndex(index)) {
            wnd.edited[index] = true;
        }
        if (wParam == VK_ESCAPE && wnd.checkIndex(index)) {
            SetWindowText(window, wnd.previousInputs[index].data());
        }
        if (wParam == VK_RETURN && wnd.checkIndex(index)) {
            const int length = GetWindowTextLength(window) + 1; //include NULL character
            std::string buf(length, '\0');
            GetWindowText(window, buf.data(), length);

            const HDC hdc = GetDC(window);
            try {
                if (const std::any &value = (*wnd.parsers[index])(buf);
                    (*wnd.validConditions[index])(value)
                ) {
                    (*wnd.callbacks[index])(value);
                    wnd.previousInputs[index] = (*wnd.unparsers[index])(value);
                    wnd.modified[index] = true;
                    wnd.edited[index] = false;
                } else {
                    wnd.error[index] = true;
                    errorMessage(window);
                    wnd.error[index] = false;
                }
                SetWindowText(window, wnd.previousInputs[index].data());
            } catch (std::invalid_argument &) {
                wnd.error[index] = true;
                errorMessage(window);
                SetWindowText(window, wnd.previousInputs[index].data());
                wnd.error[index] = false;
            }
            ReleaseDC(window, hdc);
        }
    }

    return DefSubclassProc(window, message, wParam, lParam);
}


void RFFSettingsWindow::setWindowCloseFunction(std::function<void(RFFSettingsWindow &)> &&function) {
    this->windowCloseFunction = std::move(function);
}
