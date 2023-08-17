//
// Created by jan on 6.8.2023.
//

#include <string.h>
#include <assert.h>
#include <errno.h>
#include "context_win32.h"
#include "../internal.h"
#include "../context.h"
#include <windowsx.h>
#include "window_win32.h"

jwin_result jwin_context_create(const jwin_context_create_info* create_info, jwin_context** p_out)
{
#ifdef ALLOC
#error macro already defined
#endif
#ifdef FREE
#error macro already defined
#endif

#define ALLOC(size) info.allocator_callbacks->alloc(info.allocator_callbacks->state, (size))
#define FREE(ptr) info.allocator_callbacks->free(info.allocator_callbacks->state, (ptr))

    jwin_context_create_info info = *create_info;
    if (!info.allocator_callbacks)
    {
        info.allocator_callbacks = &JWIN_DEFAULT_ALLOCATOR_CALLBACKS;
    }
    else if (!CHECK_ALLOCATOR_CALLBACKS(info.allocator_callbacks))
    {
        return JWIN_RESULT_BAD_ALLOCATOR;
    }
    jwin_result res;
    jwin_context* ctx = NULL;
    const HINSTANCE hinst = GetModuleHandleA(NULL);


    ctx = ALLOC(sizeof(*ctx));
    if (!ctx)
    {
        return JWIN_RESULT_BAD_ALLOC;
    }
    ctx->error_callbacks = *info.error_callbacks;

    //  Register window class with WIN32
    WNDCLASSW wc =
    {
        .hInstance = hinst,
        .style = CS_DBLCLKS,
        .lpfnWndProc = INTERNAL_window_proc,
        .lpszClassName = L"jwin-window",
    };

    if (!RegisterClassW(&wc))
    {
        res = JWIN_RESULT_NO_PLATFORM;
        REPORT_ERROR(ctx, "Could not open register windows window class display");
        goto failed;
    }
    ctx->hinstance = hinst;
    ctx->window_class = wc;
    ctx->allocator_callbacks = *info.allocator_callbacks;

    

    ctx->group = (unsigned int) VK_TAB;
    
    {
        memset(ctx->keymap, 0, sizeof(ctx->keymap));
        memset(ctx->reverse_keymap, 0, sizeof(ctx->reverse_keymap));
        
        struct
        {
            jwin_keycode key;
            unsigned keycode;
        } key_table[] =
                {

                        { .key = JWIN_KEY_UP, .keycode = VK_UP },
                        { .key = JWIN_KEY_LEFT, .keycode = VK_LEFT },
                        { .key = JWIN_KEY_DOWN, .keycode = VK_DOWN },
                        { .key = JWIN_KEY_RIGHT, .keycode = VK_RIGHT },
                        { .key = JWIN_KEY_Q, .keycode = 0x51 },
                        { .key = JWIN_KEY_W, .keycode = 0x57 },
                        { .key = JWIN_KEY_E, .keycode = 0x45 },
                        { .key = JWIN_KEY_R, .keycode = 0x52 },
                        { .key = JWIN_KEY_T, .keycode = 0x54 },
                        { .key = JWIN_KEY_Y, .keycode = 0x59 },
                        { .key = JWIN_KEY_U, .keycode = 0x55 },
                        { .key = JWIN_KEY_I, .keycode = 0x49 },
                        { .key = JWIN_KEY_O, .keycode = 0x4F },
                        { .key = JWIN_KEY_P, .keycode = 0x50},
                        { .key = JWIN_KEY_LBRACKET, .keycode = VK_OEM_4 },
                        { .key = JWIN_KEY_RBRACKET, .keycode = VK_OEM_6 },
                        { .key = JWIN_KEY_A, .keycode = 0x41 },
                        { .key = JWIN_KEY_S, .keycode = 0x53 },
                        { .key = JWIN_KEY_D, .keycode = 0x44 },
                        { .key = JWIN_KEY_F, .keycode = 0x46 },
                        { .key = JWIN_KEY_G, .keycode = 0x47 },
                        { .key = JWIN_KEY_H, .keycode = 0x48 },
                        { .key = JWIN_KEY_J, .keycode = 0x5A },
                        { .key = JWIN_KEY_K, .keycode = 0x5B },
                        { .key = JWIN_KEY_L, .keycode = 0x4C },
                        { .key = JWIN_KEY_SEMICOL, .keycode = 0xBA },
                        { .key = JWIN_KEY_APPOSTROPHE, .keycode = VK_OEM_7 },
                        { .key = JWIN_KEY_BACKSLASH, .keycode = VK_OEM_5 },
                        { .key = JWIN_KEY_Z, .keycode = 0x5A },
                        { .key = JWIN_KEY_X, .keycode = 0x58 },
                        { .key = JWIN_KEY_C, .keycode = 0x43 },
                        { .key = JWIN_KEY_V, .keycode = 0x56 },
                        { .key = JWIN_KEY_B, .keycode = 0x42 },
                        { .key = JWIN_KEY_N, .keycode = 0x4E },
                        { .key = JWIN_KEY_M, .keycode = 0x4D },
                        { .key = JWIN_KEY_COMMA, .keycode = 0xBC },
                        { .key = JWIN_KEY_PERIOD, .keycode = 0xBE },
                        { .key = JWIN_KEY_SLASH, .keycode = 0xBD },
                        { .key = JWIN_KEY_GRAVE, .keycode = 0xC0 },
                        { .key = JWIN_KEY_1, .keycode = 0x31 },
                        { .key = JWIN_KEY_2, .keycode = 0x32 },
                        { .key = JWIN_KEY_3, .keycode = 0x33 },
                        { .key = JWIN_KEY_4, .keycode = 0x34 },
                        { .key = JWIN_KEY_5, .keycode = 0x35 },
                        { .key = JWIN_KEY_6, .keycode = 0x36 },
                        { .key = JWIN_KEY_7, .keycode = 0x37 },
                        { .key = JWIN_KEY_8, .keycode = 0x38 },
                        { .key = JWIN_KEY_9, .keycode = 0x39 },
                        { .key = JWIN_KEY_0, .keycode = 0x30 },
                        { .key = JWIN_KEY_MINUS, .keycode = 0xBF },
                        { .key = JWIN_KEY_EQUAL, .keycode = 0xBB },
                        { .key = JWIN_KEY_BACKSPACE, .keycode = VK_BACK },
                        { .key = JWIN_KEY_TAB, .keycode = VK_TAB },
                        { .key = JWIN_KEY_CAPSLOCK, .keycode = VK_CAPITAL },
                        { .key = JWIN_KEY_LSHIFT, .keycode = VK_SHIFT },
                        { .key = JWIN_KEY_RSHIFT, .keycode = VK_RSHIFT },
                        { .key = JWIN_KEY_LCTRL, .keycode = VK_CONTROL },
                        { .key = JWIN_KEY_RCTRL, .keycode = VK_RCONTROL },
                        { .key = JWIN_KEY_MENU, .keycode = 0x5D },
                        { .key = JWIN_KEY_RSUPER, .keycode = VK_LWIN },
                        { .key = JWIN_KEY_LSUPER, .keycode = VK_RWIN },
                        { .key = JWIN_KEY_LALT, .keycode = VK_MENU },
                        { .key = JWIN_KEY_RALT, .keycode = VK_RMENU },
                        { .key = JWIN_KEY_RETURN, .keycode = VK_RETURN },
                        { .key = JWIN_KEY_INSERT, .keycode = VK_INSERT },
                        { .key = JWIN_KEY_HOME, .keycode = VK_HOME },
                        { .key = JWIN_KEY_END, .keycode = VK_END },
                        { .key = JWIN_KEY_DELETE, .keycode = VK_DELETE },
                        { .key = JWIN_KEY_PGUP, .keycode = VK_PRIOR },
                        { .key = JWIN_KEY_PGDN, .keycode = VK_NEXT },
                        { .key = JWIN_KEY_PRINTSCR, .keycode = VK_SNAPSHOT },
                        { .key = JWIN_KEY_SCRLOCK, .keycode = VK_SCROLL },
                        { .key = JWIN_KEY_PAUSE, .keycode = VK_PAUSE },
                        { .key = JWIN_KEY_NUMLOCK, .keycode = VK_NUMLOCK },
                        { .key = JWIN_KEY_NUM_SEP, .keycode = VK_SEPARATOR },
                        { .key = JWIN_KEY_NUM_0, .keycode = VK_NUMPAD0 },
                        { .key = JWIN_KEY_NUM_1, .keycode = VK_NUMPAD1 },
                        { .key = JWIN_KEY_NUM_2, .keycode = VK_NUMPAD2 },
                        { .key = JWIN_KEY_NUM_3, .keycode = VK_NUMPAD3 },
                        { .key = JWIN_KEY_NUM_4, .keycode = VK_NUMPAD4 },
                        { .key = JWIN_KEY_NUM_5, .keycode = VK_NUMPAD5 },
                        { .key = JWIN_KEY_NUM_6, .keycode = VK_NUMPAD6 },
                        { .key = JWIN_KEY_NUM_7, .keycode = VK_NUMPAD7 },
                        { .key = JWIN_KEY_NUM_8, .keycode = VK_NUMPAD8 },
                        { .key = JWIN_KEY_NUM_9, .keycode = VK_NUMPAD9 },
                        { .key = JWIN_KEY_NUM_DIV, .keycode = VK_DIVIDE },
                        { .key = JWIN_KEY_NUM_MUL, .keycode = VK_MULTIPLY },
                        { .key = JWIN_KEY_NUM_SUB, .keycode = VK_SUBTRACT },
                        { .key = JWIN_KEY_NUM_ADD, .keycode = VK_ADD },
                        { .key = JWIN_KEY_NUM_ENTER, .keycode = 0x0D },
                        { .key = JWIN_KEY_F1, .keycode = VK_F1 },
                        { .key = JWIN_KEY_F2, .keycode = VK_F2 },
                        { .key = JWIN_KEY_F3, .keycode = VK_F3 },
                        { .key = JWIN_KEY_F4, .keycode = VK_F4 },
                        { .key = JWIN_KEY_F5, .keycode = VK_F5 },
                        { .key = JWIN_KEY_F6, .keycode = VK_F6 },
                        { .key = JWIN_KEY_F7, .keycode = VK_F7 },
                        { .key = JWIN_KEY_F8, .keycode = VK_F8 },
                        { .key = JWIN_KEY_F9, .keycode = VK_F9 },
                        { .key = JWIN_KEY_F10, .keycode = VK_F10 },
                        { .key = JWIN_KEY_F11, .keycode = VK_F11 },
                        { .key = JWIN_KEY_F12, .keycode = VK_F12 },
                        { .key = JWIN_KEY_ESC, .keycode = VK_ESCAPE },
                        { .key = JWIN_KEY_SPACE, .keycode = VK_SPACE },
                };
        for (unsigned vk = 0; vk < sizeof(key_table) / sizeof(*key_table); ++vk)
        {
            ctx->keymap[key_table[vk].keycode] = key_table[vk].key;
        }


        for (unsigned vk = 0; vk < sizeof(key_table) / sizeof(*key_table); ++vk)
        {
            ctx->reverse_keymap[key_table[vk].key] = key_table[vk].keycode;
        }

    }

    ctx->window_count = 0;
    ctx->window_capacity = 0;
    ctx->window_array = NULL;
    ctx->event_hook = NULL;
    ctx->event_param = NULL;
    ctx->should_close = 0;

    *p_out = ctx;

    return JWIN_RESULT_SUCCESS;

    failed:
    FREE(ctx);
    return res;
}

