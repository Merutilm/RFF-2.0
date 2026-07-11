#!/bin/bash

set -e

in_src="../VulkanHelper/src/vulkan_helper"
#in_dll="../VulkanHelper/bin/libvulkan_helper.dll"
#in_link="../VulkanHelper/lib_export/libvulkan_helper.dll.a"
in_include="../VulkanHelper/include/vulkan_helper"
in_extern="../VulkanHelper/extern"

out_src="src/vulkan_helper"
#out_dll="bin/libvulkan_helper.dll"
#out_link="lib/libvulkan_helper.dll.a"
out_include="include/vulkan_helper"
out_extern="extern"

# cp -f "$in_dll" "$out_dll"
# cp -f "$in_link" "$out_link"

rm -rf "$out_src"
rm -rf "$out_include"
rm -rf "$out_extern"

cp -r "$in_src" "$out_src"
cp -r "$in_include" "$out_include"
cp -r "$in_extern" "$out_extern"

echo "SUCCESSFULLY APPLIED NEW LIBRARY"