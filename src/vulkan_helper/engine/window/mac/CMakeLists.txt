add_library(engine_window_mac OBJECT
        NativeWindow.cpp
)

target_link_libraries(engine_window_mac PRIVATE
        vkh_dependency
)