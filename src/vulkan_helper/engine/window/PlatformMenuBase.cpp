//
// Created by Merutilm on 2026-05-10.
//
#include <vulkan_helper/engine/window/PlatformMenuBase.hpp>
namespace merutilm::vkh {

    PlatformMenuBase::PlatformMenuBase(std::wstring name , const PlatformMenuBase * parent) : name(std::move(name)), parent(parent) {
        PlatformMenuBase::init();
    }
    PlatformMenuBase::~PlatformMenuBase() {
        PlatformMenuBase::cleanup();
    }
    void PlatformMenuBase::init() {

    }
    void PlatformMenuBase::cleanup() {

    }
} // namespace merutilm::vkh
