//
// Created by Merutilm on 2025-05-13.
//

#pragma once
#include <any>
#include <functional>
#include <iostream>
#include <string>

#include "../constants/Constants.hpp"
#include <windows.h>
#include <commctrl.h>
#include <memory>
#include <optional>

#include "../attr/Selectable.h"

namespace merutilm::rff2 {
    class SettingsWindow {
        HWND window;
        int count = 0;
        int elements = 0;
        std::vector<std::any> references;
        std::vector<std::unique_ptr<std::function<std::any(std::wstring &)> > > parsers;
        std::vector<std::unique_ptr<std::function<std::wstring(const std::any &)> > > unparsers;
        std::vector<std::unique_ptr<std::function<bool(const std::any &)> > > validConditions;
        std::vector<std::unique_ptr<std::function<void(std::any &)> > > callbacks;
        std::vector<std::unique_ptr<std::vector<std::any> > > enumValues;
        std::vector<HWND> createdChildWindows;
        std::vector<bool> error;
        std::vector<bool> edited;
        std::vector<bool> modified;
        std::function<void()> windowCloseFunction;
        LPARAM font;

    public:
        explicit SettingsWindow(const std::wstring &name);

        ~SettingsWindow();

        SettingsWindow(const SettingsWindow &) = delete;

        SettingsWindow &operator=(const SettingsWindow &) = delete;

        SettingsWindow(SettingsWindow &&) = delete;

        SettingsWindow &operator=(SettingsWindow &&) = delete;


        void setWindowCloseFunction(std::function<void()> &&function);

        template<typename T>
        HWND registerTextInput(const std::wstring &settingsName, T *ptr,
                               std::function<std::wstring(const T &)> &&unparser,
                               std::function<T(std::wstring &)> &&parser,
                               std::function<bool(const T &)> &&validCondition,
                               std::function<void()> &&callback, const std::wstring &descriptionTitle,
                               const std::wstring &descriptionDetail);


        template<typename T> requires std::is_enum_v<T>
        HWND registerSelectionInput(const std::wstring &settingsName, T *ptr, std::function<void()> &&callback,
                                    const std::wstring &descriptionTitle, const std::wstring &descriptionDetail);


        template<typename T> requires std::is_enum_v<T> || std::is_same_v<T, bool>
        std::vector<HWND> registerRadioButtonInput(const std::wstring &settingsName, T *defaultValue,
                                                   std::function<void()> &&callback,
                                                   const std::wstring &descriptionTitle,
                                                   const std::wstring &descriptionDetail);

        HWND registerCheckboxInput(const std::wstring &settingsName, bool *defaultValue,
                                            std::function<void()> &&callback, const std::wstring &descriptionTitle,
                                            const std::wstring &descriptionDetail);


        [[nodiscard]] HWND getWindow() const;


        static LRESULT CALLBACK settingsWindowProc(HWND window, UINT message, WPARAM wParam,
                                                   LPARAM lParam);

        static LRESULT CALLBACK textFieldProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam,
                                              UINT_PTR uIdSubclass,
                                              DWORD_PTR dwRefData);

    private:
        static int getIndex(HWND wnd);

        static bool isCheckbox(HWND wnd);

        static int getRadioIndex(HWND wnd);

        [[nodiscard]] bool checkIndex(int index) const;

        [[nodiscard]] int getYOffset() const;

        void createLabel(const std::wstring &settingsName, const std::wstring &descriptionTitle,
                         const std::wstring &descriptionDetail, int nw);

        template<typename T>
        void registerActions(T *defaultValuePtr,
                             const std::function<std::wstring(const T &)> &unparser,
                             const std::optional<std::function<T(std::wstring &)> > &parser,
                             const std::optional<std::function<bool(const T &)> > &validCondition,
                             const std::function<void()> &callback,
                             std::optional<std::vector<T> > values);


        [[nodiscard]] std::wstring currValueToString(int index) const;

        [[nodiscard]] int getFixedNameWidth() const;

        [[nodiscard]] int getFixedValueWidth() const;

