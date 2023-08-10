//
// Created by jan on 6.8.2023.
//

#ifndef JWIN_EVENTS_H
#define JWIN_EVENTS_H

#include <stddef.h>
#include "common.h"


enum jwin_keycode_T
{
    JWIN_KEY_NONE = 0,

    JWIN_KEY_UP = 1,
    JWIN_KEY_LEFT,
    JWIN_KEY_DOWN,
    JWIN_KEY_RIGHT,

    //  Printable keys
    JWIN_KEY_Q = 'Q',
    JWIN_KEY_W = 'W',
    JWIN_KEY_E = 'E',
    JWIN_KEY_R = 'R',
    JWIN_KEY_T = 'T',
    JWIN_KEY_Y = 'Y',
    JWIN_KEY_U = 'U',
    JWIN_KEY_I = 'I',
    JWIN_KEY_O = 'O',
    JWIN_KEY_P = 'P',
    JWIN_KEY_LBRACKET = '[',
    JWIN_KEY_RBRACKET = ']',

    JWIN_KEY_A = 'A',
    JWIN_KEY_S = 'S',
    JWIN_KEY_D = 'D',
    JWIN_KEY_F = 'F',
    JWIN_KEY_G = 'G',
    JWIN_KEY_H = 'H',
    JWIN_KEY_J = 'J',
    JWIN_KEY_K = 'K',
    JWIN_KEY_L = 'L',
    JWIN_KEY_SEMICOL = ';',
    JWIN_KEY_APPOSTROPHE = '\'',
    JWIN_KEY_BACKSLASH = '\\',

    JWIN_KEY_Z = 'Z',
    JWIN_KEY_X = 'X',
    JWIN_KEY_C = 'C',
    JWIN_KEY_V = 'V',
    JWIN_KEY_B = 'B',
    JWIN_KEY_N = 'N',
    JWIN_KEY_M = 'M',
    JWIN_KEY_COMMA = ',',
    JWIN_KEY_PERIOD = '.',
    JWIN_KEY_SLASH = '/',

    JWIN_KEY_GRAVE = '`',
    JWIN_KEY_1 = '1',
    JWIN_KEY_2 = '2',
    JWIN_KEY_3 = '3',
    JWIN_KEY_4 = '4',
    JWIN_KEY_5 = '5',
    JWIN_KEY_6 = '6',
    JWIN_KEY_7 = '7',
    JWIN_KEY_8 = '8',
    JWIN_KEY_9 = '9',
    JWIN_KEY_0 = '0',
    JWIN_KEY_MINUS = '-',
    JWIN_KEY_EQUAL = '=',

    JWIN_KEY_BACKSPACE = '\b',
    JWIN_KEY_TAB = '\t',
    JWIN_KEY_SPACE = ' ',

    //  Enums without printable equivalent
    JWIN_KEY_CAPSLOCK = 128,
    JWIN_KEY_LSHIFT,
    JWIN_KEY_RSHIFT,
    JWIN_KEY_LCTRL,
    JWIN_KEY_RCTRL,
    JWIN_KEY_MENU,
    JWIN_KEY_LSUPER,
    JWIN_KEY_RSUPER,
    JWIN_KEY_LALT, //  Alt
    JWIN_KEY_RALT, //  Alt Gr

    JWIN_KEY_RETURN,

    JWIN_KEY_INSERT,
    JWIN_KEY_HOME,
    JWIN_KEY_END,
    JWIN_KEY_DELETE,
    JWIN_KEY_PGUP,
    JWIN_KEY_PGDN,

    JWIN_KEY_PRINTSCR,
    JWIN_KEY_SCRLOCK,
    JWIN_KEY_PAUSE,

    JWIN_KEY_NUMLOCK,
    JWIN_KEY_NUM_SEP,
    JWIN_KEY_NUM_0,
    JWIN_KEY_NUM_1,
    JWIN_KEY_NUM_2,
    JWIN_KEY_NUM_3,
    JWIN_KEY_NUM_4,
    JWIN_KEY_NUM_5,
    JWIN_KEY_NUM_6,
    JWIN_KEY_NUM_7,
    JWIN_KEY_NUM_8,
    JWIN_KEY_NUM_9,
    JWIN_KEY_NUM_DIV,
    JWIN_KEY_NUM_MUL,
    JWIN_KEY_NUM_SUB,
    JWIN_KEY_NUM_ADD,
    JWIN_KEY_NUM_ENTER,

