set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Options
option(USE_IMGUI "Enable Dear ImGui" ON)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

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
        $<TARGET_OBJECTS:engine_internal>
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

target_link_libraries(vulkan_helper PUBLIC
        # Dependency
        vkh_dependency
)