        void adjustWindowHeight() const;

        void callError(int index);
    };


    // DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW
    // DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW
    // DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW
    // DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW
    // DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW  DEFINITION OF RFF SETTINGS WINDOW


    template<typename T>
    HWND SettingsWindow::registerTextInput(const std::wstring &settingsName, T *ptr,
                                           std::function<std::wstring(const T &)> &&unparser,
                                           std::function<T(std::wstring &)> &&parser,
                                           std::function<bool(const T &)> &&validCondition,
                                           std::function<void()> &&callback,
                                           const std::wstring &descriptionTitle,
                                           const std::wstring &descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();


        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const HWND text = CreateWindowExW(0, WC_EDITW, unparser(*ptr).data(),
                                          Constants::Win32::STYLE_TEXT_FIELD, nw,
                                          getYOffset(), vw,
                                          Constants::Win32::SETTINGS_INPUT_HEIGHT, window,
                                          reinterpret_cast<HMENU>(Constants::Win32::ID_OPTIONS + count),
                                          nullptr,
                                          nullptr);

        SetWindowSubclass(text, textFieldProc, 1, 0);
        SetWindowLongPtr(text, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        SendMessage(text, WM_SETFONT, font, TRUE);
        SendMessage(text, EM_SETLIMITTEXT, 0, 0);
        createdChildWindows.push_back(text);
        ++elements;
        registerActions<T>(ptr, std::move(unparser), std::optional{std::move(parser)},
                           std::optional{std::move(validCondition)},
                           std::move(callback), std::nullopt);
        adjustWindowHeight();

        return text;
    }

    template<typename T> requires std::is_enum_v<T>
    HWND SettingsWindow::registerSelectionInput(const std::wstring &settingsName, T *ptr,
                                                std::function<void()> &&callback,
                                                const std::wstring &descriptionTitle,
                                                const std::wstring &descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();
        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const HWND combobox = CreateWindowExW(0, WC_COMBOBOXW,
                                              L"",
                                              Constants::Win32::STYLE_COMBOBOX, nw,
                                              getYOffset(), vw,
                                              Constants::Win32::SETTINGS_INPUT_HEIGHT *
                                              Constants::Win32::MAX_AMOUNT_COMBOBOX,
                                              window,
                                              reinterpret_cast<HMENU>(Constants::Win32::ID_OPTIONS + count),
                                              nullptr,
                                              nullptr);
        std::vector<T> values = Selectable::values<T>();
        int defaultValueIndex = 0;

        for (int i = 0; i < values.size(); ++i) {
            SendMessageW(combobox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Selectable::toString(values[i]).data()));
            SendMessageW(combobox, WM_SETFONT, font, TRUE);
            if (values[i] == *ptr) {
                defaultValueIndex = i;
            }
        }

        SendMessageW(combobox, CB_SETCURSEL, defaultValueIndex, 0);
        auto unparser = [](T v) { return Selectable::toString(v); };
        createdChildWindows.push_back(combobox);
        ++elements;
        registerActions<T>(ptr, unparser, std::nullopt, std::nullopt, std::move(callback), values);
        adjustWindowHeight();
        return combobox;
    }

    template<typename T> requires std::is_enum_v<T> || std::is_same_v<T, bool>
    std::vector<HWND> SettingsWindow::registerRadioButtonInput(const std::wstring &settingsName, T *defaultValue,
                                                               std::function<void()> &&callback,
                                                               const std::wstring &descriptionTitle,
                                                               const std::wstring &descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();
        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const auto values = Selectable::values<T>();
        HWND defaultValueItem = nullptr;
        auto createdItem = std::vector<HWND>();
        createdItem.reserve(values.size());

        for (int i = 0; i < values.size(); ++i) {
            const HWND item = CreateWindowExW(0, WC_BUTTONW,
                                              Selectable::toString(values[i]).data(),
                                              Constants::Win32::STYLE_RADIOBUTTON | (i == 0 ? WS_GROUP : 0), nw,
                                              getYOffset(), vw,
                                              Constants::Win32::SETTINGS_INPUT_HEIGHT, window,
                                              reinterpret_cast<HMENU>(
                                                  Constants::Win32::ID_OPTIONS + i *
                                                  Constants::Win32::ID_OPTIONS_RADIO + count),
                                              nullptr,
                                              nullptr);

            SendMessage(item, WM_SETFONT, font, TRUE);
            createdItem.push_back(item);
            createdChildWindows.push_back(item);
            ++elements;
            if (values[i] == *defaultValue) {
                defaultValueItem = item;
            }
        }
        SendMessage(defaultValueItem, BM_SETCHECK, BST_CHECKED, 0);
        auto unparser = [](T v) { return Selectable::toString(v); };
        registerActions<T>(defaultValue, unparser, std::nullopt, std::nullopt, std::move(callback),
                           values);
        adjustWindowHeight();
        return createdItem;
    }


    inline HWND SettingsWindow::registerCheckboxInput(const std::wstring &settingsName, bool *defaultValue,
                                                               std::function<void()> &&callback,
                                                               const std::wstring &descriptionTitle,
                                                               const std::wstring &descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();
        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const auto item = CreateWindowExW(0, WC_BUTTONW, L"", Constants::Win32::STYLE_CHECKBOX, nw, getYOffset(), vw,
                                    Constants::Win32::SETTINGS_INPUT_HEIGHT, window, reinterpret_cast<HMENU>(
                                        Constants::Win32::ID_OPTIONS + Constants::Win32::ID_OPTIONS_CHECKBOX_FLAG + count), nullptr, nullptr);

        SendMessage(item, WM_SETFONT, font, TRUE);
        SendMessage(item, BM_SETCHECK, *defaultValue ? BST_CHECKED : BST_UNCHECKED, 0);
        auto unparser = [](const bool v) { return Selectable::toString(v); };
        ++elements;
        registerActions<bool>(defaultValue, unparser, std::nullopt, std::nullopt, std::move(callback), std::nullopt);
        adjustWindowHeight();
        return item;
    }


    inline HWND SettingsWindow::getWindow() const {
        return window;
    }

    template<typename T>
    void SettingsWindow::registerActions(T *defaultValuePtr, const std::function<std::wstring(const T &)> &unparser,
                                         const std::optional<std::function<T(std::wstring &)> > &parser,
                                         const std::optional<std::function<bool(const T &)> > &validCondition,
                                         const std::function<void()> &callback,
                                         const std::optional<std::vector<T> > values) {
        const std::any defaultValue = *defaultValuePtr;
        references.emplace_back(defaultValue);
        unparsers.emplace_back(std::make_unique<std::function<std::wstring(const std::any &)> >(
            [unparser](const std::any &value) {
                return unparser(std::any_cast<T>(value));
            }
        ));

        parsers.emplace_back(parser == std::nullopt
                                 ? nullptr
                                 : std::make_unique<std::function<std::any(std::wstring &)> >(
                                     [parser](std::wstring &value) {
                                         auto f = *parser;
                                         std::any result = f(value);
                                         return result;
                                     }));


        validConditions.emplace_back(validCondition == std::nullopt
                                         ? nullptr
                                         : std::make_unique<std::function<bool(const std::any &)> >(
                                             [validCondition](const std::any &value) {
                                                 auto f = *validCondition;
                                                 return f(std::any_cast<T>(value));
                                             }
                                         )
        );

        callbacks.emplace_back(std::make_unique<std::function<void(std::any &v)> >(
            [defaultValuePtr, callback](std::any &v) {
                *defaultValuePtr = std::any_cast<T &>(v);
                callback();
            }
        ));
        enumValues.emplace_back(values == std::nullopt
                                    ? nullptr
                                    : [&values] {
                                        auto result = std::make_unique<std::vector<std::any> >();
                                        result->reserve((*values).size());
                                        for (const auto &value: *values) {
                                            result->push_back(value);
                                        }
                                        return result;
                                    }());
        error.emplace_back(false);
        edited.emplace_back(false);
        modified.emplace_back(false);
        ++count;
    }
}