void jwin_context_destroy(jwin_context* ctx)
{
    UnregisterClassW(ctx->window_class.lpszClassName, ctx->hinstance);
    ctx->allocator_callbacks.free(ctx->allocator_callbacks.state, ctx->window_array);
    ctx->allocator_callbacks.free(ctx->allocator_callbacks.state, ctx);
}

void jwin_context_set_user_ptr(jwin_context* ctx, void* ptr)
{
    ctx->usr_ptr = ptr;
}

void* jwin_context_get_user_ptr(jwin_context* ctx)
{
    return ctx ? ctx->usr_ptr : NULL;
}

void jwin_context_mark_to_close(jwin_context* ctx)
{
    ctx->should_close = 1;
}

unsigned jwin_context_window_count(jwin_context* ctx)
{
    return ctx->window_count;
}

#undef FREE
#undef ALLOC


jwin_result jwin_context_handle_event(jwin_context* ctx)
{
    jwin_event_custom event_custom;

    MSG message;

    //  Process the WIN32 event

    if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        //  Translate message (generates WM_CHAR from WM_KEYDOWN)
        TranslateMessage(&message);
        //  Send it to correct window
        DispatchMessage(&message);
    }

    if (ctx->should_close)
    {
        return JWIN_RESULT_SHOULD_CLOSE;
    }

    for (unsigned i = 0; i < ctx->window_count; ++i)
    {
        jwin_window* const win = ctx->window_array[i];
        if (win->should_close)
        {
            jwin_window_close(win);
        }
    }

    return JWIN_RESULT_SUCCESS;
}

