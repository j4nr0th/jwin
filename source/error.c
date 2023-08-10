//
// Created by jan on 6.8.2023.
//
#include "error.h"

static const char* const jwin_result_string_array[JWIN_RESULT_COUNT] =
        {
                [JWIN_RESULT_SUCCESS] = "Success",
                [JWIN_RESULT_SHOULD_CLOSE] = "Context is marked to close",
                [JWIN_RESULT_BAD_ALLOCATOR] = "Allocator callbacks were invalid",
                [JWIN_RESULT_BAD_ALLOC] = "Memory allocation/reallocation failed",
                [JWIN_RESULT_NO_PLATFORM] = "Could not initialize the platform interface",
                [JWIN_RESULT_BAD_CONTEXT] = "Context was not valid for the specified operation",
                [JWIN_RESULT_NO_WINDOW] = "Could not initialize the window",
                [JWIN_RESULT_BAD_EVENT_TYPE] = "Event type was invalid",
                [JWIN_RESULT_BAD_WAIT] = "Error occurred while waiting",
                [JWIN_RESULT_TIMED_OUT] = "Specified timeout time was exceeded",
                [JWIN_RESULT_VK_FAILED] = "Vulkan call failed",
        };

const char* jwin_result_msg_str(jwin_result res)
{
    if (res < JWIN_RESULT_SUCCESS || res >= JWIN_RESULT_COUNT) { return "Unknown"; }
    return jwin_result_string_array[res];
}


static const char* const jwin_result_string_names[JWIN_RESULT_COUNT] =
        {
                [JWIN_RESULT_SUCCESS] = "JWIN_RESULT_SUCCESS",
                [JWIN_RESULT_SHOULD_CLOSE] = "JWIN_RESULT_SHOULD_CLOSE",
                [JWIN_RESULT_BAD_ALLOCATOR] = "JWIN_RESULT_BAD_ALLOCATOR",
                [JWIN_RESULT_BAD_ALLOC] = "JWIN_RESULT_BAD_ALLOC",
                [JWIN_RESULT_NO_PLATFORM] = "JWIN_RESULT_NO_PLATFORM",
                [JWIN_RESULT_BAD_CONTEXT] = "JWIN_RESULT_BAD_CONTEXT",
                [JWIN_RESULT_NO_WINDOW] = "JWIN_RESULT_NO_WINDOW",
                [JWIN_RESULT_BAD_EVENT_TYPE] = "JWIN_RESULT_BAD_EVENT_TYPE",
                [JWIN_RESULT_BAD_WAIT] = "JWIN_RESULT_BAD_WAIT",
                [JWIN_RESULT_TIMED_OUT] = "JWIN_RESULT_TIMED_OUT",
                [JWIN_RESULT_VK_FAILED] = "JWIN_RESULT_VK_FAILED",
        };

const char* jwin_result_to_str(jwin_result res)
{
    if (res < JWIN_RESULT_SUCCESS || res >= JWIN_RESULT_COUNT) { return "Unknown"; }
    return jwin_result_string_names[res];
}
