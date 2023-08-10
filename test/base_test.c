//
// Created by jan on 8.8.2023.
//
#include <stdio.h>
#include "../source/jwin.h"
#include "test_common.h"

static void error_report_callback(const char* msg, const char* file, int line, const char* function, void* state)
{
    (void) state;
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

//    if (e->type == JWIN_EVENT_TYPE_KEY_PRESS && !e->key_press.repeated)
//    {
//        printf("Pressed %s\n", jwin_keycode_to_str(e->key_press.keycode));
//    }
    (void) param;
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

    jwin_context_set_event_hook(ctx, context_event_hook, NULL);

    const time_t t_begin = time(NULL);

    while ((res = jwin_context_wait_for_events(ctx)) == JWIN_RESULT_SUCCESS)
    {
        printf("Processing...\n");
        res = jwin_context_handle_events(ctx);
        if (res != JWIN_RESULT_SUCCESS)
        {
            break;
        }
        const time_t t_now = time(NULL);

        if (t_now - t_begin >= (5))
        {
            printf("Asking window to close (politely)\n");
            jwin_window_ask_to_close(wnd);
        }
    }

    ASSERT(res == JWIN_RESULT_SHOULD_CLOSE);

    jwin_context_destroy(ctx);

    return 0;
}
