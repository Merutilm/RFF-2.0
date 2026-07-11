add_library(engine_window OBJECT
        PlatformWindow.cpp
        Surface.cpp
        Swapchain.cpp
        PlatformMenuBase.cpp
        PlatformMenuItemBase.cpp
        PlatformCheckboxMenuItemBase.cpp
)

target_link_libraries(engine_window PRIVATE
        vkh_dependency
)