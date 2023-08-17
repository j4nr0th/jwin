//
// Created by jan on 6.8.2023.
//
#include "events.h"
#include <stdio.h>
#include <assert.h>

static const char* const EVENT_TYPE_STRUCT_NAMES[JWIN_EVENT_TYPE_COUNT] =
        {
                [JWIN_EVENT_TYPE_FOCUS_GAIN] = "jwin_event_focus_gain",
                [JWIN_EVENT_TYPE_FOCUS_LOSE] = "jwin_event_focus_lose",
                [JWIN_EVENT_TYPE_MOUSE_ENTER] = "jwin_event_mouse_enter",
                [JWIN_EVENT_TYPE_MOUSE_LEAVE] = "jwin_event_mouse_leave",
                [JWIN_EVENT_TYPE_MOUSE_MOVE] = "jwin_event_mouse_motion",
                [JWIN_EVENT_TYPE_MOUSE_PRESS] = "jwin_event_mouse_button_press",
                [JWIN_EVENT_TYPE_MOUSE_RELEASE] = "jwin_event_mouse_button_release",
                [JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS] = "jwin_event_mouse_button_double_press",
                [JWIN_EVENT_TYPE_KEY_PRESS] = "jwin_event_key_press",
                [JWIN_EVENT_TYPE_KEY_RELEASE] = "jwin_event_key_release",
                [JWIN_EVENT_TYPE_KEY_CHARACTER] = "jwin_event_key_char",
                [JWIN_EVENT_TYPE_CLOSE] = "jwin_event_close",
                [JWIN_EVENT_TYPE_DESTROY] = "jwin_event_destroy",
                [JWIN_EVENT_TYPE_CUSTOM] = "jwin_event_custom",
                [JWIN_EVENT_TYPE_REFRESH] = "jwin_event_refresh",
                [JWIN_EVENT_TYPE_MOVE] = "jwin_event_move",
                [JWIN_EVENT_TYPE_RESIZE] = "jwin_event_resize",
        };

static const char* event_type_to_struct_name(jwin_event_type type)
{
    if (type > JWIN_EVENT_TYPE_NONE && type < JWIN_EVENT_TYPE_COUNT)
    {
        return EVENT_TYPE_STRUCT_NAMES[type];
    }
    return NULL;
}


static const char* const EVENT_TYPE_NAMES[JWIN_EVENT_TYPE_COUNT] =
        {
                [JWIN_EVENT_TYPE_FOCUS_GAIN] = "JWIN_EVENT_TYPE_FOCUS_GAIN",
                [JWIN_EVENT_TYPE_FOCUS_LOSE] = "JWIN_EVENT_TYPE_FOCUS_LOSE",
                [JWIN_EVENT_TYPE_MOUSE_ENTER] = "JWIN_EVENT_TYPE_MOUSE_ENTER",
                [JWIN_EVENT_TYPE_MOUSE_LEAVE] = "JWIN_EVENT_TYPE_MOUSE_LEAVE",
                [JWIN_EVENT_TYPE_MOUSE_MOVE] = "JWIN_EVENT_TYPE_MOUSE_MOVE",
                [JWIN_EVENT_TYPE_MOUSE_PRESS] = "JWIN_EVENT_TYPE_MOUSE_PRESS",
                [JWIN_EVENT_TYPE_MOUSE_RELEASE] = "JWIN_EVENT_TYPE_MOUSE_RELEASE",
                [JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS] = "JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS",
                [JWIN_EVENT_TYPE_KEY_PRESS] = "JWIN_EVENT_TYPE_KEY_PRESS",
                [JWIN_EVENT_TYPE_KEY_RELEASE] = "JWIN_EVENT_TYPE_KEY_RELEASE",
                [JWIN_EVENT_TYPE_KEY_CHARACTER] = "JWIN_EVENT_TYPE_KEY_CHARACTER",
                [JWIN_EVENT_TYPE_CLOSE] = "JWIN_EVENT_TYPE_CLOSE",
                [JWIN_EVENT_TYPE_DESTROY] = "JWIN_EVENT_TYPE_DESTROY",
                [JWIN_EVENT_TYPE_RESIZE] = "JWIN_EVENT_TYPE_RESIZE",
                [JWIN_EVENT_TYPE_MOVE] = "JWIN_EVENT_TYPE_MOVE",
                [JWIN_EVENT_TYPE_REFRESH] = "JWIN_EVENT_TYPE_REFRESH",
                [JWIN_EVENT_TYPE_CUSTOM] = "JWIN_EVENT_TYPE_CUSTOM",
        };

