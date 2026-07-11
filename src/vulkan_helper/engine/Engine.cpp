//
// Created by Merutilm on 2025-07-19.
//

#include <vulkan_helper/engine/Engine.hpp>

#include <vulkan_helper/engine/repo/GlobalDescriptorSetLayoutRepo.hpp>
#include <vulkan_helper/engine/repo/GlobalPipelineLayoutRepo.hpp>
#include <vulkan_helper/engine/repo/GlobalSamplerRepo.hpp>


namespace merutilm::vkh {
    Engine::Engine() {
        Engine::init();
    }

    Engine::~Engine() {
        Engine::cleanup();
    }

    bool Engine::isValidWindowContext(const uint32_t windowAttachmentIndex) const {
        return windowContexts.size() > windowAttachmentIndex && windowContexts[windowAttachmentIndex] != nullptr;
    }

    WindowContext & Engine::attachWindowContext(WindowInitializerSettings &&wic, uint32_t windowAttachmentIndexExpected) {

        if (windowAttachmentIndexExpected >= windowContexts.size()) {
            windowContexts.resize(windowAttachmentIndexExpected + 1);
        }
        if (windowContexts[windowAttachmentIndexExpected] != nullptr) {
            throw exception_invalid_args(std::format("given window context {} is already using", windowAttachmentIndexExpected));
        }

        auto window = std::make_unique<PlatformWindow>(std::move(wic));

        windowContexts[windowAttachmentIndexExpected] = std::make_unique<WindowContext>(core, *commandPool, windowAttachmentIndexExpected, std::move(window));
        return *windowContexts[windowAttachmentIndexExpected];
    }

    std::unique_ptr<WindowContext> Engine::detachWindowContext(const uint32_t windowAttachmentIndex) {
        if (windowAttachmentIndex >= windowContexts.size()) {
            throw exception_invalid_args(std::format("window attachment index out of range : {}, size = {}", windowAttachmentIndex, windowContexts.size()));
        }
        if (windowContexts[windowAttachmentIndex] == nullptr) {
            throw exception_invalid_args(std::format("deleted non-existing context {}", windowAttachmentIndex));
        }
        auto detached = std::move(windowContexts[windowAttachmentIndex]);
        windowContexts[windowAttachmentIndex] = nullptr;
        return std::move(detached);
    }


    void Engine::init() {
        commandPool = std::make_unique<CommandPool>(core);
        sharedResource = std::make_unique<SharedResource>(core);
        configureRepositories();
    }

    void Engine::configureRepositories() {
        globalRepositories.addRepository<GlobalDescriptorSetLayoutRepo>(core);
        globalRepositories.addRepository<GlobalPipelineLayoutRepo>(core);
        globalRepositories.addRepository<GlobalShaderModuleRepo>(core);
        globalRepositories.addRepository<GlobalSamplerRepo>(core);
    }


    void Engine::cleanup() {
        windowContexts.clear();
        globalRepositories.clear();
        sharedResource.reset();
        commandPool.reset();
    }
}
