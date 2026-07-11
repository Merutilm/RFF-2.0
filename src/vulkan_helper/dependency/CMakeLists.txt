
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)


add_library(vkh_dependency OBJECT
        ${CMAKE_SOURCE_DIR}/extern/stb_image.c
)


target_include_directories(vkh_dependency PUBLIC
        # standard include
        ${CMAKE_SOURCE_DIR}/include

        # extern sources
        ${CMAKE_SOURCE_DIR}/extern

        # vulkan sdk directory
        ${Vulkan_INCLUDE_DIRS}
)

if (USE_IMGUI)
    target_sources(vkh_dependency PRIVATE
            ${CMAKE_SOURCE_DIR}/extern/imgui/imgui.cpp
            ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_draw.cpp
            ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_tables.cpp
            ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_widgets.cpp
            ${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.cpp
            ${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_vulkan.cpp
    )

    target_include_directories(vkh_dependency PUBLIC
            # imgui sources
            ${CMAKE_SOURCE_DIR}/extern/imgui
            ${CMAKE_SOURCE_DIR}/extern/imgui/backends
    )

    target_compile_definitions(vkh_dependency PUBLIC
            VKH_USE_IMGUI
    )
endif ()

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    target_include_directories(vkh_dependency PUBLIC ${GTK3_INCLUDE_DIRS})
    target_link_directories(vkh_dependency PUBLIC ${GTK3_LIBRARY_DIRS})
    target_link_libraries(vkh_dependency PUBLIC ${GTK3_LIBRARIES})
    target_compile_options(vkh_dependency PUBLIC ${GTK3_CFLAGS_OTHER})
endif()


target_link_libraries(vkh_dependency PUBLIC
        # Vulkan
        Vulkan::Vulkan
        # glfw3
        glfw
)