static const char* event_type_to_str(jwin_event_type type)
{
    if (type > JWIN_EVENT_TYPE_NONE && type < JWIN_EVENT_TYPE_COUNT)
    {
        return EVENT_TYPE_NAMES[type];
    }
    return NULL;
}

static const char* const MOUSE_BUTTON_NAMES[] =
        {
                [JWIN_MOUSE_BUTTON_TYPE_NONE] = "JWIN_MOUSE_BUTTON_TYPE_NONE",
                [JWIN_MOUSE_BUTTON_TYPE_LEFT] = "JWIN_MOUSE_BUTTON_TYPE_LEFT",
                [JWIN_MOUSE_BUTTON_TYPE_MIDDLE] = "JWIN_MOUSE_BUTTON_TYPE_MIDDLE",
                [JWIN_MOUSE_BUTTON_TYPE_RIGHT] = "JWIN_MOUSE_BUTTON_TYPE_RIGHT",
                [JWIN_MOUSE_BUTTON_TYPE_M4] = "JWIN_MOUSE_BUTTON_TYPE_M4",
                [JWIN_MOUSE_BUTTON_TYPE_M5] = "JWIN_MOUSE_BUTTON_TYPE_M5",
                [JWIN_MOUSE_BUTTON_TYPE_SCROLL_UP] = "JWIN_MOUSE_BUTTON_TYPE_SCROLL_UP",
                [JWIN_MOUSE_BUTTON_TYPE_SCROLL_DN] = "JWIN_MOUSE_BUTTON_TYPE_SCROLL_DN",
        };

const char* jwin_mouse_button_type_to_str(jwin_mouse_button_type type)
{
    if (type < 0 || type >= sizeof(MOUSE_BUTTON_NAMES) / sizeof(*MOUSE_BUTTON_NAMES))
    {
        return "Unknown";
    }
    return MOUSE_BUTTON_NAMES[type];
}

