//
// Created by jan on 6.8.2023.
//

#ifndef JWIN_CONTEXT_H
#define JWIN_CONTEXT_H

#include "common.h"
#include "error.h"

struct jwin_context_create_info_T
{
    const jwin_allocator_callbacks* allocator_callbacks;
    const jwin_error_callbacks* error_callbacks;
};

typedef struct jwin_context_create_info_T jwin_context_create_info;

JWIN_API jwin_result jwin_context_create(const jwin_context_create_info* create_info, jwin_context** p_out);

JWIN_API void jwin_context_destroy(jwin_context* ctx);

JWIN_API void jwin_context_set_user_ptr(jwin_context* ctx, void* ptr);

JWIN_API void* jwin_context_get_user_ptr(jwin_context* ctx);

JWIN_API void jwin_context_mark_to_close(jwin_context* ctx);

JWIN_API unsigned jwin_context_window_count(jwin_context* ctx);

JWIN_API jwin_result jwin_context_wait_for_events(jwin_context* ctx);

JWIN_API jwin_result jwin_context_wait_for_events_timeout(jwin_context* ctx, int timeout_ms);

JWIN_API jwin_result jwin_context_handle_event(jwin_context* ctx);

JWIN_API jwin_result jwin_context_handle_events(jwin_context* ctx);

JWIN_API int jwin_context_should_close(jwin_context* ctx);

#ifdef JWIN_NATIVE_HANDLES
#ifndef _WIN32

#include <X11/Xlib-xcb.h>

JWIN_API Display* jwin_context_native_xlib(jwin_context* ctx);

JWIN_API xcb_connection_t* jwin_contex_native_xcb(jwin_context* ctx);

JWIN_API Window jwin_contex_native_window_xlib(jwin_window* win);

JWIN_API xcb_window_t jwin_contex_native_window_xcb(jwin_window* win);
#else

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

JWIN_API HWND jwin_contex_native_window_win32(jwin_window* win);

#endif
#endif
#endif //JWIN_CONTEXT_H
