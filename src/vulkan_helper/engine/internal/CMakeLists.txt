add_library(engine_internal OBJECT
        RCCImGui.cpp
        RendererImGui.cpp
)

target_link_libraries(engine_internal PRIVATE
        vkh_dependency
)