const char* jwin_keycode_to_str(jwin_keycode keycode)
{
    switch (keycode)
    {
    case JWIN_KEY_NONE: return "JWIN_KEY_NONE";
    case JWIN_KEY_UP: return "JWIN_KEY_UP";
    case JWIN_KEY_LEFT: return "JWIN_KEY_LEFT";
    case JWIN_KEY_DOWN: return "JWIN_KEY_DOWN";
    case JWIN_KEY_RIGHT: return "JWIN_KEY_RIGHT";
    case JWIN_KEY_Q: return "JWIN_KEY_Q";
    case JWIN_KEY_W: return "JWIN_KEY_W";
    case JWIN_KEY_E: return "JWIN_KEY_E";
    case JWIN_KEY_R: return "JWIN_KEY_R";
    case JWIN_KEY_T: return "JWIN_KEY_T";
    case JWIN_KEY_Y: return "JWIN_KEY_Y";
    case JWIN_KEY_U: return "JWIN_KEY_U";
    case JWIN_KEY_I: return "JWIN_KEY_I";
    case JWIN_KEY_O: return "JWIN_KEY_O";
    case JWIN_KEY_P: return "JWIN_KEY_P";
    case JWIN_KEY_LBRACKET: return "JWIN_KEY_LBRACKET";
    case JWIN_KEY_RBRACKET: return "JWIN_KEY_RBRACKET";
    case JWIN_KEY_A: return "JWIN_KEY_A";
    case JWIN_KEY_S: return "JWIN_KEY_S";
    case JWIN_KEY_D: return "JWIN_KEY_D";
    case JWIN_KEY_F: return "JWIN_KEY_F";
    case JWIN_KEY_G: return "JWIN_KEY_G";
    case JWIN_KEY_H: return "JWIN_KEY_H";
    case JWIN_KEY_J: return "JWIN_KEY_J";
    case JWIN_KEY_K: return "JWIN_KEY_K";
    case JWIN_KEY_L: return "JWIN_KEY_L";
    case JWIN_KEY_SEMICOL: return "JWIN_KEY_SEMICOL";
    case JWIN_KEY_APPOSTROPHE: return "JWIN_KEY_APPOSTROPHE";
    case JWIN_KEY_BACKSLASH: return "JWIN_KEY_BACKSLASH";
    case JWIN_KEY_Z: return "JWIN_KEY_Z";
    case JWIN_KEY_X: return "JWIN_KEY_X";
    case JWIN_KEY_C: return "JWIN_KEY_C";
    case JWIN_KEY_V: return "JWIN_KEY_V";
    case JWIN_KEY_B: return "JWIN_KEY_B";
    case JWIN_KEY_N: return "JWIN_KEY_N";
    case JWIN_KEY_M: return "JWIN_KEY_M";
    case JWIN_KEY_COMMA: return "JWIN_KEY_COMMA";
    case JWIN_KEY_PERIOD: return "JWIN_KEY_PERIOD";
    case JWIN_KEY_SLASH: return "JWIN_KEY_SLASH";
    case JWIN_KEY_GRAVE: return "JWIN_KEY_GRAVE";
    case JWIN_KEY_1: return "JWIN_KEY_1";
    case JWIN_KEY_2: return "JWIN_KEY_2";
    case JWIN_KEY_3: return "JWIN_KEY_3";
    case JWIN_KEY_4: return "JWIN_KEY_4";
    case JWIN_KEY_5: return "JWIN_KEY_5";
    case JWIN_KEY_6: return "JWIN_KEY_6";
    case JWIN_KEY_7: return "JWIN_KEY_7";
    case JWIN_KEY_8: return "JWIN_KEY_8";
    case JWIN_KEY_9: return "JWIN_KEY_9";
    case JWIN_KEY_0: return "JWIN_KEY_0";
    case JWIN_KEY_MINUS: return "JWIN_KEY_MINUS";
    case JWIN_KEY_EQUAL: return "JWIN_KEY_EQUAL";
    case JWIN_KEY_BACKSPACE: return "JWIN_KEY_BACKSPACE";
    case JWIN_KEY_TAB: return "JWIN_KEY_TAB";
    case JWIN_KEY_SPACE: return "JWIN_KEY_SPACE";
    case JWIN_KEY_CAPSLOCK: return "JWIN_KEY_CAPSLOCK";
    case JWIN_KEY_LSHIFT: return "JWIN_KEY_LSHIFT";
    case JWIN_KEY_RSHIFT: return "JWIN_KEY_RSHIFT";
    case JWIN_KEY_LCTRL: return "JWIN_KEY_LCTRL";
    case JWIN_KEY_RCTRL: return "JWIN_KEY_RCTRL";
    case JWIN_KEY_MENU: return "JWIN_KEY_MENU";
    case JWIN_KEY_LSUPER: return "JWIN_KEY_LSUPER";
    case JWIN_KEY_RSUPER: return "JWIN_KEY_RSUPER";
    case JWIN_KEY_LALT: return "JWIN_KEY_LALT";
    case JWIN_KEY_RALT: return "JWIN_KEY_RALT";
    case JWIN_KEY_RETURN: return "JWIN_KEY_RETURN";
    case JWIN_KEY_INSERT: return "JWIN_KEY_INSERT";
    case JWIN_KEY_HOME: return "JWIN_KEY_HOME";
    case JWIN_KEY_END: return "JWIN_KEY_END";
    case JWIN_KEY_DELETE: return "JWIN_KEY_DELETE";
    case JWIN_KEY_PGUP: return "JWIN_KEY_PGUP";
    case JWIN_KEY_PGDN: return "JWIN_KEY_PGDN";
    case JWIN_KEY_PRINTSCR: return "JWIN_KEY_PRINTSCR";
    case JWIN_KEY_SCRLOCK: return "JWIN_KEY_SCRLOCK";
    case JWIN_KEY_PAUSE: return "JWIN_KEY_PAUSE";
    case JWIN_KEY_NUMLOCK: return "JWIN_KEY_NUMLOCK";
    case JWIN_KEY_NUM_SEP: return "JWIN_KEY_NUM_SEP";
    case JWIN_KEY_NUM_0: return "JWIN_KEY_NUM_0";
    case JWIN_KEY_NUM_1: return "JWIN_KEY_NUM_1";
    case JWIN_KEY_NUM_2: return "JWIN_KEY_NUM_2";
    case JWIN_KEY_NUM_3: return "JWIN_KEY_NUM_3";
    case JWIN_KEY_NUM_4: return "JWIN_KEY_NUM_4";
    case JWIN_KEY_NUM_5: return "JWIN_KEY_NUM_5";
    case JWIN_KEY_NUM_6: return "JWIN_KEY_NUM_6";
    case JWIN_KEY_NUM_7: return "JWIN_KEY_NUM_7";
    case JWIN_KEY_NUM_8: return "JWIN_KEY_NUM_8";
    case JWIN_KEY_NUM_9: return "JWIN_KEY_NUM_9";
    case JWIN_KEY_NUM_DIV: return "JWIN_KEY_NUM_DIV";
    case JWIN_KEY_NUM_MUL: return "JWIN_KEY_NUM_MUL";
    case JWIN_KEY_NUM_SUB: return "JWIN_KEY_NUM_SUB";
    case JWIN_KEY_NUM_ADD: return "JWIN_KEY_NUM_ADD";
    case JWIN_KEY_NUM_ENTER: return "JWIN_KEY_NUM_ENTER";
    case JWIN_KEY_F1: return "JWIN_KEY_F1";
    case JWIN_KEY_F2: return "JWIN_KEY_F2";
    case JWIN_KEY_F3: return "JWIN_KEY_F3";
    case JWIN_KEY_F4: return "JWIN_KEY_F4";
    case JWIN_KEY_F5: return "JWIN_KEY_F5";
    case JWIN_KEY_F6: return "JWIN_KEY_F6";
    case JWIN_KEY_F7: return "JWIN_KEY_F7";
    case JWIN_KEY_F8: return "JWIN_KEY_F8";
    case JWIN_KEY_F9: return "JWIN_KEY_F9";
    case JWIN_KEY_F10: return "JWIN_KEY_F10";
    case JWIN_KEY_F11: return "JWIN_KEY_F11";
    case JWIN_KEY_F12: return "JWIN_KEY_F12";
    case JWIN_KEY_ESC: return "JWIN_KEY_ESC";
    case JWIN_KEY_LAST: return "JWIN_KEY_LAST";
    default: return "Unknown";
    }
}

