//
// Created by Merutilm on 2026-02-07.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>

namespace merutilm::vkh {
    template<typename... T>
    class Listener {

        using CallbackHandle = uint64_t;
        using Callback = std::function<void(T...)>;
        CallbackHandle handle = 0;
        std::vector<std::pair<uint64_t, Callback>> callbacks = {};
        std::vector<std::pair<uint64_t, Callback>> listenerToAdd = {};
        std::unordered_set<CallbackHandle> listenerToRemove = {};

    public:
        template<typename F>
            requires std::is_invocable_v<F, T...>
        CallbackHandle add(F &&listener) {
            listenerToAdd.emplace_back(handle, std::forward<F>(listener));
            return handle++;
        }

        void remove(CallbackHandle callbackHandle) { listenerToRemove.emplace(callbackHandle); }

        void invoke(T... args) {
            std::erase_if(callbacks, [this](auto &callback) { return listenerToRemove.contains(callback.first); });
            callbacks.insert(callbacks.end(), listenerToAdd.begin(), listenerToAdd.end());
            listenerToAdd.clear();
            listenerToRemove.clear();

            for (auto &[_, func]: callbacks) {
                func(args...);
            }
        }
    };
} // namespace merutilm::vkh
