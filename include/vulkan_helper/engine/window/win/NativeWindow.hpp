//
// Created by Merutilm on 2026-02-06.
//

#pragma once
#ifdef _WIN32
#include <Windows.h>
#include <vulkan_helper/engine/window/PlatformWindowBase.hpp>

namespace merutilm::vkh {

    class NativeWindow final : public PlatformWindowBase {
        HWND mainWindow = nullptr;
        HWND renderWindow = nullptr;
        static constexpr auto RENDER_CLASS_NAME = L"RenderWindow";
        static constexpr auto MAIN_CLASS_NAME = L"MainWindow";

        std::unordered_map<UINT, PlatformMenuBase *> nativeMenuMap;

    public:
        explicit NativeWindow(WindowInitializerSettings &&settings);

        ~NativeWindow() override;

        NativeWindow(const NativeWindow &) = delete;
        NativeWindow &operator=(const NativeWindow &) = delete;
        NativeWindow(NativeWindow &&) = delete;
        NativeWindow &operator=(NativeWindow &&) noexcept = delete;

        [[nodiscard]] HWND getMainWindow() const { return mainWindow;}
        [[nodiscard]] HWND getRenderWindow() const { return renderWindow;}


    private:

        static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK RenderWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        void initWindow();

        void initMenu();


        void resolveWindowResizeEnd() const;

        void prepareWindow() const;

        void createMainWindow();

        void createRenderWindow();

        static std::string ws2s(std::wstring_view str);

        static std::wstring s2ws(std::string_view str);

        void setRenderWindowSizeWithClientAdjustment(int width, int height) const override;

        void setRenderWindowSize(uint32_t width, uint32_t height) const override;

        VkExtent2D calculateClientSizeFromMainWindow() const override;

        bool canRenderable() override;

        void getRenderWindowExtent(uint16_t *x, uint16_t *y) const override;

        void getMousePosition(int *x, int *y) override;

        void start() override;

        void init() override;

        void cleanup() override;

        void generateKeyMapper() override;

        VkSurfaceKHR createSurface(VkInstance instance) override;

        PlatformMenuBase &addMenu(PlatformMenuBase &menu, std::wstring name) override;
        PlatformMenuItemBase &addMenuItem(PlatformMenuBase &menu, std::wstring name) override;
        PlatformCheckboxMenuItemBase &addCheckboxMenuItem(PlatformMenuBase &menu, std::wstring name) override;
    };
} // namespace merutilm::vkh
#endif