static size_t print_event_base(size_t buf_size, char* buffer, const jwin_event_any* e)
{
    const char* const type_str = event_type_to_str(e->type);
    if (type_str)
    {
        int written = snprintf(
                buffer, buf_size, "\t.base = { .type = %s, .context = %p, .window = %p },\n", type_str, e->base.context,
                e->base.window);
        if (written <= 0)
        {
            return 0;
        }
        else if (buf_size < (size_t) written)
        {
            return buf_size;
        }
        return written;
    }
    return 0;
}
#ifndef _WIN32
    #define PRINT_TO_BUFFER(fmt, ...) {chars_written += snprintf(buffer + chars_written, buf_size - chars_written, fmt __VA_OPT__(,) __VA_ARGS__); if (chars_written > buf_size) {chars_written = buf_size;}}(void)0
#else
    #define PRINT_TO_BUFFER(fmt, ...) {chars_written += snprintf(buffer + chars_written, buf_size - chars_written, fmt, __VA_ARGS__); if (chars_written > buf_size) {chars_written = buf_size;}}(void)0
#endif


static size_t print_mods(size_t buf_size, char* buffer, const jwin_mod_state_type mods)
{
    size_t chars_written = 0;
    unsigned was_before = 0;
    static const char* const mod_names[] =
            {
                    "JWIN_MOD_STATE_TYPE_SHIFT",
                    "JWIN_MOD_STATE_TYPE_CTRL",
                    "JWIN_MOD_STATE_TYPE_ALT",
                    "JWIN_MOD_STATE_TYPE_SUPER",
                    "JWIN_MOD_STATE_TYPE_CAPSLOCK",
                    "JWIN_MOD_STATE_TYPE_NUMLOCK",
            };
    static const jwin_mod_state_type mod_values[] =
            {
                    JWIN_MOD_STATE_TYPE_SHIFT,
                    JWIN_MOD_STATE_TYPE_CTRL,
                    JWIN_MOD_STATE_TYPE_ALT,
                    JWIN_MOD_STATE_TYPE_SUPER,
                    JWIN_MOD_STATE_TYPE_CAPSLOCK,
                    JWIN_MOD_STATE_TYPE_NUMLOCK,
            };
    static const unsigned len = sizeof(mod_values) / sizeof(*mod_values);
    for (unsigned i = 0; i < len; ++i)
    {
        if (mods & mod_values[i])
        {
            PRINT_TO_BUFFER(was_before ? "|%s" : "%s", mod_names[i]);
            was_before = 1;
        }
    }

    return chars_written;
}

