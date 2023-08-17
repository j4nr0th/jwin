//
// Created by jan on 7.8.2023.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "window_win32.h"

#include "context_win32.h"
#include "../window.h"
#include "../events.h"



jwin_result jwin_window_create(jwin_context* ctx, const jwin_window_create_info* create_info, jwin_window** p_out)
{
    jwin_window* const this = ctx->allocator_callbacks.alloc(ctx->allocator_callbacks.state, sizeof(*this));
    if (!this)
    {
        REPORT_ERROR(ctx, "Could not allocate memory for the window");
        return JWIN_RESULT_BAD_ALLOC;
    }

    jwin_window_create_info info = *create_info;
    create_info = NULL;
    if (!info.title)
    {
        info.title = "jwin-program";
    }

    RECT size = { .bottom = info.height, .right = info.width };
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
    int x = info.x != JWIN_POSITION_DONT_CARE ? info.x : 0;
    int y = info.y != JWIN_POSITION_DONT_CARE ? info.y : 0;
    const HWND hwnd = CreateWindow(
        ctx->window_class.lpszClassName,
        info.title,
        WS_OVERLAPPEDWINDOW,
        x, y,
        size.right, size.bottom,
        NULL,
        NULL,
        ctx->hinstance,
        (void*)this
    );
    if (!hwnd)
    {
        REPORT_ERROR(ctx, "Could not create Win32 window");
        return JWIN_RESULT_NO_WINDOW;
    }


    //  Recheck window geometry

    this->ctx = ctx;
    memset(this->event_handlers, 0, sizeof(this->event_handlers));

    this->hwnd = hwnd;

    this->is_minimized = 1;

    this->event_hook = NULL;
    this->event_param = NULL;

    this->should_close = 0;

    if (info.visible)
    {
        jwin_window_show(this);
    }
    jwin_result res = INTERNAL_add_window_to_context(ctx, this);
    if (res != JWIN_RESULT_SUCCESS)
    {
        REPORT_ERROR(ctx, "Could not add window to context");
        DestroyWindow(hwnd);
        ctx->allocator_callbacks.free(ctx->allocator_callbacks.state, this);
        return res;
    }

    *p_out = this;
    return JWIN_RESULT_SUCCESS;
}

jwin_result jwin_window_destroy(jwin_window* win)
{
    jwin_context* ctx = win->ctx;
    void
    (*const dtor)(const jwin_event_destroy*, void*) = win->event_handlers[JWIN_EVENT_TYPE_DESTROY].callback.destroy;
    const jwin_event_destroy event =
            {
                    .type = JWIN_EVENT_TYPE_DESTROY,
                    .window = win,
                    .context = ctx,
            };
    CALL_EVENT_HOOKS(win, (jwin_event_any) { .destroy = event });
    if (dtor)
    {
        dtor(&event, win->event_handlers[JWIN_EVENT_TYPE_DESTROY].param);
    }
    const jwin_allocator_callbacks* allocator_callbacks = &ctx->allocator_callbacks;
    
    const jwin_result res = INTERNAL_remove_window_from_context(ctx, win);
    if (res != JWIN_RESULT_SUCCESS)
    {
        REPORT_ERROR(ctx, "Could not remove the window from the context");
        return res;
    }

    
    DestroyWindow(win->hwnd);
    allocator_callbacks->free(allocator_callbacks->state, win);
    return res;
}

jwin_result
jwin_window_set_event_handler(jwin_window* win, jwin_event_type type, jwin_event_callback callback, void* param)
{
    if (type <= JWIN_EVENT_TYPE_NONE || type >= JWIN_EVENT_TYPE_COUNT)
    {
        REPORT_ERROR(win->ctx, "Event type was given as %d, should be [%d, %d]", type, JWIN_EVENT_TYPE_NONE + 1,
                     JWIN_EVENT_TYPE_COUNT - 1);
        return JWIN_RESULT_BAD_EVENT_TYPE;
    }
    win->event_handlers[type].type = type;
    win->event_handlers[type].callback = callback;
    win->event_handlers[type].param = param;

    return JWIN_RESULT_SUCCESS;
}

jwin_result jwin_window_get_event_handler(jwin_window* win, jwin_event_type type, jwin_event_handler* p_out)
{
    if (type <= JWIN_EVENT_TYPE_NONE || type >= JWIN_EVENT_TYPE_COUNT)
    {
        REPORT_ERROR(win->ctx, "Event type was given as %d, should be [%d, %d]", type, JWIN_EVENT_TYPE_NONE + 1,
                     JWIN_EVENT_TYPE_COUNT - 1);
        return JWIN_RESULT_BAD_EVENT_TYPE;
    }
    *p_out = win->event_handlers[type];

    return JWIN_RESULT_SUCCESS;
}

jwin_result jwin_window_send_custom_event(jwin_window* win, const jwin_event_custom* event)
{
    if (event->base.type < JWIN_EVENT_TYPE_CUSTOM)
    {
        REPORT_ERROR(win->ctx, "Event type was not custom + i (%d), but was %d", JWIN_EVENT_TYPE_CUSTOM, event->base.type);
        return JWIN_RESULT_BAD_EVENT_TYPE;
    }
    const jwin_context* const ctx = win->ctx;
    void* const copy = ctx->allocator_callbacks.alloc(ctx->allocator_callbacks.state, sizeof(*event));
    if (!copy)
    {
        REPORT_ERROR(ctx, "Could not allocate %zu bytes of memory for sending the custom event", sizeof(*event));
        return JWIN_RESULT_BAD_ALLOC;
    }
    memcpy(copy, event, sizeof(*event));
    SendMessage(win->hwnd, WM_USER, (WPARAM)copy, 0);

    return JWIN_RESULT_SUCCESS;
}

jwin_result jwin_window_close(jwin_window* win)
{
    int destroy = 1;
    int
    (* close_handler)(const jwin_event_close*, void*) = win->event_handlers[JWIN_EVENT_TYPE_CLOSE].callback.close;
    if (close_handler)
    {
        const jwin_event_close event =
                {
                        .type = JWIN_EVENT_TYPE_CLOSE,
                        .context = win->ctx,
                        .window = win,
                };
        destroy = close_handler(&event, win->event_handlers[JWIN_EVENT_TYPE_CLOSE].param);
    }

    if (destroy)
    {
        return jwin_window_destroy(win);
    }

    return JWIN_RESULT_SUCCESS;
}

jwin_result jwin_window_set_title(jwin_window* win, const char* new_title)
{
    SetWindowText(win->hwnd, new_title);
    return JWIN_RESULT_SUCCESS;
}

void jwin_window_show(jwin_window* win)
{
    win->is_minimized = 0;
    ShowWindow(win->hwnd, SW_SHOWDEFAULT);
}

void jwin_window_hide(jwin_window* win)
{
    win->is_minimized = 1;
    ShowWindow(win->hwnd, SW_HIDE);
}

jwin_result jwin_window_ask_to_close(jwin_window* win)
{
    win->should_close = 1;
    return JWIN_RESULT_SUCCESS;
}

void jwin_window_get_size(jwin_window* win, unsigned* p_width, unsigned* p_height)
{
    *p_width = win->width;
    *p_height = win->height;
}

void jwin_window_get_position(jwin_window* win, int* p_x, int* p_y)
{
    *p_x = win->x;
    *p_y = win->y;
}

void jwin_window_set_event_hook(jwin_window* win, void(* hook)(const jwin_event_any* e, void* param), void* param)
{
    win->event_hook = hook;
    win->event_param = param;
}

HWND jwin_contex_native_window_win32(jwin_window* win)
{
    return win->hwnd;
}

