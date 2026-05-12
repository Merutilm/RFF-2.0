//
// Created by Merutilm on 2026-02-07.
//

#ifdef _WIN32
#include <vulkan_helper/base/logger.hpp>
#include <vulkan_helper/engine/window/win/NativeWindow.hpp>

#include "vulkan_helper/engine/window/win/NativeCheckboxMenuItem.hpp"
#include "vulkan_helper/engine/window/win/NativeMenu.hpp"
#include "vulkan_helper/engine/window/win/NativeMenuItem.hpp"

namespace merutilm::vkh {
    NativeWindow::NativeWindow(WindowInitializerSettings &&settings) : PlatformWindowBase(std::move(settings)) {
        NativeWindow::init();
    }

    NativeWindow::~NativeWindow() { NativeWindow::cleanup(); }


    void NativeWindow::initWindow() {
        initMenu();
        generateKeyMapper();
        createMainWindow();
        createRenderWindow();
        setRenderWindowSizeWithClientAdjustment(initializerSettings.widthInfo.first,
                                                initializerSettings.heightInfo.first);
        prepareWindow();
    }
    LRESULT NativeWindow::MainWindowProc(const HWND hWnd, const UINT message, const WPARAM wParam,
                                         const LPARAM lParam) {

        const auto windowPtr = reinterpret_cast<NativeWindow *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (windowPtr == nullptr) {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        auto &window = *windowPtr;

        switch (message) {
            case WM_INITMENUPOPUP: {
                const auto popup = reinterpret_cast<HMENU>(wParam);
                const int count = GetMenuItemCount(popup);
                for (int i = 0; i < count; ++i) {
                    // synchronize current settings
                    MENUITEMINFO info = {};
                    info.cbSize = sizeof(MENUITEMINFO);
                    info.fMask = MIIM_ID;

                    if (GetMenuItemInfo(popup, i, TRUE, &info)) {
                        const UINT id = info.wID;
                        const auto nm = window.nativeMenuMap.find(id);
                        if (nm != window.nativeMenuMap.end()) {
                            if (const auto item = dynamic_cast<NativeCheckboxMenuItem *>(nm->second)) {
                                item->refreshChecked();
                                CheckMenuItem(popup, id, MF_BYCOMMAND | (item->checked ? MF_CHECKED : MF_UNCHECKED));
                            }
                        }
                    }
                }
                return 0;
            }
            case WM_COMMAND: {
                const HMENU menu = GetMenu(window.getMainWindow());
                const int menuID = LOWORD(wParam);
                auto nm = window.nativeMenuMap.at(menuID);
                if (const auto checkbox = dynamic_cast<NativeCheckboxMenuItem *>(nm)) {
                    checkbox->checked = !checkbox->checked;
                    checkbox->onClick.invoke(checkbox->checked);
                    CheckMenuItem(menu, menuID, checkbox->checked ? MF_CHECKED : MF_UNCHECKED);
                } else if (const auto item = dynamic_cast<NativeMenuItem *>(nm)) {
                    item->onClick.invoke();
                }
                return 0;
            }
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN: {
                if (lParam & 1 << 30) // is not first input
                    return 0;


                if (wParam == VK_CONTROL)
                    window.controlKeyPressed = true;
                if (wParam == VK_MENU)
                    window.altKeyPressed = true;
                if (wParam == VK_SHIFT)
                    window.shiftKeyPressed = true;

                const auto key = window.findEquivalentKey(wParam);
                window.pressedKeys.emplace(key);
                window.eventSystem.keyboard.onKeyDown.invoke(key);
                return 0;
            }

            case WM_KEYUP:
            case WM_SYSKEYUP: {
                if (wParam == VK_CONTROL)
                    window.controlKeyPressed = false;
                if (wParam == VK_MENU)
                    window.altKeyPressed = false;
                if (wParam == VK_SHIFT)
                    window.shiftKeyPressed = false;
                const auto key = window.findEquivalentKey(wParam);
                window.pressedKeys.erase(key);
                window.eventSystem.keyboard.onKeyUp.invoke(key);
                return 0;
            }
            case WM_KILLFOCUS: {
                window.controlKeyPressed = false;
                window.altKeyPressed = false;
                window.shiftKeyPressed = false;
                window.mouseHovered = false;
                window.leftMouse.reset();
                window.middleMouse.reset();
                window.rightMouse.reset();
                window.pressedKeys.clear();
                window.eventSystem.focus.onFocusLost.invoke();
                return 0;
            }
            case WM_SETFOCUS: {
                window.eventSystem.focus.onFocusGained.invoke();
                return 0;
            }
            case WM_MOUSEWHEEL: {
                window.eventSystem.mouseWheel.onMouseScroll.invoke(GET_WHEEL_DELTA_WPARAM(wParam) /
                                                                   static_cast<float>(WHEEL_DELTA));
                return 0;
            }
            case WM_DESTROY: {
                window.eventSystem.applicationLifecycle.onQuit.invoke();
                PostQuitMessage(0);
                return 0;
            }
            case WM_SIZE: {
                const auto [width, height] = window.calculateClientSizeFromMainWindow();

                if (!window.isResizing) {
                    window.isResizing = true;
                    window.eventSystem.resize.onResizeBegin.invoke();
                }

                if (wParam == SIZE_MAXIMIZED || wParam == SIZE_MINIMIZED || wParam == SIZE_RESTORED) {
                    window.setRenderWindowSize(width, height);
                    window.isResizing = false;
                    window.eventSystem.resize.onResizeEnd.invoke(width, height);

                    if (wParam == SIZE_MINIMIZED) {
                        window.eventSystem.window.onMinimize.invoke();
                    } else if (wParam == SIZE_MAXIMIZED) {
                        window.eventSystem.window.onMaximize.invoke();
                    } else {
                        window.eventSystem.window.onRestore.invoke();
                    }
                }else {
                    window.eventSystem.resize.onResizing.invoke(width, height);
                }


                return 0;
            }

            case WM_EXITSIZEMOVE: {
                if (window.isResizing) {
                    const auto [width, height] = window.calculateClientSizeFromMainWindow();
                    window.isResizing = false;
                    window.setRenderWindowSize(width, height);
                    window.eventSystem.resize.onResizeEnd.invoke(width, height);
                }

                return 0;
            }
            case WM_DROPFILES: {
                const auto hDrop = reinterpret_cast<HDROP>(wParam);
                const uint32_t fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
                std::vector<std::string> files;
                for (uint32_t i = 0; i < fileCount; ++i) {
                    std::wstring filePath(MAX_PATH, 0);
                    DragQueryFile(hDrop, i, filePath.data(), MAX_PATH);
                    files.emplace_back(ws2s(filePath));
                }

                window.eventSystem.dnd.onFileDrop.invoke(files);
                DragFinish(hDrop);
                return 0;
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

    LRESULT NativeWindow::RenderWindowProc(const HWND hWnd, const UINT message, const WPARAM wParam,
                                           const LPARAM lParam) {
        const auto windowPtr = reinterpret_cast<NativeWindow *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (windowPtr == nullptr) {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        auto &window = *windowPtr;
        static int lastX;
        static int lastY;

        auto handleMouseUp = [&](const KeyInput::MouseButton btn, MouseState &mouse, int x, int y) {
            if (mouse.isDragging)
                window.eventSystem.mouseDrag.onMouseDragEnd.invoke(btn, x, y);
            window.eventSystem.mouse.onMouseUp.invoke(btn, x, y);
            mouse.reset();
        };

        switch (message) {
            case WM_GETMINMAXINFO: {
                const auto info = reinterpret_cast<LPMINMAXINFO>(lParam);
                info->ptMinTrackSize.x = window.initializerSettings.widthInfo.min;
                info->ptMinTrackSize.y = window.initializerSettings.heightInfo.min;
                info->ptMaxTrackSize.x = window.initializerSettings.widthInfo.max;
                info->ptMaxTrackSize.y = window.initializerSettings.heightInfo.max;
                return 0;
            }
            case WM_MOUSELEAVE: {

                window.eventSystem.mouse.onMouseExit.invoke();
                window.mouseHovered = false;

                if (window.leftMouse.isPressed) {
                    handleMouseUp(KeyInput::MouseButton::LEFT, window.leftMouse, lastX, lastY);
                }
                if (window.middleMouse.isPressed) {
                    handleMouseUp(KeyInput::MouseButton::MIDDLE, window.middleMouse, lastX, lastY);
                }
                if (window.rightMouse.isPressed) {
                    handleMouseUp(KeyInput::MouseButton::RIGHT, window.rightMouse, lastX, lastY);
                }

                return 0;
            }
            case WM_LBUTTONDOWN: {
                const auto xPos = GET_X_LPARAM(lParam);
                const auto yPos = GET_Y_LPARAM(lParam);
                window.leftMouse.isPressed = true;
                window.leftMouse.pressedX = xPos;
                window.leftMouse.pressedY = yPos;
                window.eventSystem.mouse.onMouseDown.invoke(KeyInput::MouseButton::LEFT, xPos, yPos);
                return 0;
            }
            case WM_MBUTTONDOWN: {
                const auto xPos = GET_X_LPARAM(lParam);
                const auto yPos = GET_Y_LPARAM(lParam);
                window.middleMouse.isPressed = true;
                window.middleMouse.pressedX = xPos;
                window.middleMouse.pressedY = yPos;
                window.eventSystem.mouse.onMouseDown.invoke(KeyInput::MouseButton::MIDDLE, xPos, yPos);
                return 0;
            }
            case WM_RBUTTONDOWN: {
                const auto xPos = GET_X_LPARAM(lParam);
                const auto yPos = GET_Y_LPARAM(lParam);
                window.rightMouse.isPressed = true;
                window.rightMouse.pressedX = xPos;
                window.rightMouse.pressedY = yPos;
                window.eventSystem.mouse.onMouseDown.invoke(KeyInput::MouseButton::RIGHT, xPos, yPos);
                return 0;
            }
            case WM_MOUSEMOVE: {
                const auto xPos = GET_X_LPARAM(lParam);
                const auto yPos = GET_Y_LPARAM(lParam);

                if (!window.mouseHovered) {
                    TRACKMOUSEEVENT tme{};
                    tme.cbSize = sizeof(tme);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hWnd;
                    TrackMouseEvent(&tme);
                    window.eventSystem.mouse.onMouseEnter.invoke();
                    window.mouseHovered = true;
                }

                lastX = xPos;
                lastY = yPos;

                window.eventSystem.mouse.onMouseMove.invoke(xPos, yPos);

                auto &mouseDragEvent = window.eventSystem.mouseDrag;

                auto dragFunc = [xPos, yPos, &mouseDragEvent](MouseState &mouseState,
                                                              const KeyInput::MouseButton mouseButton) {
                    if (!mouseState.isPressed)
                        return;

                    const float dx = static_cast<float>(mouseState.pressedX) - xPos;
                    const float dy = static_cast<float>(mouseState.pressedY) - yPos;

                    if (!mouseState.isDragging && dx * dx + dy * dy > MOUSE_DRAG_THRESHOLD) {
                        mouseDragEvent.onMouseDragStart.invoke(mouseButton, xPos, yPos);
                        mouseState.isDragging = true;
                    }
                    if (mouseState.isDragging)
                        mouseDragEvent.onMouseDrag.invoke(mouseButton, xPos, yPos);
                };

                dragFunc(window.leftMouse, KeyInput::MouseButton::LEFT);
                dragFunc(window.middleMouse, KeyInput::MouseButton::MIDDLE);
                dragFunc(window.rightMouse, KeyInput::MouseButton::RIGHT);

                return 0;
            }

            case WM_LBUTTONUP: {
                handleMouseUp(KeyInput::MouseButton::LEFT, window.leftMouse, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                return 0;
            }
            case WM_MBUTTONUP: {
                handleMouseUp(KeyInput::MouseButton::MIDDLE, window.middleMouse, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                return 0;
            }
            case WM_RBUTTONUP: {
                handleMouseUp(KeyInput::MouseButton::RIGHT, window.rightMouse, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                return 0;
            }

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    std::string NativeWindow::ws2s(const std::wstring_view str) {
        if (str.empty())
            return {};
        const int sizeRequired =
                WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
        std::string result(sizeRequired + 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeRequired, nullptr,
                            nullptr);
        return result;
    }
    std::wstring NativeWindow::s2ws(const std::string_view str) {
        if (str.empty())
            return {};
        const int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
        std::wstring result(sizeRequired + 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeRequired);
        return result;
    }


    void NativeWindow::generateKeyMapper() {
        using namespace KeyInput;

        for (int i = VK_F1; i <= VK_F24; ++i) {
            keyMapper[i] = static_cast<Key>(static_cast<int>(KeyCategory::FUNCTION) + i - VK_F1);
        }
        for (int i = '0'; i <= '9'; ++i) {
            keyMapper[i] = static_cast<Key>(static_cast<int>(KeyCategory::NUM) + i - '0');
        }
        for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; ++i) {
            keyMapper[i] = static_cast<Key>(static_cast<int>(KeyCategory::NUMPAD) + i - VK_NUMPAD0);
        }

        keyMapper[VK_MULTIPLY] = Key::KEY_NUMPAD_MULTIPLY;
        keyMapper[VK_ADD] = Key::KEY_NUMPAD_ADD;
        keyMapper[VK_SEPARATOR] = Key::KEY_NUMPAD_SEPARATOR;
        keyMapper[VK_SUBTRACT] = Key::KEY_NUMPAD_SUBTRACT;
        keyMapper[VK_DECIMAL] = Key::KEY_NUMPAD_DECIMAL;
        keyMapper[VK_DIVIDE] = Key::KEY_NUMPAD_DIVIDE;

        keyMapper[VK_OEM_3] = Key::KEY_BACKQUOTE;
        keyMapper[VK_OEM_MINUS] = Key::KEY_MINUS_UNDERLINE;
        keyMapper[VK_OEM_PLUS] = Key::KEY_PLUS_EQUAL;
        keyMapper[VK_OEM_4] = Key::KEY_LEFTBRACKET;
        keyMapper[VK_OEM_6] = Key::KEY_RIGHTBRACKET;
        keyMapper[VK_OEM_5] = Key::KEY_BACKSLASH; // ansi backslash
        keyMapper[VK_OEM_102] = Key::KEY_BACKSLASH; // iso backslash
        keyMapper[VK_OEM_1] = Key::KEY_SEMICOLON_COLON;
        keyMapper[VK_OEM_7] = Key::KEY_QUOTE;
        keyMapper[VK_OEM_COMMA] = Key::KEY_COMMA;
        keyMapper[VK_OEM_PERIOD] = Key::KEY_PERIOD;
        keyMapper[VK_OEM_2] = Key::KEY_SLASH;

        keyMapper[VK_UP] = Key::KEY_UP;
        keyMapper[VK_DOWN] = Key::KEY_DOWN;
        keyMapper[VK_LEFT] = Key::KEY_LEFT;
        keyMapper[VK_RIGHT] = Key::KEY_RIGHT;

        keyMapper[VK_LSHIFT] = Key::KEY_LEFT_SHIFT;
        keyMapper[VK_RSHIFT] = Key::KEY_RIGHT_SHIFT;
        keyMapper[VK_LCONTROL] = Key::KEY_LEFT_CONTROL;
        keyMapper[VK_RCONTROL] = Key::KEY_RIGHT_CONTROL;
        keyMapper[VK_LMENU] = Key::KEY_LEFT_ALT;
        keyMapper[VK_RMENU] = Key::KEY_RIGHT_ALT;
        keyMapper[VK_LWIN] = Key::KEY_LEFT_SUPER;
        keyMapper[VK_RWIN] = Key::KEY_RIGHT_SUPER;

        keyMapper[VK_TAB] = Key::KEY_TAB;
        keyMapper[VK_CAPITAL] = Key::KEY_CAPS_LOCK;
        keyMapper[VK_SPACE] = Key::KEY_SPACE;
        keyMapper[VK_BACK] = Key::KEY_BACKSPACE;
        keyMapper[VK_RETURN] = Key::KEY_ENTER;
        keyMapper[VK_ESCAPE] = Key::KEY_ESCAPE;
        keyMapper[VK_INSERT] = Key::KEY_INSERT;
        keyMapper[VK_DELETE] = Key::KEY_DELETE;
        keyMapper[VK_HOME] = Key::KEY_HOME;
        keyMapper[VK_END] = Key::KEY_END;
        keyMapper[VK_PRIOR] = Key::KEY_PAGE_UP;
        keyMapper[VK_NEXT] = Key::KEY_PAGE_DOWN;
        keyMapper[VK_NUMLOCK] = Key::KEY_NUM_LOCK;


        for (int i = 'A'; i <= 'Z'; ++i) {
            keyMapper[i] = static_cast<Key>(static_cast<int>(KeyCategory::ALPHABET) + i - 'A');
        }

        keyMapper[VK_PRINT] = Key::KEY_PRINT_SCREEN;
        keyMapper[VK_SCROLL] = Key::KEY_SCROLL_LOCK;
        keyMapper[VK_PAUSE] = Key::KEY_PAUSE;
    }


    void NativeWindow::initMenu() { menubar = std::make_unique<NativeMenu>(L"", nullptr); }

    void NativeWindow::setRenderWindowSizeWithClientAdjustment(const int width, const int height) const {
        const RECT rect = {0, 0, width, height};
        RECT adjusted = rect;
        const auto &[top, left, bottom, right] = initializerSettings.paddings;
        AdjustWindowRect(&adjusted, WS_OVERLAPPEDWINDOW | WS_SYSMENU, initializerSettings.useMenubar);

        SetWindowPos(mainWindow, nullptr, 0, 0, adjusted.right - adjusted.left + left + right,
                     adjusted.bottom - adjusted.top + bottom + top, SWP_NOMOVE | SWP_NOZORDER);
        setRenderWindowSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    void NativeWindow::setRenderWindowSize(const uint32_t width, const uint32_t height) const {
        const auto &paddings = initializerSettings.paddings;
        SetWindowPos(renderWindow, nullptr, paddings.left, paddings.top, width, height, SWP_NOZORDER);
    }

    VkExtent2D NativeWindow::calculateClientSizeFromMainWindow() const {
        RECT rect;
        const auto &[top, left, bottom, right] = initializerSettings.paddings;
        GetClientRect(mainWindow, &rect);

        const uint32_t width = rect.right - rect.left - right - left;
        const uint32_t height = rect.bottom - rect.top - bottom - top;

        if (width <= 0 || height <= 0) {
            return {0, 0};
        }

        return {width, height};
    }

    void NativeWindow::resolveWindowResizeEnd() const {
        auto [width, height] = calculateClientSizeFromMainWindow();
        setRenderWindowSize(width, height);
    }

    void NativeWindow::prepareWindow() const {
        SetWindowLongPtr(mainWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        SetWindowLongPtr(renderWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }
    void NativeWindow::createMainWindow() {

        const auto &name = initializerSettings.name;
        const auto &icon = initializerSettings.icon;

        const WNDCLASSEX wcex = {
                .cbSize = sizeof(WNDCLASSEX),
                .style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
                .lpfnWndProc = MainWindowProc,
                .hInstance = GetModuleHandle(nullptr),
                .hIcon = static_cast<HICON>(
                        LoadImage(GetModuleHandle(nullptr), icon.data(), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR)),
                .hCursor = nullptr,
                .hbrBackground = nullptr,
                .lpszMenuName = nullptr,
                .lpszClassName = MAIN_CLASS_NAME,
        };
        RegisterClassEx(&wcex);

        const DWORD style = initializerSettings.resizable ? WS_OVERLAPPEDWINDOW
                                                          : WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

        const HMENU menu = menubar ? dynamic_cast<NativeMenu *>(menubar.get())->getHandle() : nullptr;
        mainWindow = CreateWindowEx(0, MAIN_CLASS_NAME, name.data(), style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                    CW_USEDEFAULT, nullptr, menu, nullptr, nullptr);


        if (!mainWindow) {
            throw exception_init("Failed to create window!\n");
        }
    }
    void NativeWindow::createRenderWindow() {


        const WNDCLASSEX wcex = {
                .cbSize = sizeof(WNDCLASSEX),
                .style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
                .lpfnWndProc = RenderWindowProc,
                .hInstance = GetModuleHandle(nullptr),
                .hIcon = nullptr,
                .hCursor = nullptr,
                .hbrBackground = nullptr,
                .lpszMenuName = nullptr,
                .lpszClassName = RENDER_CLASS_NAME,
        };
        RegisterClassEx(&wcex);

        renderWindow = CreateWindowEx(0, RENDER_CLASS_NAME, RENDER_CLASS_NAME, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                                      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, mainWindow, nullptr,
                                      nullptr, nullptr);

        if (!renderWindow) {
            throw exception_init("Failed to create render window!\n");
        }


        DragAcceptFiles(renderWindow, initializerSettings.filedrop);
    }
    bool NativeWindow::canRenderable() {
        RECT rect;
        GetClientRect(renderWindow, &rect);
        return !isResizing && IsWindow(renderWindow) && IsWindowVisible(renderWindow) && !IsIconic(renderWindow) &&
               rect.bottom - rect.top > 0 && rect.right - rect.left > 0;
    }

    void NativeWindow::getMousePosition(int *x, int *y) {
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(getRenderWindow(), &cursor);
        *x = cursor.x;
        *y = cursor.y;
    }


    VkSurfaceKHR NativeWindow::createSurface(VkInstance instance) {
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        const VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                                                               .pNext = nullptr,
                                                               .flags = 0,
                                                               .hinstance = GetModuleHandle(nullptr),
                                                               .hwnd = renderWindow};
        if (vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS) {
            throw exception_init("failed to create window surface!");
        }
        return surface;
    }

    PlatformMenuBase &NativeWindow::addMenu(PlatformMenuBase &menu, std::wstring name) {
        return PlatformWindowBase::addMenu(menu, std::move(name));
    }

    PlatformMenuItemBase &NativeWindow::addMenuItem(PlatformMenuBase &menu, std::wstring name) {
        auto &m = static_cast<NativeMenuItem &>(PlatformWindowBase::addMenuItem(menu, std::move(name)));
        nativeMenuMap[m.id] = &m;
        return m;
    }

    PlatformCheckboxMenuItemBase &NativeWindow::addCheckboxMenuItem(PlatformMenuBase &menu, std::wstring name) {
        auto &m = static_cast<NativeCheckboxMenuItem &>(PlatformWindowBase::addCheckboxMenuItem(menu, std::move(name)));
        nativeMenuMap[m.id] = &m;
        return m;
    }

    void NativeWindow::init() {
        SetProcessDPIAware();
        initWindow();
    }

    void NativeWindow::start() {
        PlatformWindowBase::start();

        DrawMenuBar(mainWindow);
        ShowWindow(mainWindow, SW_SHOW);
        UpdateWindow(mainWindow);

        MSG message;
        using namespace std::chrono;
        auto started = high_resolution_clock::now();

        while (true) {
            if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessage(&message);

                if (message.message == WM_QUIT) {
                    break;
                }
            }
            auto now = high_resolution_clock::now();
            const auto elapsed = duration_cast<milliseconds>(now - started);

            if (canRenderable() && static_cast<float>(elapsed.count()) > 1000 / initializerSettings.framerate) {
                started = now;
                eventSystem.applicationLifecycle.onUpdate.invoke();
            }
        }
    }


    void NativeWindow::cleanup() {}


} // namespace merutilm::vkh
#endif
