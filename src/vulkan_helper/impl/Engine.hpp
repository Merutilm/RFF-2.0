//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include "../core/vkh_base.hpp"

#include "Core.hpp"
#include "../context/WindowContext.hpp"

namespace merutilm::vkh {
    class EngineImpl final : public Handler {
        Core core = nullptr;
        Repositories globalRepositories = nullptr;
        std::vector<WindowContext> windowContexts = {};

    public:
        explicit EngineImpl(Core &&core);

        ~EngineImpl() override;

        bool isValidWindowContext(uint32_t windowAttachmentIndex) const;

        EngineImpl(const EngineImpl &) = delete;

        EngineImpl &operator=(const EngineImpl &) = delete;

        EngineImpl(EngineImpl &&) = delete;

        EngineImpl &operator=(EngineImpl &&) = delete;

        [[nodiscard]] WindowContextPtr attachWindowContext(HWND hwnd, uint32_t windowAttachmentIndexExpected);

        void detachWindowContext(uint32_t windowAttachmentIndex);

        [[nodiscard]] CoreRef getCore() const { return *core; }

        [[nodiscard]] WindowContextRef getWindowContext(const uint32_t windowContextIndex) const {
            return *windowContexts.at(windowContextIndex);
        }

        [[nodiscard]] RepositoriesRef getGlobalRepositories() const {
            return *globalRepositories;
        }

    private:
        void init() override;

        void configureRepositories() const;

        void destroy() override;
    };


    using Engine = std::unique_ptr<EngineImpl>;
    using EnginePtr = EngineImpl *;
    using EngineRef = EngineImpl &;
}
