//
// Created by Merutilm on 2025-05-13.
//

#pragma once
#include <any>
#include <functional>
#include <iostream>
#include <string>

#include "RFFConstants.h"
#include <windows.h>
#include <commctrl.h>
#include <memory>
#include <optional>

#include "../settings/Selectable.h"

class RFFSettingsWindow {
    HWND window;
    int count = 0;
    int elements = 0;
    std::vector<std::unique_ptr<std::function<std::any(std::string &)> > > parsers;
    std::vector<std::unique_ptr<std::function<std::string(const std::any &)> > > unparsers;
    std::vector<std::unique_ptr<std::function<bool(const std::any &)> > > validConditions;
    std::vector<std::unique_ptr<std::function<void(const std::any &)> > > callbacks;
    std::vector<std::unique_ptr<std::vector<std::any> > > valuesSel;
    std::vector<HWND> createdChildWindows;
    std::vector<std::string> previousInputs;
    std::vector<bool> error;
    std::vector<bool> edited;
    std::vector<bool> modified;
    std::function<void(RFFSettingsWindow &)> windowCloseFunction;
    LPARAM font;

public:
    explicit RFFSettingsWindow(const std::string_view &name);

    template<typename T>
    void registerTextInput(const std::string_view &settingsName, const T &defaultValue,
                           std::function<std::string(const T &)> &&unparser,
                           std::function<T(std::string &)> &&parser,
                           std::function<bool(const T &)> &&validCondition,
                           std::function<void(const T &)> &&callback, const std::string_view descriptionTitle, const std::string_view descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();
        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const HWND text = CreateWindowEx(0, WC_EDIT, unparser(defaultValue).data(),
                                         RFFConstants::Win32::STYLE_TEXT_FIELD, nw,
                                         getYOffset(), vw,
                                         getInputHeight(), window,
                                         reinterpret_cast<HMENU>(RFFConstants::Win32::ID_OPTIONS + count),
                                         nullptr,
                                         nullptr);

        SetWindowSubclass(text, textFieldProc, 1, 0);
        SetWindowLongPtr(text, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        registerActions<T>(defaultValue, std::optional{std::move(unparser)}, std::optional{std::move(parser)},
                           std::optional{std::move(validCondition)},
                           std::optional{std::move(callback)}, std::nullopt);

        SendMessage(text, WM_SETFONT, font, TRUE);
        createdChildWindows.push_back(text);
        ++elements;
    }

    template<typename T> requires std::is_enum_v<T>
    void registerSelectionInput(const std::string_view &settingsName, const T &defaultValue,
                                std::function<void(const T &)> &&callback, const std::string_view descriptionTitle, const std::string_view descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();
        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const HWND combobox = CreateWindowEx(0, WC_COMBOBOX,
                                             "",
                                             RFFConstants::Win32::STYLE_COMBOBOX, nw,
                                             getYOffset(), vw,
                                             getInputHeight() * RFFConstants::Win32::MAX_AMOUNT_COMBOBOX, window,
                                             reinterpret_cast<HMENU>(RFFConstants::Win32::ID_OPTIONS + count),
                                             nullptr,
                                             nullptr);
        std::vector<T> values = Selectable::values<T>();
        int defaultValueIndex = 0;

        for (int i = 0; i < values.size(); ++i) {
            SendMessage(combobox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Selectable::toString(values[i]).data()));
            SendMessage(combobox, WM_SETFONT, font, TRUE);
            if (values[i] == defaultValue) {
                defaultValueIndex = i;
            }
        }

        SendMessage(combobox, CB_SETCURSEL, defaultValueIndex, 0);
        auto unparser = [](T v) { return Selectable::toString(v); };
        registerActions<T>(defaultValue, unparser, std::nullopt, std::nullopt, std::move(callback), values);
        createdChildWindows.push_back(combobox);
        ++elements;
    }

