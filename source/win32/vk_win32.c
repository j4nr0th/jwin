//
// Created by jan on 9.8.2023.
//
#include "../jwin_vk.h"
#include "window_win32.h"
#include "context_win32.h"
#include <vulkan/vulkan_win32.h>

jwin_result
jwin_window_create_window_vk_surface(
        VkInstance instance, jwin_window* win, const VkAllocationCallbacks* alloc_callbacks, const void* next,
        VkResult* p_result, VkSurfaceKHR* p_out)
{
    VkWin32SurfaceCreateInfoKHR create_info =
            {
                    .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                    .hwnd = win->hwnd,
                    .flags = 0,
                    .hinstance = win->ctx->hinstance,
                    .pNext = next,
            };
    VkSurfaceKHR surface;
    const VkResult result = vkCreateWin32SurfaceKHR(instance, &create_info, alloc_callbacks, &surface);

    if (p_result)
    {
        *p_result = result;
    }
    if (result == VK_SUCCESS)
    {
        *p_out = surface;
        return JWIN_RESULT_SUCCESS;
    }

    return JWIN_RESULT_VK_FAILED;
}

static const char* const VK_EXTENSION_NAMES[] =
        {
                "VK_KHR_surface",
                "VK_KHR_win32_surface",
        };

static const unsigned VK_EXTENSION_COUNT = sizeof(VK_EXTENSION_NAMES) / sizeof(*VK_EXTENSION_NAMES);

void jwin_required_vk_extensions(unsigned* p_count, const char* const** p_names)
{
    *p_count = VK_EXTENSION_COUNT;
    *p_names = VK_EXTENSION_NAMES;
}
