add_library(engine_window OBJECT
        PlatformWindowBase.cpp
        Surface.cpp
        Swapchain.cpp
        PlatformMenuBase.cpp
        PlatformMenuItemBase.cpp
        PlatformCheckboxMenuItemBase.cpp
)

add_subdirectory(win)
add_subdirectory(mac)
add_subdirectory(linux)

target_link_libraries(engine_window PRIVATE
        vkh_dependency
)