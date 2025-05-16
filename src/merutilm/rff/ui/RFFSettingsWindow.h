//
// Created by Merutilm on 2025-05-13.
//

#pragma once
#include <any>
#include <functional>
#include <iostream>
#include <string>

#include "RFF.h"
#include <windows.h>
#include <commctrl.h>
#include <memory>
#include <optional>

#include "../settings/Selectable.h"

class RFFSettingsWindow {
    HWND window;
    int count = 0;
    int elements = 0;
    std::vector<std::any> references;
    std::vector<std::unique_ptr<std::function<std::any(std::string &)> > > parsers;
    std::vector<std::unique_ptr<std::function<std::string(const std::any &)> > > unparsers;
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
    explicit RFFSettingsWindow(const std::string_view &name);

    template<typename T>
    HWND registerTextInput(const std::string_view &settingsName, T *ptr,
                           std::function<std::string(const T &)> &&unparser,
                           std::function<T(std::string &)> &&parser,
                           std::function<bool(const T &)> &&validCondition,
                           std::function<void()> &&callback, const std::string_view descriptionTitle,
                           const std::string_view descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();


        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const HWND text = CreateWindowEx(0, WC_EDIT, unparser(*ptr).data(),
                                         RFF::Win32::STYLE_TEXT_FIELD, nw,
                                         getYOffset(), vw,
                                         RFF::Win32::HEIGHT_SETTINGS_INPUT, window,
                                         reinterpret_cast<HMENU>(RFF::Win32::ID_OPTIONS + count),
                                         nullptr,
                                         nullptr);

        SetWindowSubclass(text, textFieldProc, 1, 0);
        SetWindowLongPtr(text, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        SendMessage(text, WM_SETFONT, font, TRUE);
        createdChildWindows.push_back(text);
        ++elements;
        registerActions<T>(ptr, std::move(unparser), std::optional{std::move(parser)},
                           std::optional{std::move(validCondition)},
                           std::move(callback), std::nullopt);
        adjustWindowHeight();

        return text;
    }

    template<typename T> requires std::is_enum_v<T>
    HWND registerSelectionInput(const std::string_view &settingsName, T *ptr,
                                std::function<void()> &&callback, const std::string_view descriptionTitle,
                                const std::string_view descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();
        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const HWND combobox = CreateWindowEx(0, WC_COMBOBOX,
                                             "",
                                             RFF::Win32::STYLE_COMBOBOX, nw,
                                             getYOffset(), vw,
                                             RFF::Win32::HEIGHT_SETTINGS_INPUT * RFF::Win32::MAX_AMOUNT_COMBOBOX,
                                             window,
                                             reinterpret_cast<HMENU>(RFF::Win32::ID_OPTIONS + count),
                                             nullptr,
                                             nullptr);
        std::vector<T> values = Selectable::values<T>();
        int defaultValueIndex = 0;

        for (int i = 0; i < values.size(); ++i) {
            SendMessage(combobox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(Selectable::toString(values[i]).data()));
            SendMessage(combobox, WM_SETFONT, font, TRUE);
            if (values[i] == *ptr) {
                defaultValueIndex = i;
            }
        }

        SendMessage(combobox, CB_SETCURSEL, defaultValueIndex, 0);
        auto unparser = [](T v) { return Selectable::toString(v); };
        createdChildWindows.push_back(combobox);
        ++elements;
        registerActions<T>(ptr, unparser, std::nullopt, std::nullopt, std::move(callback), values);
        adjustWindowHeight();
        return combobox;
    }

    template<typename T> requires std::is_enum_v<T> || std::is_same_v<T, bool>
    std::vector<HWND> registerRadioButtonInput(const std::string_view &settingsName, T *defaultValue,
                                               std::function<void()> &&callback,
                                               const std::string_view descriptionTitle,
                                               const std::string_view descriptionDetail) {
        const int nw = getFixedNameWidth();
        const int vw = getFixedValueWidth();
        createLabel(settingsName, descriptionTitle, descriptionDetail, nw);
        const auto values = Selectable::values<T>();
        HWND defaultValueItem = nullptr;
        auto createdItem = std::vector<HWND>();
        createdItem.reserve(values.size());

        for (int i = 0; i < values.size(); ++i) {
            const HWND item = CreateWindowEx(0, WC_BUTTON,
                                             Selectable::toString(values[i]).data(),
                                             RFF::Win32::STYLE_RADIOBUTTON | (i == 0 ? WS_GROUP : 0), nw,
                                             getYOffset(), vw,
                                             RFF::Win32::HEIGHT_SETTINGS_INPUT, window,
                                             reinterpret_cast<HMENU>(
                                                 RFF::Win32::ID_OPTIONS + i *
                                                 RFF::Win32::ID_OPTIONS_RADIO + count),
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


    std::vector<HWND> registerBoolInput(const std::string_view &settingsName, bool *defaultValue,
                                        std::function<void()> &&callback, const std::string_view descriptionTitle,
                                        const std::string_view descriptionDetail) {
        return registerRadioButtonInput(settingsName, defaultValue, std::move(callback), descriptionTitle,
                                        descriptionDetail);
    }


    void setWindowCloseFunction(std::function<void()> &&function);

    static void initClass();

    HWND getWindow() const {
        return window;
    };

private:
    static int getIndex(HWND wnd);

    static int getRadioIndex(HWND wnd);

    bool checkIndex(int index) const;

    int getYOffset() const;

    void createLabel(const std::string_view &settingsName, std::string_view descriptionTitle,
                     std::string_view descriptionDetail, int nw);

    template<typename T>
    void registerActions(T *defaultValuePtr,
                         const std::function<std::string(const T &)> &unparser,
                         const std::optional<std::function<T(std::string &)> > &parser,
                         const std::optional<std::function<bool(const T &)> > &validCondition,
                         const std::function<void()> &callback,
                         const std::optional<std::vector<T> > values) {
        const std::any defaultValue = *defaultValuePtr;
        references.emplace_back(defaultValue);
        unparsers.emplace_back(std::make_unique<std::function<std::string(const std::any &)> >(
            [unparser](const std::any &value) {
                return unparser(std::any_cast<T>(value));
            }
        ));

        parsers.emplace_back(parser == std::nullopt
                                 ? nullptr
                                 : std::make_unique<std::function<std::any(std::string &)> >(
                                     [parser](std::string &value) {
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
    };

    static LRESULT CALLBACK settingsProc(HWND window, UINT message, WPARAM wParam,
                                         LPARAM lParam);

    static LRESULT CALLBACK textFieldProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam,
                                          UINT_PTR uIdSubclass,
                                          DWORD_PTR dwRefData);

    std::string currValueToString(int index) const;

    int getFixedNameWidth() const;


    int getFixedValueWidth() const;

    void adjustWindowHeight() const;

    void callError(int index);
};
