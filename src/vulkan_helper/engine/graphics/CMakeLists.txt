add_library(engine_graphics OBJECT
        Framebuffer.cpp
        Renderer.cpp
        RenderPass.cpp
)

target_link_libraries(engine_graphics PRIVATE
        vkh_dependency
)