int jwin_context_should_close(jwin_context* ctx)
{
    return ctx->should_close;
}

jwin_result jwin_context_wait_for_events(jwin_context* ctx)
{
    return INTERNAL_wait_for_any(ctx, -1);
}

jwin_result jwin_context_wait_for_events_timeout(jwin_context* ctx, int timeout_ms)
{
    return INTERNAL_wait_for_any(ctx, timeout_ms);
}

jwin_result jwin_context_handle_events(jwin_context* ctx)
{
    jwin_event_custom event_custom;

    MSG message;

    //  Process the WIN32 event

    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        //  Translate message (generates WM_CHAR from WM_KEYDOWN)
        TranslateMessage(&message);
        //  Send it to correct window
        DispatchMessage(&message);
    }

    if (ctx->should_close)
    {
        return JWIN_RESULT_SHOULD_CLOSE;
    }

    for (unsigned i = 0; i < ctx->window_count; ++i)
    {
        jwin_window* const win = ctx->window_array[i];
        if (win->should_close)
        {
            jwin_window_close(win);
        }
    }

    return JWIN_RESULT_SUCCESS;
}

jwin_result INTERNAL_add_window_to_context(jwin_context* ctx, jwin_window* win)
{
    //  Make sure there is enough space in the array
    if (ctx->window_count == ctx->window_capacity)
    {
        const unsigned new_capacity = ctx->window_capacity ? ctx->window_capacity << 1 : 8;
        jwin_window** const new_ptr = ctx->allocator_callbacks.realloc(
                ctx->allocator_callbacks.state,
                ctx->window_array,
                sizeof(jwin_window*) * new_capacity);
        if (!new_ptr)
        {
            REPORT_ERROR(ctx, "Could not (re-)allocate memory for window array");
            return JWIN_RESULT_BAD_ALLOC;
        }
#ifndef NDEBUG
        //  On debug mode fill it up with garbage
        memset(new_ptr + ctx->window_count, 0xCC, sizeof(jwin_window*) * (new_capacity - ctx->window_capacity));
#endif
        ctx->window_array = new_ptr;
        ctx->window_capacity = new_capacity;
    }

    ctx->window_array[ctx->window_count] = win;
    ctx->window_count += 1;
    
    return JWIN_RESULT_SUCCESS;
}

