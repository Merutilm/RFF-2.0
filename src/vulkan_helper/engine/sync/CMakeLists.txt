add_library(engine_sync OBJECT
        Fence.cpp
        Semaphore.cpp
        SyncObject.cpp
)

target_link_libraries(engine_sync PRIVATE
        vkh_dependency
)