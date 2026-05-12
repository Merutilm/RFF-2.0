
add_subdirectory(dependency)
add_subdirectory(core)
add_subdirectory(engine)
add_subdirectory(util)


add_library(vulkan_helper SHARED
        Application.cpp
        $<TARGET_OBJECTS:core>
        $<TARGET_OBJECTS:util>
        $<TARGET_OBJECTS:engine>
        $<TARGET_OBJECTS:engine_buffer>
        $<TARGET_OBJECTS:engine_cmd>
        $<TARGET_OBJECTS:engine_configurator>
        $<TARGET_OBJECTS:engine_context>
        $<TARGET_OBJECTS:engine_descriptor>
        $<TARGET_OBJECTS:engine_executor>
        $<TARGET_OBJECTS:engine_graphics>
        $<TARGET_OBJECTS:engine_manage>
        $<TARGET_OBJECTS:engine_pipeline>
        $<TARGET_OBJECTS:engine_repo>
        $<TARGET_OBJECTS:engine_sampler>
        $<TARGET_OBJECTS:engine_sync>
        $<TARGET_OBJECTS:engine_window>
)


target_link_directories(vulkan_helper PRIVATE ${CMAKE_SOURCE_DIR}/lib)

target_compile_options(vulkan_helper PRIVATE
        #Debug options
        $<$<CONFIG:Debug>:-O0>
        #Release options
        $<$<CONFIG:Release>:-O3 -funroll-loops -march=native>
)

if (WIN32)
    target_sources(vulkan_helper PUBLIC $<TARGET_OBJECTS:engine_window_win>)
    target_compile_definitions(vulkan_helper PUBLIC VK_USE_PLATFORM_WIN32_KHR)
elseif (APPLE)
    target_sources(vulkan_helper PUBLIC $<TARGET_OBJECTS:engine_window_mac>)
    target_compile_definitions(vulkan_helper PUBLIC VK_USE_PLATFORM_MACOS_MVK)
elseif (UNIX)
    target_sources(vulkan_helper PUBLIC $<TARGET_OBJECTS:engine_window_linux>)
    target_compile_definitions(vulkan_helper PUBLIC VK_USE_PLATFORM_WAYLAND_KHR)
else ()
    message(STATUS "failed to find OS")
endif ()

find_package(Vulkan REQUIRED)



target_link_libraries(vulkan_helper PUBLIC
        # Vulkan
        Vulkan::Vulkan
        # Dependency
        vkh_dependency
)