jwin_result INTERNAL_remove_window_from_context(jwin_context* ctx, const jwin_window* win)
{
    unsigned pos;
    for (pos = 0; pos < ctx->window_count; ++pos)
    {
        if (ctx->window_array[pos]->hwnd == win->hwnd)
        {
            break;
        }
    }

    //  Window was not in the context's window array
    if (pos == ctx->window_count)
    {
        REPORT_ERROR(ctx,
                     "Window was not found in the context (element at pos = %u had a handle %p, but %p was needed",
                     pos, ctx->window_array[pos]->hwnd, win->hwnd);
        return JWIN_RESULT_BAD_CONTEXT;
    }
    
    memmove(
            ctx->window_array + pos, ctx->window_array + pos + 1,
            sizeof(jwin_window*) * (ctx->window_count - pos - 1));

    ctx->window_array[ctx->window_count - 1] = (void*) 0xCCCCCCCCCCCCCCCC;
    ctx->window_count -= 1;

    return JWIN_RESULT_SUCCESS;
}

jwin_result INTERNAL_wait_for_any(const jwin_context* ctx, int ms_timeout)
{
    if (ctx->should_close)
    {
        return JWIN_RESULT_SHOULD_CLOSE;
    }
    if (WaitMessage()) 
    {
        return JWIN_RESULT_SUCCESS;
    }
    return JWIN_RESULT_BAD_WAIT;
}

