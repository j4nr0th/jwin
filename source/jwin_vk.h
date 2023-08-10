//
// Created by jan on 9.8.2023.
//

#ifndef JWIN_JWIN_VK_H
#define JWIN_JWIN_VK_H

#include "window.h"
#include <vulkan/vulkan.h>
#include "error.h"

jwin_result
jwin_window_create_window_vk_surface(
        VkInstance instance, jwin_window* win, const VkAllocationCallbacks* alloc_callbacks, const void* next,
        VkResult* p_result, VkSurfaceKHR* p_out);

JWIN_API void jwin_required_vk_extensions(unsigned* p_count, const char* const** p_names);

#endif //JWIN_JWIN_VK_H
