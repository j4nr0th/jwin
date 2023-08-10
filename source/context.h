//
// Created by jan on 6.8.2023.
//

#ifndef JWIN_CONTEXT_H
#define JWIN_CONTEXT_H
#include "common.h"

struct jwin_context_create_info_T
{
    const jwin_allocator_callbacks* allocator_callbacks;
    const jwin_error_callbacks* error_callbacks;
};

typedef struct jwin_context_create_info_T jwin_context_create_info;

jwin_result jwin_context_create(const jwin_context_create_info* create_info, jwin_context** p_out);

void jwin_context_destroy(jwin_context* ctx);

void jwin_context_set_user_ptr(jwin_context* ctx, void* ptr);

void* jwin_context_get_user_ptr(jwin_context* ctx);

void jwin_context_mark_to_close(jwin_context* ctx);

unsigned jwin_context_window_count(jwin_context* ctx);

jwin_result jwin_context_wait_for_events(jwin_context* ctx);

jwin_result jwin_context_wait_for_events_timeout(jwin_context* ctx, int timeout_ms);

jwin_result jwin_context_handle_event(jwin_context* ctx);

jwin_result jwin_context_handle_events(jwin_context* ctx);

int jwin_context_should_close(jwin_context* ctx);


#ifndef _WIN32
#include <X11/Xlib-xcb.h>
Display* jwin_context_native_xlib(jwin_context* ctx);

xcb_connection_t* jwin_contex_native_xcb(jwin_context* ctx);
#else
#error not implemented yet
#endif

#endif //JWIN_CONTEXT_H
