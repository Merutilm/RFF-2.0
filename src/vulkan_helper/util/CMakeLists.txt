add_library(util OBJECT
        BarrierUtils.cpp
        BufferImageContextUtils.cpp
        BufferImageUtils.cpp
        Debugger.cpp
        DescriptorUpdater.cpp
        PhysicalDeviceUtils.cpp
        SwapchainUtils.cpp
)
target_link_libraries(util PRIVATE
        vkh_dependency
)