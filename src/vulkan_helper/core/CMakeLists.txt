add_library(core OBJECT
        Core.cpp
        Instance.cpp
        LogicalDevice.cpp
        PhysicalDeviceLoader.cpp
        ValidationLayer.cpp
)

target_link_libraries(core PRIVATE
        vkh_dependency
)