static inline jwin_keycode win32_keycode_to_jwin_keycode(const jwin_context* ctx, unsigned keycode)
{
    if (keycode >= JWIN_KEY_LAST)
    {
        return JWIN_KEY_NONE;
    }
    return ctx->keymap[keycode];
}

static inline jwin_mod_state_type win32_mods_to_jwin_mods(jwin_context* ctx)
{
    jwin_mod_state_type mods =
            (((GetKeyState(VK_LSHIFT) & 0x8000) || (GetKeyState(VK_RSHIFT) & 0x8000))  ? JWIN_MOD_STATE_TYPE_SHIFT : 0) |
            ((GetKeyState(VK_LCONTROL) & 0x8000) || (GetKeyState(VK_RCONTROL) & 0x8000) ? JWIN_MOD_STATE_TYPE_CTRL : 0) |
            ((GetKeyState(VK_LMENU) & 0x8000) || (GetKeyState(VK_RMENU) & 0x8000) ? JWIN_MOD_STATE_TYPE_ALT : 0) |
            ((GetKeyState(VK_LWIN) & 0x8000) || (GetKeyState(VK_RWIN) & 0x8000) ? JWIN_MOD_STATE_TYPE_SUPER : 0) |
            ((GetKeyState(VK_CAPITAL) & 1) ? JWIN_MOD_STATE_TYPE_CAPSLOCK : 0) |
            ((GetKeyState(VK_NUMLOCK) & 1) ? JWIN_MOD_STATE_TYPE_NUMLOCK : 0);
    return mods;
}

static inline int should_process_event(const jwin_window* win, const jwin_event_type type)
{
    return win->event_handlers[type].callback.any != NULL || win->event_hook != NULL || win->ctx->event_hook != NULL;
}

