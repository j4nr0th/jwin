//
// Created by jan on 7.8.2023.
//

#ifndef JWIN_WINDOW_XLIB_H
#define JWIN_WINDOW_XLIB_H

#include "common_win32.h"
#include "../window.h"

struct jwin_window_T
{
    jwin_context* ctx;
    jwin_event_handler event_handlers[JWIN_EVENT_TYPE_COUNT];

    HWND hwnd;
    
    unsigned width, height;
    int x, y;

    BOOL is_minimized;
    unsigned long double_click_time;

    void (* event_hook)(const jwin_event_any*, void*);

    void* event_param;
    int should_close;
};

static inline void INTERNAL_window_event_hook(const jwin_window* win, const jwin_event_any e)
{
    if (win->event_hook)
    {
        win->event_hook(&e, win->event_param);
    }
}


#endif //JWIN_WINDOW_XLIB_H