size_t jwin_event_to_str(size_t buf_size, char* buffer, const jwin_event_any* event)
{
    if (event->type <= JWIN_EVENT_TYPE_NONE || event->type >= JWIN_EVENT_TYPE_COUNT)
    {
        return 0;
    }
    const char* event_type = event_type_to_struct_name(event->type);
    if (!event_type)
    {
        return 0;
    }
    size_t chars_written = 0;
    PRINT_TO_BUFFER("(%s) {\n", event_type);
    chars_written += print_event_base(buf_size - chars_written, buffer + chars_written, event);
    switch (event->type)
    {
    case JWIN_EVENT_TYPE_FOCUS_GAIN:
    case JWIN_EVENT_TYPE_FOCUS_LOSE:
    case JWIN_EVENT_TYPE_MOUSE_ENTER:
    case JWIN_EVENT_TYPE_MOUSE_LEAVE:
    case JWIN_EVENT_TYPE_REFRESH:
    case JWIN_EVENT_TYPE_CLOSE:
    case JWIN_EVENT_TYPE_DESTROY:
    case JWIN_EVENT_TYPE_COUNT:
    case JWIN_EVENT_TYPE_NONE:
//        PRINT_TO_BUFFER("\t.type = %s, .context = %p, .window = %p\n", event_type, event->base.context, event->base.window);
        break;

    case JWIN_EVENT_TYPE_MOUSE_MOVE:
    {
        const jwin_event_mouse_motion* e = &event->mouse_motion;
        PRINT_TO_BUFFER("\t.x = %u,\n\t.y = %u,\n\t.mods = ", e->x, e->y);
        chars_written += print_mods(buf_size - chars_written, buffer + chars_written, e->mods);
        PRINT_TO_BUFFER("\n");
    }
        break;

    case JWIN_EVENT_TYPE_MOUSE_PRESS:
    {
        const jwin_event_mouse_button_press* e = &event->button_press;
        PRINT_TO_BUFFER("\t.x = %u,\n\t.y = %u,\n\t.mods = ", e->x, e->y);
        chars_written += print_mods(buf_size - chars_written, buffer + chars_written, e->mods);
        PRINT_TO_BUFFER("\n\t.button = %s\n", jwin_mouse_button_type_to_str(e->button));
    }
        break;

    case JWIN_EVENT_TYPE_MOUSE_RELEASE:
    {
        const jwin_event_mouse_button_release* e = &event->button_release;
        PRINT_TO_BUFFER("\t.x = %u,\n\t.y = %u,\n\t.mods = ", e->x, e->y);
        chars_written += print_mods(buf_size - chars_written, buffer + chars_written, e->mods);
        PRINT_TO_BUFFER("\n\t.button = %s\n", jwin_mouse_button_type_to_str(e->button));
    }
        break;

    case JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS:
    {
        const jwin_event_mouse_button_double_press* e = &event->double_press;
        PRINT_TO_BUFFER("\t.x = %u,\n\t.y = %u,\n\t.mods = ", e->x, e->y);
        chars_written += print_mods(buf_size - chars_written, buffer + chars_written, e->mods);
        PRINT_TO_BUFFER("\n\t.button = %s\n", jwin_mouse_button_type_to_str(e->button));
    }
        break;

    case JWIN_EVENT_TYPE_KEY_RELEASE:
    {
        const jwin_event_key_release* e = &event->key_release;
        PRINT_TO_BUFFER("\t.keycode = %s,\n\t.mods = ", jwin_keycode_to_str(e->keycode));
        chars_written += print_mods(buf_size - chars_written, buffer + chars_written, e->mods);
        PRINT_TO_BUFFER("\n\t.repeated = %s,\n", e->repeated ? "true" : "false");
    }
        break;

    case JWIN_EVENT_TYPE_KEY_PRESS:
    {
        const jwin_event_key_press* e = &event->key_press;
        PRINT_TO_BUFFER("\t.keycode = %s,\n\t.mods = ", jwin_keycode_to_str(e->keycode));
        chars_written += print_mods(buf_size - chars_written, buffer + chars_written, e->mods);
        PRINT_TO_BUFFER("\n\t.repeated = %s,\n", e->repeated ? "true" : "false");
    }
        break;

    case JWIN_EVENT_TYPE_KEY_CHARACTER:
    {
        const jwin_event_key_char* e = &event->key_char;
    	PRINT_TO_BUFFER("\t.utf8 = \"%s\",\n", e->utf8);
    }
        break;

    case JWIN_EVENT_TYPE_CUSTOM:
    {
        const jwin_event_custom* e = &event->custom;
        PRINT_TO_BUFFER("\t.custom = %p,\n", e->custom);
    }
        break;

    case JWIN_EVENT_TYPE_RESIZE:
    {
        const jwin_event_resize* e = &event->resize;
        PRINT_TO_BUFFER("\t.width = %u,\n\t.height = %u,\n", e->width, e->height);
    }
        break;

    case JWIN_EVENT_TYPE_MOVE:
    {
        const jwin_event_move* e = &event->move;
        PRINT_TO_BUFFER("\t.x = %d,\n\t.y = %d,\n", e->x, e->y);
    }
        break;

    }
    PRINT_TO_BUFFER("}");


    return chars_written;
}
