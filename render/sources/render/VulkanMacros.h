#pragma once

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#define VULKAN_HPP_STORAGE_SHARED
#define VULKAN_HPP_STORAGE_SHARED_EXPORT

#define VULKAN_HPP_NO_EXCEPTIONS
#pragma warning(push, 0)
#pragma warning(disable:4100) // unreference formal parameter
#include <vulkan/vulkan.hpp>
#pragma warning (pop)
