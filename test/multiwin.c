//
// Created by jan on 10.8.2023.
//
#include <stdio.h>
#include "../source/jwin.h"
#include "test_common.h"

static void error_report_callback(const char* msg, const char* file, int line, const char* function, void* state)
{
    fprintf(stderr, "%s:%d - %s: \"%s\"\n", file, line, function, msg);
}

static void destroy_callback(const jwin_event_destroy* e, void* param)
{
    printf("Destroying the window, message was: %s\n", (const char*) param);
    jwin_context_mark_to_close(e->context);
}

static void context_event_hook(const jwin_event_any* e, void* param)
{
    char buffer[256];
    size_t len = jwin_event_to_str(sizeof(buffer), buffer, e);
    ASSERT(len <= sizeof(buffer));
    printf("Event str length: %zu\n%s\n\n", len, buffer);

    (void) param;
}

#define WINDOW_COUNT (16)

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

    jwin_window* wnd_array[WINDOW_COUNT];
    jwin_window_create_info win_info =
            {
                    .title = "Cool window",
                    .visible = 1,
                    .width = 720,
                    .height = 480,
                    .fixed_size = 0,
                    .double_click_time_ms = 1000,
            };
    for (unsigned i = 0; i < WINDOW_COUNT; ++i)
    {
        JWIN_TEST_CALL(jwin_window_create(ctx, &win_info, wnd_array + i));
        ASSERT(res == JWIN_RESULT_SUCCESS);
    }

    jwin_context_set_event_hook(ctx, context_event_hook, NULL);

    time_t t_begin = time(NULL);
    const time_t t_zero = t_begin;

    unsigned window_close_array[5] = { 2, 5, 6, 1, 9 };
    unsigned i = 0;

    while ((res = jwin_context_wait_for_events(ctx)) == JWIN_RESULT_SUCCESS)
    {
        printf("Processing...\n");
        res = jwin_context_handle_events(ctx);
        if (res != JWIN_RESULT_SUCCESS)
        {
            break;
        }
        const time_t t_now = time(NULL);

        if ((t_now - t_zero) >= 4)
        {
            printf("Asking context to close (politely)\n");
            jwin_context_mark_to_close(ctx);
        }
        if (t_now - t_begin >= 1)
        {
            t_begin = t_now;
            printf("Asking window %u to close (politely)\n", window_close_array[i]);
            jwin_window_ask_to_close(wnd_array[window_close_array[i++]]);
        }
    }

    ASSERT(res == JWIN_RESULT_SHOULD_CLOSE);

    jwin_context_destroy(ctx);

    return 0;
}
