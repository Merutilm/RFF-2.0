add_library(engine_sync OBJECT
        Fence.cpp
        Semaphore.cpp
        SyncObjectGroup.cpp
)

target_link_libraries(engine_sync PRIVATE
        vkh_dependency
)