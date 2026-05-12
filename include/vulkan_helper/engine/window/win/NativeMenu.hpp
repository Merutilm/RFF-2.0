//
// Created by Merutilm on 2026-05-10.
//

#pragma once
#ifdef _WIN32
#include <Windows.h>
#include "../PlatformMenuBase.hpp"
namespace merutilm::vkh {
    class NativeMenu final : public PlatformMenuBase{
        HMENU menu;

    public:

        explicit NativeMenu(std::wstring name, const PlatformMenuBase *parent);
        ~NativeMenu() override;
        NativeMenu(const NativeMenu&) = delete;
        NativeMenu& operator=(const NativeMenu&) = delete;
        NativeMenu(NativeMenu&&) = delete;
        NativeMenu& operator=(NativeMenu&&) = delete;

        HMENU getHandle() const {return menu;}

    private:
        void init() override;
        void cleanup() override;
    };
}
#endif