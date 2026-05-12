add_library(engine_window_win OBJECT
        NativeWindow.cpp
        NativeMenu.cpp
        NativeMenuItem.cpp
        NativeCheckboxMenuItem.cpp
)

target_link_libraries(engine_window_win PRIVATE
        vkh_dependency
)