LRESULT CALLBACK INTERNAL_window_proc(HWND wnd, UINT type, WPARAM wparam, LPARAM lparam)
{
    switch (type)
    {
    case WM_CREATE:
    {
        const CREATESTRUCT* const cs = (const CREATESTRUCT*)lparam;
        SetWindowLongPtr(wnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
    }
    return 0;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        const int repeated = (lparam & 0xFFFF) != 0;

        void (*kp_callback)(
            const jwin_event_key_press*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_KEY_PRESS].callback.key_press;
        const jwin_keycode key = win32_keycode_to_jwin_keycode(ctx, (unsigned int)wparam);
        const jwin_mod_state_type mods = win32_mods_to_jwin_mods(ctx);

        jwin_event_key_press ev =
        {
                .base = {.type = JWIN_EVENT_TYPE_KEY_PRESS, .context = ctx, .window = win },
                .keycode = key,
                .mods = mods,
                .repeated = repeated,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .key_press = ev });
        if (kp_callback)
        {
            kp_callback(&ev, win->event_handlers[JWIN_EVENT_TYPE_KEY_PRESS].param);
        }
    }
    return 0;

    case WM_CHAR:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;

        void (*kp_callback)(
            const jwin_event_key_char*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_KEY_CHARACTER].callback.key_char;
        char buffer[5] = { 0 };
        //  Convert UTF-16 to UTF-8
        WCHAR wc = (WCHAR)wparam;
        if (wc < 0x80)
        {
            //  One code point
            buffer[0] = (char)wc;
        }
        else if (wc < 0x0800)
        {
            //  Two code points
            buffer[0] = (char)(wc >> 6) | 0xC0;
            buffer[1] = (char)(wc & 0x3F) | 0x80;
        }
        else
        {
            //  Three code points
            buffer[0] = (char)(wc >> 12) | 0xE0;
            buffer[1] = (char)((wc >> 6) & 0x3F) | 0x80;
            buffer[2] = (char)(wc & 0x3F) | 0x80;
        }
        jwin_event_key_char ev =
        {
                .base = {.type = JWIN_EVENT_TYPE_KEY_CHARACTER, .context = ctx, .window = win },
                .utf8 = buffer,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .key_char = ev });
        if (kp_callback)
        {
            kp_callback(&ev, win->event_handlers[JWIN_EVENT_TYPE_KEY_CHARACTER].param);
        }
    }
    return 0;

    case WM_SYSKEYUP:
    case WM_KEYUP:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*kr_callback)(
            const jwin_event_key_release*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_KEY_RELEASE].callback.key_release;
        const jwin_keycode key = win32_keycode_to_jwin_keycode(ctx, (unsigned int)wparam);
        const jwin_mod_state_type mods = win32_mods_to_jwin_mods(ctx);

        jwin_event_key_release e =
        {
                .base = {.type = JWIN_EVENT_TYPE_KEY_RELEASE, .context = ctx, .window = win },
                .keycode = key,
                .mods = mods,
                .repeated = 0,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .key_release = e });
        if (kr_callback)
        {
            kr_callback(&e, win->event_handlers[JWIN_EVENT_TYPE_KEY_RELEASE].param);
        }
    }
    return 0;

    case WM_LBUTTONDOWN:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*press)(
            const jwin_event_mouse_button_press*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].callback.mouse_button_press;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_PRESS, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = JWIN_MOUSE_BUTTON_TYPE_LEFT,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_press = e });
        if (press)
        {
            press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].param);
        }
    }
    return 0;

    case WM_RBUTTONDOWN:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*press)(
            const jwin_event_mouse_button_press*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].callback.mouse_button_press;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_PRESS, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = JWIN_MOUSE_BUTTON_TYPE_RIGHT,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_press = e });
        if (press)
        {
            press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].param);
        }
    }
    return 0;

    case WM_MBUTTONDOWN:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*press)(
            const jwin_event_mouse_button_press*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].callback.mouse_button_press;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_PRESS, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = JWIN_MOUSE_BUTTON_TYPE_MIDDLE,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_press = e });
        if (press)
        {
            press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].param);
        }
    }
    return 0;

    case WM_XBUTTONDOWN:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*press)(
            const jwin_event_mouse_button_press*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].callback.mouse_button_press;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_PRESS, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = (wparam >> 16) == 2 ? JWIN_MOUSE_BUTTON_TYPE_M4 : JWIN_MOUSE_BUTTON_TYPE_M5,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_press = e });
        if (press)
        {
            press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].param);
        }
    }
    return 0;

    case WM_LBUTTONUP:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*release)(
            const jwin_event_mouse_button_release*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].callback.mouse_button_release;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_RELEASE, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = JWIN_MOUSE_BUTTON_TYPE_LEFT,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_release = e });
        if (release)
        {
            release(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].param);
        }
    }
    return 0;

    case WM_RBUTTONUP:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*release)(
            const jwin_event_mouse_button_release*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].callback.mouse_button_release;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_RELEASE, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = JWIN_MOUSE_BUTTON_TYPE_RIGHT,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_release = e });
        if (release)
        {
            release(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].param);
        }
    }
    return 0;

    case WM_MBUTTONUP:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*release)(
            const jwin_event_mouse_button_release*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].callback.mouse_button_release;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_RELEASE, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = JWIN_MOUSE_BUTTON_TYPE_MIDDLE,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_release = e });
        if (release)
        {
            release(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].param);
        }
    }
    return 0;

    case WM_XBUTTONUP:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*release)(
            const jwin_event_mouse_button_release*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].callback.mouse_button_release;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_RELEASE, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = (wparam >> 16) == 2 ? JWIN_MOUSE_BUTTON_TYPE_M4 : JWIN_MOUSE_BUTTON_TYPE_M5,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_release = e });
        if (release)
        {
            release(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].param);
        }
    }
    return 0;

    case WM_LBUTTONDBLCLK:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*double_press)(
            const jwin_event_mouse_button_double_press*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].callback.mouse_button_double_press;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = JWIN_MOUSE_BUTTON_TYPE_LEFT,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .double_press = e });
        if (double_press)
        {
            double_press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].param);
        }
    }
    return 0;

    case WM_RBUTTONDBLCLK:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*double_press)(
            const jwin_event_mouse_button_double_press*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].callback.mouse_button_double_press;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = JWIN_MOUSE_BUTTON_TYPE_RIGHT,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .double_press = e });
        if (double_press)
        {
            double_press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].param);
        }
    }
    return 0;

    case WM_MBUTTONDBLCLK:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*double_press)(
            const jwin_event_mouse_button_double_press*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].callback.mouse_button_double_press;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = JWIN_MOUSE_BUTTON_TYPE_MIDDLE,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .double_press = e });
        if (double_press)
        {
            double_press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].param);
        }
    }
    return 0;

    case WM_XBUTTONDBLCLK:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        void (*double_press)(
            const jwin_event_mouse_button_double_press*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].callback.mouse_button_double_press;
        //  Regular single click
        jwin_event_mouse_button_press e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS, .context = ctx, .window = win },
                .mods = win32_mods_to_jwin_mods(ctx),
                .x = GET_X_LPARAM(lparam),
                .y = GET_Y_LPARAM(lparam),
                .button = (wparam >> 16) == 2 ? JWIN_MOUSE_BUTTON_TYPE_M5 : JWIN_MOUSE_BUTTON_TYPE_M4,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .double_press = e });
        if (double_press)
        {
            double_press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].param);
        }
    }
    return 0;

    case WM_MOUSEMOVE:
    {
        jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
        jwin_context* const ctx = win->ctx;
        const int x = GET_X_LPARAM(lparam);
        const int y = GET_Y_LPARAM(lparam);
        void (*motion)(
            const jwin_event_mouse_motion*,
            void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_MOVE].callback.mouse_motion;
        jwin_mod_state_type mods =
            (wparam & 0x0008 ? JWIN_MOD_STATE_TYPE_CTRL : 0)
            | (wparam & 0x0004 ? JWIN_MOD_STATE_TYPE_SHIFT : 0);
        jwin_event_mouse_motion e =
        {
                .base = {.type = JWIN_EVENT_TYPE_MOUSE_MOVE, .context = ctx, .window = win },
                .mods = mods,
                .x = x,
                .y = y,
        };
        CALL_EVENT_HOOKS(win, (jwin_event_any) { .mouse_motion = e });
        if (motion)
        {
            motion(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_MOVE].param);
        }
    }
    return 0;


    case WM_MOUSEWHEEL:
        {
            jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
            jwin_context* const ctx = win->ctx;
            void (*press)(
                const jwin_event_mouse_button_press*,
                void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].callback.mouse_button_press;
            //  Regular single click
            jwin_event_mouse_button_press e =
            {
                    .base = {.type = JWIN_EVENT_TYPE_MOUSE_PRESS, .context = ctx, .window = win },
                    .mods = win32_mods_to_jwin_mods(ctx),
                    .x = GET_X_LPARAM(lparam),
                    .y = GET_Y_LPARAM(lparam),
                    .button = GET_WHEEL_DELTA_WPARAM(wparam) > 0 ? JWIN_MOUSE_BUTTON_TYPE_SCROLL_DN : JWIN_MOUSE_BUTTON_TYPE_SCROLL_UP,
            };
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_press = e });
            if (press)
            {
                press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].param);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
            jwin_context* const ctx = win->ctx;
            void (* leave)(
                    const jwin_event_mouse_leave*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_LEAVE].callback.mouse_leave;
            jwin_event_mouse_leave e =
                    {
                            .type = JWIN_EVENT_TYPE_MOUSE_LEAVE,
                            .context = ctx,
                            .window = win,
                    };
            if (leave)
            {
                CALL_EVENT_HOOKS(win, (jwin_event_any) { .mouse_leave = e });
                leave(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_LEAVE].param);
            }
        }
        return 0;

    case WM_SETFOCUS:
        {
            jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
            jwin_context* const ctx = win->ctx;
            void (* gain)(
                    const jwin_event_focus_gain*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_FOCUS_GAIN].callback.focus_gain;
            jwin_event_focus_gain e =
                    {
                            .type = JWIN_EVENT_TYPE_FOCUS_GAIN,
                            .context = ctx,
                            .window = win,
                    };
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .focus_gain = e });
            if (gain)
            {
                gain(&e, win->event_handlers[JWIN_EVENT_TYPE_FOCUS_GAIN].param);
            }
        }
        return 0;

    case WM_KILLFOCUS:
        {
            jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
            jwin_context* const ctx = win->ctx;
            void (* lose)(
                    const jwin_event_focus_gain*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_FOCUS_LOSE].callback.focus_lose;
            jwin_event_focus_gain e =
                    {
                            .type = JWIN_EVENT_TYPE_FOCUS_LOSE,
                            .context = ctx,
                            .window = win,
                    };
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .focus_lose = e });
            if (lose)
            {
                lose(&e, win->event_handlers[JWIN_EVENT_TYPE_FOCUS_LOSE].param);
            }
        }
        return 0;

    case WM_PAINT:
        {
            jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
            jwin_context* const ctx = win->ctx;
            void (* refresh)(
                    const jwin_event_refresh*, void*) = win->event_handlers[JWIN_EVENT_TYPE_REFRESH].callback.refresh;
            jwin_event_refresh e =
                    {
                            .type = JWIN_EVENT_TYPE_REFRESH,
                            .context = ctx,
                            .window = win,
                    };
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .refresh = e });
            if (refresh)
            {
                refresh(&e, win->event_handlers[JWIN_EVENT_TYPE_REFRESH].param);
            }
            RECT r;
            GetClientRect(wnd, &r);
            ValidateRect(wnd, &r);
        }
        return 0;

    case WM_SIZE:
        {
            jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
            jwin_context* const ctx = win->ctx;

            void (*resize)(
                const jwin_event_resize*, void*) = win->event_handlers[JWIN_EVENT_TYPE_RESIZE].callback.resize;
            jwin_event_resize e =
            {
                    .base = {.type = JWIN_EVENT_TYPE_RESIZE, .context = ctx, .window = win },
                    .width = LOWORD(lparam),
                    .height = HIWORD(lparam),
            };
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .resize = e });
            if (resize)
            {
                resize(&e, win->event_handlers[JWIN_EVENT_TYPE_RESIZE].param);
            }

            win->width = e.width;
            win->height = e.height;
        }
        return 0;

    case WM_MOVE:
        {
            jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
            jwin_context* const ctx = win->ctx;
            //  Check position
            int x = GET_X_LPARAM(lparam), y = GET_Y_LPARAM(lparam);
            if (x != win->x || y != win->y)
            {
                void (* move)(const jwin_event_move*, void*) = win->event_handlers[JWIN_EVENT_TYPE_MOVE].callback.move;
                jwin_event_move e =
                        {
                                .base = { .type = JWIN_EVENT_TYPE_MOVE, .context = ctx, .window = win },
                                .x = x,
                                .y = y,
                        };
                CALL_EVENT_HOOKS(win, (jwin_event_any) { .move = e });
                if (move)
                {
                    move(&e, win->event_handlers[JWIN_EVENT_TYPE_MOVE].param);
                }

                win->x = x;
                win->y = y;
            }
        }
        return 0;

    case WM_CLOSE:
        {
            jwin_window* const win = (jwin_window*)GetWindowLongPtr(wnd, GWLP_USERDATA);
            jwin_context* const ctx = win->ctx;
            //  Window was asked to close
            int (*close)(
                const jwin_event_close*, void*) = win->event_handlers[JWIN_EVENT_TYPE_CLOSE].callback.close;
            const jwin_event_close ce =
            {
                    .type = JWIN_EVENT_TYPE_CLOSE,
                    .context = ctx,
                    .window = win,
            };
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .close = ce });
            if (!close || close(&ce, win->event_handlers[JWIN_EVENT_TYPE_CLOSE].param))
            {
                //  Window should close
                jwin_window_destroy(win);
            }
        }
        return 0;

    case WM_USER:
        {
            jwin_event_custom* const e = (jwin_event_custom* const)wparam;
            assert(e->base.type >= JWIN_EVENT_TYPE_CUSTOM);
            jwin_window* const win = e->base.window;
            jwin_context* const ctx = win->ctx;
            assert(win->hwnd == wnd);
            void (*custom)(const jwin_event_custom * e, void* param) = win->event_handlers[JWIN_EVENT_TYPE_CUSTOM].callback.custom;
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .custom = *e });
            if (custom)
            {
                custom(e, win->event_handlers[JWIN_EVENT_TYPE_CUSTOM].param);
            }

            ctx->allocator_callbacks.free(ctx->allocator_callbacks.state, e);
        }

    default: return DefWindowProcW(wnd, type, wparam, lparam);
    }
}

void jwin_context_set_event_hook(jwin_context* ctx, void (* hook)(const jwin_event_any*, void*), void* param)
{
    ctx->event_hook = hook;
    ctx->event_param = param;
}

