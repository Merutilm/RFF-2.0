//
// Created by Merutilm on 2026-02-06.
//


#include <vulkan_helper/Application.hpp>


namespace merutilm::vkh {

    Application::Application() {
        Application::init();
    }

    Application::~Application() { Application::cleanup(); }


    void Application::init() {
        engine.emplace();
    }

    void Application::cleanup() {
        sharedResource.reset();
        engine.reset();
    }

} // namespace merutilm::vkh
