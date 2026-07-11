//
// Created by Merutilm on 2025-08-31.
//

#pragma once
#include <vulkan/vulkan.h>
#include <stb_image.h>
#include <glm/glm.hpp>

#include <cmath>
#include <numeric>
#include <string>
#include <format>
#include <cstdint>

#include <utility>
#include <memory>
#include <cstddef>
#include <algorithm>

#include <optional>
#include <functional>
#include <variant>

#include <thread>
#include <condition_variable>
#include <mutex>
#include <locale>
#include <chrono>

#include <vector>
#include <array>
#include <list>
#include <span>
#include <unordered_map>
#include <unordered_set>

#include <iostream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __linux__
#include <gtk/gtk.h>
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan_helper/base/exception.hpp>