    JWIN_KEY_F1,
    JWIN_KEY_F2,
    JWIN_KEY_F3,
    JWIN_KEY_F4,
    JWIN_KEY_F5,
    JWIN_KEY_F6,
    JWIN_KEY_F7,
    JWIN_KEY_F8,
    JWIN_KEY_F9,
    JWIN_KEY_F10,
    JWIN_KEY_F11,
    JWIN_KEY_F12,

    JWIN_KEY_ESC,

    JWIN_KEY_LAST = 256,
};

const char* jwin_keycode_to_str(jwin_keycode keycode);


enum jwin_mouse_button_type_T
{
    JWIN_MOUSE_BUTTON_TYPE_NONE         = 0,
    JWIN_MOUSE_BUTTON_TYPE_LEFT         = 1,
    JWIN_MOUSE_BUTTON_TYPE_MIDDLE       = 2,
    JWIN_MOUSE_BUTTON_TYPE_RIGHT        = 3,
    JWIN_MOUSE_BUTTON_TYPE_M4           = 4,
    JWIN_MOUSE_BUTTON_TYPE_M5           = 5,
    JWIN_MOUSE_BUTTON_TYPE_SCROLL_UP    = 6,
    JWIN_MOUSE_BUTTON_TYPE_SCROLL_DN    = 7,
};
typedef enum jwin_mouse_button_type_T jwin_mouse_button_type;

const char* jwin_mouse_button_type_to_str(jwin_mouse_button_type type);

enum jwin_mod_state_type_T
{
    JWIN_MOD_STATE_TYPE_NONE    = 0,
    JWIN_MOD_STATE_TYPE_SHIFT        = 1 << 0,
    JWIN_MOD_STATE_TYPE_CTRL    = 1 << 1,
    JWIN_MOD_STATE_TYPE_ALT    = 1 << 2,
    JWIN_MOD_STATE_TYPE_SUPER    = 1 << 3,
    JWIN_MOD_STATE_TYPE_CAPSLOCK = 1 << 4,
    JWIN_MOD_STATE_TYPE_NUMLOCK = 1 << 5,
};
typedef enum jwin_mod_state_type_T jwin_mod_state_type;

enum jwin_event_type_T
{
    JWIN_EVENT_TYPE_NONE = 0,
    JWIN_EVENT_TYPE_FOCUS_GAIN,         //  Window gained focus
    JWIN_EVENT_TYPE_FOCUS_LOSE,         //  Window lost focus

    JWIN_EVENT_TYPE_MOUSE_ENTER,        //  Mouse entered the window
    JWIN_EVENT_TYPE_MOUSE_LEAVE,        //  Mouse left the window
    JWIN_EVENT_TYPE_MOUSE_MOVE,         //  Mouse moved
    JWIN_EVENT_TYPE_MOUSE_PRESS,        //  Mouse button was pressed
    JWIN_EVENT_TYPE_MOUSE_RELEASE,      //  Mouse button was released
    JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS, //  Mouse button was pressed quickly after a previous release

//    JWIN_EVENT_TYPE_FOCUS_DND,                  //  Drag 'n' drop

    JWIN_EVENT_TYPE_KEY_PRESS,          //  Keyboard key was pressed
    JWIN_EVENT_TYPE_KEY_RELEASE,        //  Keyboard key was released
    JWIN_EVENT_TYPE_KEY_CHARACTER,      //  Keyboard key release generated a character

    JWIN_EVENT_TYPE_RESIZE,             //  Window was resized
    JWIN_EVENT_TYPE_MOVE,               //  Window was moved
    JWIN_EVENT_TYPE_REFRESH,            //  Window has to be refreshed/redrawn

    JWIN_EVENT_TYPE_CLOSE,              // Window was signaled to close
    JWIN_EVENT_TYPE_DESTROY,            // Window is about to be destroyed

    JWIN_EVENT_TYPE_CUSTOM,             // Custom event

    JWIN_EVENT_TYPE_COUNT,              //  Number of all events
};
typedef enum jwin_event_type_T jwin_event_type;

struct jwin_event_base_T
{
    jwin_event_type type;
    jwin_context* context;
    jwin_window* window;
};
typedef struct jwin_event_base_T jwin_event_base;

typedef struct jwin_event_base_T jwin_event_focus_gain;
typedef struct jwin_event_base_T jwin_event_focus_lose;

typedef struct jwin_event_base_T jwin_event_mouse_enter;
typedef struct jwin_event_base_T jwin_event_mouse_leave;

