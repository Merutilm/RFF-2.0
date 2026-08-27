add_library(engine_sync OBJECT
        Fence.cpp
        SemaphoreGroup.cpp
        SyncObjectGroup.cpp
)

target_link_libraries(engine_sync PRIVATE
        vkh_dependency
)