    template<typename T> requires std::is_enum_v<T> || std::is_same_v<T, bool>
    void registerRadioButtonInput(const std::string_view &settingsName, const T &defaultValue,
                                  std::function<void(const T &)> &&callback, const std::string_view descriptionTitle, const std::string_view descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();
        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const auto values = Selectable::values<T>();
        HWND defaultValueItem = nullptr;


        for (int i = 0; i < values.size(); ++i) {
            const HWND item = CreateWindowEx(0, WC_BUTTON,
                                             Selectable::toString(values[i]).data(),
                                             RFFConstants::Win32::STYLE_RADIOBUTTON | (i == 0 ? WS_GROUP : 0) , nw,
                                             getYOffset(), vw,
                                             getInputHeight(), window,
                                             reinterpret_cast<HMENU>(
                                                 RFFConstants::Win32::ID_OPTIONS + i *
                                                 RFFConstants::Win32::ID_OPTIONS_RADIO + count),
                                             nullptr,
                                             nullptr);

            SendMessage(item, WM_SETFONT, font, TRUE);
            createdChildWindows.push_back(item);
            ++elements;
            if (values[i] == defaultValue) {
                defaultValueItem = item;
            }
        }
        SendMessage(defaultValueItem, BM_SETCHECK, BST_CHECKED, 0);
        auto unparser = [](T v) { return Selectable::toString(v); };
        registerActions<T>(defaultValue, unparser, std::nullopt, std::nullopt, std::move(callback), values);
    }

    void registerBoolInput(const std::string_view &settingsName, const bool &defaultValue, std::function<void(const bool &)> &&callback, std::string_view descriptionTitle, std::string_view descriptionDetail) {
        registerRadioButtonInput(settingsName, defaultValue, std::move(callback), descriptionTitle, descriptionDetail);
    }



    void setWindowCloseFunction(std::function<void(RFFSettingsWindow &)> &&function);

    static void initClass();

    HWND getWindow() const {
        return window;
    };

private:
    static int getIndex(HWND wnd);

    static int getRadioIndex(HWND wnd);

    bool checkIndex(int index) const;

    int getYOffset() const;

    static int getInputHeight();

    void createLabel(const std::string_view &settingsName, std::string_view descriptionTitle, std::string_view descriptionDetail, int nw);

    template<typename T>
    void registerActions(const T &defaultValue,
                         std::optional<std::function<std::string(const T &)> > unparser,
                         std::optional<std::function<T(std::string &)> > parser,
                         std::optional<std::function<bool(const T &)> > validCondition,
                         std::optional<std::function<void(const T &)> > callback,
                         std::optional<std::vector<T> > values) {
        parsers.emplace_back(parser == std::nullopt
                                 ? nullptr
                                 : std::make_unique<std::function<std::any(std::string &)> >(
                                     [parser](std::string &value) {
                                         auto f = *parser;
                                         std::any result = f(value);
                                         return result;
                                     }));


        unparsers.emplace_back(unparser == std::nullopt
                                   ? nullptr
                                   : std::make_unique<std::function<std::string(const std::any &)> >(
                                       [unparser](const std::any &value) {
                                           auto f = *unparser;
                                           return f(std::any_cast<T>(value));
                                       }
                                   )
        );
        validConditions.emplace_back(validCondition == std::nullopt
                                         ? nullptr
                                         : std::make_unique<std::function<bool(const std::any &)> >(
                                             [validCondition](const std::any &value) {
                                                 auto f = *validCondition;
                                                 return f(std::any_cast<T>(value));
                                             }
                                         )
        );

        callbacks.emplace_back(callback == std::nullopt
                                   ? nullptr
                                   : std::make_unique<std::function<void(const std::any &)> >(
                                       [callback](const std::any &value) {
                                           auto f = *callback;
                                           f(std::any_cast<T>(value));
                                       }
                                   )
        );
        previousInputs.emplace_back([&unparser, &defaultValue] {
            auto f = *unparser;
            return f(defaultValue);
        }());

        valuesSel.emplace_back(values == std::nullopt
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
        adjustWindowHeight();
    };

    static LRESULT CALLBACK settingsProc(HWND window, UINT message, WPARAM wParam,
                                         LPARAM lParam);

    static LRESULT CALLBACK textFieldProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam,
                                          UINT_PTR uIdSubclass,
                                          DWORD_PTR dwRefData);

    int getFixedNameWidth() const;

    int getFixedValueWidth() const;


    void adjustWindowHeight() const;
    static void errorMessage(const HWND window) {
        MessageBox(window, "Invalid value!!", "Error", MB_OK | MB_ICONERROR);
    }
};
