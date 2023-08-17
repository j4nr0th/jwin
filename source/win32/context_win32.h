//
// Created by jan on 6.8.2023.
//

#ifndef JWIN_CONTEXT_XLIB_H
#define JWIN_CONTEXT_XLIB_H

#include "../context.h"
#include "common_win32.h"
#include "../events.h"
#include <stdio.h>
#include <stdarg.h>

struct jwin_context_T
{
    int should_close;
    jwin_allocator_callbacks allocator_callbacks;
    jwin_error_callbacks error_callbacks;

    HINSTANCE hinstance;
    WNDCLASS window_class;
    uint32_t window_count;
    uint32_t window_capacity;
    jwin_window** window_array;


    unsigned int group;
    jwin_keycode keymap[JWIN_KEY_LAST];
    uint8_t reverse_keymap[JWIN_KEY_LAST];

    void* usr_ptr;
    
    void (* event_hook)(const jwin_event_any*, void*);
    void* event_param;
};

static inline void
INTERNAL_report_error(const jwin_context* ctx, const char* file, int line, const char* function, _Printf_format_string_ const char* fmt, ...)
{
    if (!ctx->error_callbacks.report)
    {
        return;
    }

    va_list args, copy;
    va_start(args, fmt);
    va_copy(copy, args);
    const size_t len = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);
    char* const msg_buffer = (char*)ctx->allocator_callbacks.alloc(ctx->allocator_callbacks.state, len + 1);
    if (msg_buffer)
    {
        (void) vsnprintf(msg_buffer, len + 1, fmt, args);
        ctx->error_callbacks.report(msg_buffer, file, line, function, ctx->error_callbacks.state);
        ctx->allocator_callbacks.free(ctx->allocator_callbacks.state, msg_buffer);
    }
    va_end(args);
}

static inline void INTERNAL_context_event_hook(const jwin_context* ctx, const jwin_event_any e)
{
    if (ctx->event_hook)
    {
        ctx->event_hook(&e, ctx->event_param);
    }
}

#define CALL_EVENT_HOOKS(win, e) {const jwin_window* const hook_call_window = (win); const jwin_event_any hook_call_event = e; INTERNAL_context_event_hook(hook_call_window->ctx, hook_call_event); INTERNAL_window_event_hook(hook_call_window, hook_call_event);}(void)0

#define REPORT_ERROR(ctx, fmt, ...) INTERNAL_report_error(ctx, __FILE__, __LINE__, __func__, fmt, __VA_ARGS__)

JWIN_LOCAL LRESULT CALLBACK INTERNAL_window_proc(HWND wnd, UINT type, WPARAM wparam, LPARAM lparam);

JWIN_LOCAL jwin_result INTERNAL_add_window_to_context(jwin_context* ctx, jwin_window* win);

JWIN_LOCAL jwin_result INTERNAL_remove_window_from_context(jwin_context* ctx, const jwin_window* win);

JWIN_LOCAL jwin_result INTERNAL_wait_for_any(const jwin_context* ctx, int ms_timeout);

#endif //JWIN_CONTEXT_XLIB_H
