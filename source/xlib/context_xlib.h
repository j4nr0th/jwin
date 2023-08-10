//
// Created by jan on 6.8.2023.
//

#ifndef JWIN_CONTEXT_XLIB_H
#define JWIN_CONTEXT_XLIB_H

#include "../context.h"
#include "common_xlib.h"
#include "../events.h"
#include <stdarg.h>

enum
{
    PIPE_OUTPUT = 0,    //  Read from this
    PIPE_INPUT = 1,     //  Write to this
    PIPE_COUNT
};

enum jwin_key_state_T
{
    JWIN_KEY_STATE_UP = 0,
    JWIN_KEY_STATE_DOWN = 1,
};
typedef enum jwin_key_state_T jwin_key_state;

struct jwin_context_T
{
    int should_close;
    jwin_allocator_callbacks allocator_callbacks;
    jwin_error_callbacks error_callbacks;

    Display* dpy;
    xcb_connection_t* xcb_connection;
    uint32_t window_count;
    uint32_t window_capacity;
    jwin_window** window_array;
    struct
    {
        Atom wm_delete_window;

        //  String atoms
        Atom utf8;
        Atom null;

        //  Clipboard atoms
        Atom targets;
        Atom multiple;
        Atom primary;
        Atom incr;
        Atom clipboard;

        //  Dnd atoms
        Atom dnd_aware;
        Atom dnd_enter;
        Atom dnd_position;
        Atom dnd_status;
        Atom dnd_action_copy;
        Atom dnd_drop;
        Atom dnd_finished;
        Atom dnd_selection;
        Atom dnd_type_list;
        Atom dnd_text_uri_list;

        //  WM properties
        Atom wm_protocols;
        Atom net_supported;
        Atom net_supporting_wm_check;
        Atom net_wm_icon;
        Atom net_wm_ping;
        Atom net_wm_pid;
        Atom net_wm_name;
        Atom net_wm_icon_name;
        Atom net_wm_bypass_compositor;
        Atom net_wm_window_opacity;
        Atom motif_wm_hints;
    } atoms;

    XIM input_method;

    Bool autorepeat_support;
    unsigned int group;
    jwin_keycode keymap[JWIN_KEY_LAST];
    uint8_t reverse_keymap[JWIN_KEY_LAST];
    jwin_key_state key_state[JWIN_KEY_LAST];
    Window root;
    int screen;

    int notification_pipes[PIPE_COUNT];
    void* usr_ptr;
    int xkb_event_code;

    void (* event_hook)(const jwin_event_any*, void*);

    void* event_param;
};

#ifdef __GNUC__

__attribute__((format(printf, 5, 6)))
#endif
static inline void
INTERNAL_report_error(const jwin_context* ctx, const char* file, int line, const char* function, const char* fmt, ...)
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
    char* const msg_buffer = ctx->allocator_callbacks.alloc(ctx->allocator_callbacks.state, len + 1);
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

#define REPORT_ERROR(ctx, fmt, ...) INTERNAL_report_error(ctx, __FILE__, __LINE__, __func__, fmt __VA_OPT__(,) __VA_ARGS__)

JWIN_LOCAL jwin_result INTERNAL_handle_xlib_event(jwin_context* ctx, const XEvent* e);

JWIN_LOCAL jwin_window* INTERNAL_find_window_from_xlib_handle(const jwin_context* ctx, Window wnd);

JWIN_LOCAL jwin_result INTERNAL_add_window_to_context(jwin_context* ctx, jwin_window* win);

JWIN_LOCAL jwin_result INTERNAL_remove_window_from_context(jwin_context* ctx, const jwin_window* win);

JWIN_LOCAL jwin_result INTERNAL_wait_for_xlib(const jwin_context* ctx);

JWIN_LOCAL jwin_result INTERNAL_wait_for_any(const jwin_context* ctx, int ms_timeout);

JWIN_LOCAL jwin_result INTERNAL_process_xlib_event(jwin_context* ctx, jwin_window* win, XEvent* event);

#endif //JWIN_CONTEXT_XLIB_H
