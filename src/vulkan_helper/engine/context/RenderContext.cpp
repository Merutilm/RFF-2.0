//
// Created by Merutilm on 2026-02-04.
//
#include <vulkan_helper/engine/context/RenderContext.hpp>
namespace merutilm::vkh {


    RenderContext::RenderContext(Core &core, std::function<VkExtent2D()> &&extentGetter, std::unique_ptr<RenderContextConfigurator> &&renderContextConfigurator) :
        core(core), extentGetter(std::move(extentGetter)) {
        RenderPassManager rpm{};
        const VkExtent2D extent = this->extentGetter();
        renderContextConfigurator->configure(rpm);
        this->renderContextConfigurator = std::move(renderContextConfigurator);
        this->renderPass.emplace(core, std::move(rpm));
        this->framebuffer.emplace(core, *renderPass, extent);
    }

    void RenderContext::recreate() {
        framebuffer.reset();
        renderPass.reset();
        RenderPassManager renderPassManager{};
        renderContextConfigurator->configure(renderPassManager);
        renderPass.emplace(core, std::move(renderPassManager));
        framebuffer.emplace(core, *renderPass, extentGetter());
    }

}