//
// Created by jan on 6.8.2023.
//

#ifndef JWIN_WINDOW_H
#define JWIN_WINDOW_H
#include "common.h"
#include "events.h"

struct jwin_window_create_info_T
{
    const char* title;
    unsigned width, height;
    int x, y;
    int fixed_size;
    int visible;

#ifndef _WIN32
    struct
    {
        const char* instance_name;
        const char* class_name;
    } xorg;
#endif
    unsigned long double_click_time_ms;
};
typedef struct jwin_window_create_info_T jwin_window_create_info;

jwin_result jwin_window_create(jwin_context* ctx, const jwin_window_create_info* create_info, jwin_window** p_out);

jwin_result jwin_window_destroy(jwin_window* win);

jwin_result
jwin_window_set_event_handler(jwin_window* win, jwin_event_type type, jwin_event_callback callback, void* param);

jwin_result jwin_window_get_event_handler(jwin_window* win, jwin_event_type type, jwin_event_handler* p_out);

jwin_result jwin_window_send_custom_event(jwin_window* win, const jwin_event_custom* event);

jwin_result jwin_window_close(jwin_window* win);

jwin_result jwin_window_ask_to_close(jwin_window* win);

jwin_result jwin_window_set_title(jwin_window* win, const char* new_title);

void jwin_window_show(jwin_window* win);

void jwin_window_hide(jwin_window* win);

#endif //JWIN_WINDOW_H
