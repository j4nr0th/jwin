//
// Created by jan on 9.8.2023.
//
#include "../source/jwin.h"
#include "test_common.h"
#include <stdio.h>

static void error_report_callback(const char* msg, const char* file, int line, const char* function, void* state)
{
    fprintf(stderr, "%s:%d - %s: \"%s\"\n", file, line, function, msg);
}

static void destroy_callback(const jwin_event_destroy* e, void* param)
{
    printf("Destroying the window, message was: %s\n", (const char*)param);
    jwin_context_mark_to_close(e->context);
}
int main()
{
    jwin_context* ctx;
    jwin_error_callbacks error_callbacks =
            {
                    .state = NULL,
                    .report = error_report_callback,
            };
    jwin_context_create_info ctx_info =
            {
                    .allocator_callbacks = NULL,
                    .error_callbacks = &error_callbacks,
            };
    jwin_result res;
    JWIN_TEST_CALL(jwin_context_create(&ctx_info, &ctx));
    ASSERT(res == JWIN_RESULT_SUCCESS);

    jwin_window* wnd;
    jwin_window_create_info win_info =
            {
                    .title = "Cool window",
                    .visible = 1,
                    .width = 720,
                    .height = 480,
                    .fixed_size = 0,
                    .double_click_time_ms = 1000,
            };
    JWIN_TEST_CALL(jwin_window_create(ctx, &win_info, &wnd));
    ASSERT(res == JWIN_RESULT_SUCCESS);
    JWIN_TEST_CALL(jwin_window_set_event_handler(
            wnd, JWIN_EVENT_TYPE_DESTROY,
            (jwin_event_callback) { .destroy = destroy_callback }, "Your mum gay"));
    ASSERT(res == JWIN_RESULT_SUCCESS);

    VkInstance instance;
    unsigned jwin_extension_count;
    const char* const* jwin_extension_names;
    jwin_required_vk_extensions(&jwin_extension_count, &jwin_extension_names);
    VkInstanceCreateInfo create_info =
            {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .enabledExtensionCount = jwin_extension_count,
            .ppEnabledExtensionNames = jwin_extension_names,
            };
    VkResult vk_res = vkCreateInstance(&create_info, NULL, &instance);
    ASSERT(vk_res == VK_SUCCESS);

    VkSurfaceKHR surface;
    JWIN_TEST_CALL(jwin_window_create_window_vk_surface(instance, wnd, NULL, NULL, &vk_res, &surface));
    ASSERT(res == JWIN_RESULT_SUCCESS);
    ASSERT(vk_res == VK_SUCCESS);

    while (res == JWIN_RESULT_SUCCESS)
    {
        res = jwin_context_handle_events(ctx);
    }
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);

    ASSERT(res == JWIN_RESULT_SHOULD_CLOSE);

    jwin_context_destroy(ctx);

    return 0;
}
