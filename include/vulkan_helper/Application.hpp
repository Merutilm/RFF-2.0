//
// Created by Merutilm on 2026-02-06.
//

#pragma once


#include "engine/graphics/Renderer.hpp"
#include "handle/EngineHandler.hpp"
namespace merutilm::vkh {


    struct Application : Handler {

        std::optional<Engine> engine = std::nullopt;
        WindowContext *rootWindowContext = nullptr;
        WindowInitializerSettings rootWindowInitializerSettings;
        std::vector<std::unique_ptr<Renderer>> renderers;


        static constexpr int WC_ROOT = 0;

        explicit Application(WindowInitializerSettings rootWindowInitializerSettings);

        ~Application() override;

        Application(const Application &) = delete;

        Application(Application &&) = delete;

        Application &operator=(const Application &) = delete;

        Application &operator=(Application &&) = delete;


        template<typename App> requires std::is_base_of_v<Application, App>
        static void start(const WindowInitializerSettings &wic) {
            VkExtent2D extent = {static_cast<uint32_t>(wic.widthInfo.first), static_cast<uint32_t>(wic.heightInfo.first)};
            App app(wic);
            app.refreshSharedImgContexts(extent);
            app.registerRenderers();
            app.callRenderContextRefreshed();
            app.addListeners();
            app.start();
        }


        template<typename Rdr, typename... Args>
            requires std::is_constructible_v<Rdr, Args...>
        Rdr *registerRenderer(Args &&...args) {
            return dynamic_cast<Rdr *>(renderers.emplace_back(std::make_unique<Rdr>(std::forward<Args>(args)...)).get());
        }

        virtual void refreshSharedImgContexts(VkExtent2D extent) = 0;

        virtual void registerRenderers() = 0;


        virtual void addListeners();

        virtual void update() = 0;

        void start() const {

            rootWindowContext->getWindow()->start();
        }


        virtual void onStart() = 0;

        virtual void onResize(VkExtent2D newExtent);

        virtual void onQuit() = 0;

    protected:


        void init() override;

        void cleanup() override;

#ifdef VKH_USE_IMGUI
        virtual void renderImGui() = 0;

        void createImGuiContext(RenderContext *rc);
#endif
    private:

        void callRenderContextRefreshed() const {
            for (const auto &renderer: renderers) {
                for (const auto &pc: renderer->configurators) {
                    pc->renderContextRefreshed();
                }
            }
        }

        void configureRootWindowContext();

#ifdef VKH_USE_IMGUI
        static void imguiVkResultConsumer(VkResult result);
#endif

    };


} // namespace merutilm::vkh