struct jwin_event_mouse_motion_T
{
    jwin_event_base base;
    jwin_mod_state_type mods;
    unsigned x, y;
};
typedef struct jwin_event_mouse_motion_T jwin_event_mouse_motion;

struct jwin_event_mouse_button_T
{
    jwin_event_base base;
    jwin_mouse_button_type button;
    jwin_mod_state_type mods;
    unsigned x, y;
};
typedef struct jwin_event_mouse_button_T jwin_event_mouse_button_press;
typedef struct jwin_event_mouse_button_T jwin_event_mouse_button_release;
typedef struct jwin_event_mouse_button_T jwin_event_mouse_button_double_press;

struct jwin_event_key_T
{
    jwin_event_base base;
    jwin_keycode keycode;
    jwin_mod_state_type mods;
    int repeated;
};
typedef struct jwin_event_key_T jwin_event_key_press;
typedef struct jwin_event_key_T jwin_event_key_release;

struct jwin_event_key_char_T
{
    jwin_event_base base;
    const char* utf8;
};
typedef struct jwin_event_key_char_T jwin_event_key_char;

typedef struct jwin_event_base_T jwin_event_close;
typedef struct jwin_event_base_T jwin_event_destroy;

struct jwin_event_custom_T
{
    jwin_event_base base;
    void* custom;
};
typedef struct jwin_event_custom_T jwin_event_custom;

typedef struct jwin_event_base_T jwin_event_refresh;

struct jwin_event_resize_T
{
    jwin_event_base base;
    unsigned width, height;
};
typedef struct jwin_event_resize_T jwin_event_resize;

struct jwin_event_move_T
{
    jwin_event_base base;
    int x, y;
};
typedef struct jwin_event_move_T jwin_event_move;


union jwin_event_any_T
{
    jwin_event_type type;
    jwin_event_base base;
    jwin_event_focus_gain focus_gain;
    jwin_event_focus_lose focus_lose;
    jwin_event_mouse_enter mouse_enter;
    jwin_event_mouse_leave mouse_leave;
    jwin_event_mouse_motion mouse_motion;
    jwin_event_mouse_button_press button_press;
    jwin_event_mouse_button_release button_release;
    jwin_event_mouse_button_double_press double_press;
    jwin_event_key_press key_press;
    jwin_event_key_release key_release;
    jwin_event_key_char key_char;
    jwin_event_resize resize;
    jwin_event_move move;
    jwin_event_refresh refresh;
    jwin_event_close close;
    jwin_event_destroy destroy;
    jwin_event_custom custom;
};
typedef union jwin_event_any_T jwin_event_any;

union jwin_event_callback_T
{
    void (*focus_gain)(const jwin_event_focus_gain* event, void* param);
    void (*focus_lose)(const jwin_event_focus_lose* event, void* param);

    void (*mouse_enter)(const jwin_event_mouse_enter* event, void* param);
    void (*mouse_leave)(const jwin_event_mouse_leave* event, void* param);

    void (*mouse_motion)(const jwin_event_mouse_motion* event, void* param);

    void (*mouse_button_press)(const jwin_event_mouse_button_press* event, void* param);
    void (*mouse_button_release)(const jwin_event_mouse_button_release* event, void* param);
    void (*mouse_button_double_press)(const jwin_event_mouse_button_double_press* event, void* param);

    void (*key_press)(const jwin_event_key_press* event, void* param);
    void (*key_release)(const jwin_event_key_release* event, void* param);
    void (*key_char)(const jwin_event_key_char* event, void* param);

    void (*move)(const jwin_event_move* event, void* param);
    void (*resize)(const jwin_event_resize* event, void* param);
    void (*refresh)(const jwin_event_refresh* event, void* param);

    int (*close)(const jwin_event_close* event, void* param);
    void (*destroy)(const jwin_event_destroy* event, void* param);

    void (*custom)(const jwin_event_custom* event, void* param);

    void* any;
};
typedef union jwin_event_callback_T jwin_event_callback;

struct jwin_event_handler_T
{
    jwin_event_type type;
    jwin_event_callback callback;
    void* param;
};
typedef struct jwin_event_handler_T jwin_event_handler;


size_t jwin_event_to_str(size_t buf_size, char* buffer, const jwin_event_any* event);

void jwin_context_set_event_hook(jwin_context* ctx, void(*hook)(const jwin_event_any* e, void* param), void* param);

void jwin_window_set_event_hook(jwin_window* win, void(*hook)(const jwin_event_any* e, void* param), void* param);

#endif //JWIN_EVENTS_H
