//
// Created by jan on 6.8.2023.
//

#include <string.h>
#include <assert.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/timeb.h>
#include "context_xlib.h"
#include "../internal.h"
#include "../context.h"

#include "window_xlib.h"

static int x_error_code;

static int x_error_handler(Display* dpy, XErrorEvent* e)
{
    (void) dpy;
    x_error_code = e->error_code;
    return 0;
}

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
    Display* dpy = NULL;


    ctx = ALLOC(sizeof(*ctx));
    if (!ctx)
    {
        return JWIN_RESULT_BAD_ALLOC;
    }
    ctx->error_callbacks = *info.error_callbacks;

    //  Open X11 connection
    dpy = XOpenDisplay(NULL);
    if (!dpy)
    {
        res = JWIN_RESULT_NO_PLATFORM;
        REPORT_ERROR(ctx, "Could not open Xlib display");
        goto failed;
    }
    ctx->dpy = dpy;
    ctx->allocator_callbacks = *info.allocator_callbacks;

    //  Check for keyboard extension and initialize it
    int xkb_op_code, xkb_event_code, xkb_error_code;
    int xkb_maj = XkbMajorVersion, xkb_min = XkbMinorVersion;
    if (XkbQueryExtension(dpy, &xkb_op_code, &xkb_event_code, &xkb_error_code, &xkb_maj, &xkb_min) == False)
    {
        REPORT_ERROR(ctx, "XKB extension was not supported");
        XCloseDisplay(dpy);
        res = JWIN_RESULT_NO_PLATFORM;
        goto failed;
    }
    ctx->xkb_event_code = xkb_event_code;

    if (XkbSetDetectableAutoRepeat(dpy, True, &ctx->autorepeat_support))
    {
        //  Maybe some shit here?
    }
    XkbStateRec kbd_state;
    if (XkbGetState(dpy, XkbUseCoreKbd, &kbd_state))
    {
        ctx->group = (unsigned int) kbd_state.group;
    }

    //  Get the XCB connection
    ctx->xcb_connection = XGetXCBConnection(dpy);

    {
        memset(ctx->keymap, 0, sizeof(ctx->keymap));
        memset(ctx->reverse_keymap, 0, sizeof(ctx->reverse_keymap));
        memset(ctx->key_state, JWIN_KEY_STATE_UP, sizeof(*ctx->key_state));
        XkbDescPtr desc = XkbGetMap(dpy, 0, XkbUseCoreKbd);
        if (!desc)
        {
            res = JWIN_RESULT_NO_PLATFORM;
            REPORT_ERROR(ctx, "Could not get Xkb map");
            XCloseDisplay(dpy);
            goto failed;
        }
        if (XkbGetNames(dpy, XkbKeyNamesMask | XkbKeyAliasesMask, desc) != Success)
        {
            REPORT_ERROR(ctx, "Could not get Xkb map names");
            XkbFreeKeyboard(desc, 0, True);
            res = JWIN_RESULT_NO_PLATFORM;
            XCloseDisplay(dpy);
            goto failed;
        }


        const unsigned scan_min = desc->min_key_code, scan_max = desc->max_key_code;
        assert(scan_max < JWIN_KEY_LAST);

        //  Use XKB to map as much as possible
        struct
        {
            int found;
            jwin_keycode key;
            KeySym backup_ks;
            const char name[XkbKeyNameLength];
        } key_table[] =
                {

                        { .key = JWIN_KEY_UP, .name = "UP", .backup_ks = XK_Up },
                        { .key = JWIN_KEY_LEFT, .name = "LEFT", .backup_ks = XK_Left },
                        { .key = JWIN_KEY_DOWN, .name = "DOWN", .backup_ks = XK_Down },
                        { .key = JWIN_KEY_RIGHT, .name = "RGHT", .backup_ks = XK_Right },
                        { .key = JWIN_KEY_Q, .name = "AD01", .backup_ks = XK_q },
                        { .key = JWIN_KEY_W, .name = "AD02", .backup_ks = XK_w },
                        { .key = JWIN_KEY_E, .name = "AD03", .backup_ks = XK_e },
                        { .key = JWIN_KEY_R, .name = "AD04", .backup_ks = XK_r },
                        { .key = JWIN_KEY_T, .name = "AD05", .backup_ks = XK_t },
                        { .key = JWIN_KEY_Y, .name = "AD06", .backup_ks = XK_y },
                        { .key = JWIN_KEY_U, .name = "AD07", .backup_ks = XK_u },
                        { .key = JWIN_KEY_I, .name = "AD08", .backup_ks = XK_i },
                        { .key = JWIN_KEY_O, .name = "AD09", .backup_ks = XK_o },
                        { .key = JWIN_KEY_P, .name = "AD10", .backup_ks = XK_p },
                        { .key = JWIN_KEY_LBRACKET, .name = "AD11", .backup_ks = XK_braceleft },
                        { .key = JWIN_KEY_RBRACKET, .name = "AD12", .backup_ks = XK_braceright },
                        { .key = JWIN_KEY_A, .name = "AC01", .backup_ks = XK_a },
                        { .key = JWIN_KEY_S, .name = "AC02", .backup_ks = XK_s },
                        { .key = JWIN_KEY_D, .name = "AC03", .backup_ks = XK_d },
                        { .key = JWIN_KEY_F, .name = "AC04", .backup_ks = XK_f },
                        { .key = JWIN_KEY_G, .name = "AC05", .backup_ks = XK_g },
                        { .key = JWIN_KEY_H, .name = "AC06", .backup_ks = XK_h },
                        { .key = JWIN_KEY_J, .name = "AC07", .backup_ks = XK_j },
                        { .key = JWIN_KEY_K, .name = "AC08", .backup_ks = XK_k },
                        { .key = JWIN_KEY_L, .name = "AC09", .backup_ks = XK_l },
                        { .key = JWIN_KEY_SEMICOL, .name = "AC10", .backup_ks = XK_semicolon },
                        { .key = JWIN_KEY_APPOSTROPHE, .name = "AC11", .backup_ks = XK_apostrophe },
                        { .key = JWIN_KEY_BACKSLASH, .name = "AC12", .backup_ks = XK_backslash },
                        { .key = JWIN_KEY_Z, .name = "AB01", .backup_ks = XK_z },
                        { .key = JWIN_KEY_X, .name = "AB02", .backup_ks = XK_x },
                        { .key = JWIN_KEY_C, .name = "AB03", .backup_ks = XK_c },
                        { .key = JWIN_KEY_V, .name = "AB04", .backup_ks = XK_v },
                        { .key = JWIN_KEY_B, .name = "AB05", .backup_ks = XK_b },
                        { .key = JWIN_KEY_N, .name = "AB06", .backup_ks = XK_n },
                        { .key = JWIN_KEY_M, .name = "AB07", .backup_ks = XK_m },
                        { .key = JWIN_KEY_COMMA, .name = "AB08", .backup_ks = XK_comma },
                        { .key = JWIN_KEY_PERIOD, .name = "AB09", .backup_ks = XK_period },
                        { .key = JWIN_KEY_SLASH, .name = "AB10", .backup_ks = XK_slash },
                        { .key = JWIN_KEY_GRAVE, .name = "TLDE", .backup_ks = XK_grave },
                        { .key = JWIN_KEY_1, .name = "AE01", .backup_ks = XK_1 },
                        { .key = JWIN_KEY_2, .name = "AE02", .backup_ks = XK_2 },
                        { .key = JWIN_KEY_3, .name = "AE03", .backup_ks = XK_3 },
                        { .key = JWIN_KEY_4, .name = "AE04", .backup_ks = XK_4 },
                        { .key = JWIN_KEY_5, .name = "AE05", .backup_ks = XK_5 },
                        { .key = JWIN_KEY_6, .name = "AE06", .backup_ks = XK_6 },
                        { .key = JWIN_KEY_7, .name = "AE07", .backup_ks = XK_7 },
                        { .key = JWIN_KEY_8, .name = "AE08", .backup_ks = XK_8 },
                        { .key = JWIN_KEY_9, .name = "AE09", .backup_ks = XK_9 },
                        { .key = JWIN_KEY_0, .name = "AE10", .backup_ks = XK_0 },
                        { .key = JWIN_KEY_MINUS, .name = "AE11", .backup_ks = XK_minus },
                        { .key = JWIN_KEY_EQUAL, .name = "AE12", .backup_ks = XK_equal },
                        { .key = JWIN_KEY_BACKSPACE, .name = "BKSP", .backup_ks = XK_BackSpace },
                        { .key = JWIN_KEY_TAB, .name = "TAB", .backup_ks = XK_Tab },
                        { .key = JWIN_KEY_CAPSLOCK, .name = "CAPS", .backup_ks = XK_Caps_Lock },
                        { .key = JWIN_KEY_LSHIFT, .name = "LFSH", .backup_ks = XK_Shift_L },
                        { .key = JWIN_KEY_RSHIFT, .name = "RTSH", .backup_ks = XK_Shift_R },
                        { .key = JWIN_KEY_LCTRL, .name = "LCTL", .backup_ks = XK_Control_L },
                        { .key = JWIN_KEY_RCTRL, .name = "RCTL", .backup_ks = XK_Control_R },
                        { .key = JWIN_KEY_MENU, .name = "MENU", .backup_ks = XK_Menu },
                        { .key = JWIN_KEY_RSUPER, .name = "RWIN", .backup_ks = XK_Super_R },
                        { .key = JWIN_KEY_LSUPER, .name = "LWIN", .backup_ks = XK_Super_L },
                        { .key = JWIN_KEY_LALT, .name = "LALT", .backup_ks = XK_Alt_L },
                        { .key = JWIN_KEY_RALT, .name = "RALT", .backup_ks = XK_Alt_R },
                        { .key = JWIN_KEY_RETURN, .name = "RTRN", .backup_ks = XK_Return },
                        { .key = JWIN_KEY_INSERT, .name = "INS", .backup_ks = XK_Insert },
                        { .key = JWIN_KEY_HOME, .name = "HOME", .backup_ks = XK_Home },
                        { .key = JWIN_KEY_END, .name = "END", .backup_ks = XK_End },
                        { .key = JWIN_KEY_DELETE, .name = "DELE", .backup_ks = XK_Delete },
                        { .key = JWIN_KEY_PGUP, .name = "PGUP", .backup_ks = XK_Page_Up },
                        { .key = JWIN_KEY_PGDN, .name = "PGDN", .backup_ks = XK_Page_Down },
                        { .key = JWIN_KEY_PRINTSCR, .name = "PRSC", .backup_ks = XK_Print },
                        { .key = JWIN_KEY_SCRLOCK, .name = "SCLK", .backup_ks = XK_Scroll_Lock },
                        { .key = JWIN_KEY_PAUSE, .name = "PAUS", .backup_ks = XK_Pause },
                        { .key = JWIN_KEY_NUMLOCK, .name = "NMLK", .backup_ks = XK_Num_Lock },
                        { .key = JWIN_KEY_NUM_SEP, .name = "KPDL", .backup_ks = XK_KP_Separator },
                        { .key = JWIN_KEY_NUM_0, .name = "KP0", .backup_ks = XK_KP_0 },
                        { .key = JWIN_KEY_NUM_1, .name = "KP1", .backup_ks = XK_KP_1 },
                        { .key = JWIN_KEY_NUM_2, .name = "KP2", .backup_ks = XK_KP_2 },
                        { .key = JWIN_KEY_NUM_3, .name = "KP3", .backup_ks = XK_KP_3 },
                        { .key = JWIN_KEY_NUM_4, .name = "KP4", .backup_ks = XK_KP_4 },
                        { .key = JWIN_KEY_NUM_5, .name = "KP5", .backup_ks = XK_KP_5 },
                        { .key = JWIN_KEY_NUM_6, .name = "KP6", .backup_ks = XK_KP_6 },
                        { .key = JWIN_KEY_NUM_7, .name = "KP7", .backup_ks = XK_KP_7 },
                        { .key = JWIN_KEY_NUM_8, .name = "KP8", .backup_ks = XK_KP_8 },
                        { .key = JWIN_KEY_NUM_9, .name = "KP9", .backup_ks = XK_KP_9 },
                        { .key = JWIN_KEY_NUM_DIV, .name = "KPDV", .backup_ks = XK_KP_Divide },
                        { .key = JWIN_KEY_NUM_MUL, .name = "KPMU", .backup_ks = XK_KP_Multiply },
                        { .key = JWIN_KEY_NUM_SUB, .name = "KPSU", .backup_ks = XK_KP_Subtract },
                        { .key = JWIN_KEY_NUM_ADD, .name = "KPAD", .backup_ks = XK_KP_Add },
                        { .key = JWIN_KEY_NUM_ENTER, .name = "KPEN", .backup_ks = XK_KP_Enter },
                        { .key = JWIN_KEY_F1, .name = "FK01", .backup_ks = XK_F1 },
                        { .key = JWIN_KEY_F2, .name = "FK02", .backup_ks = XK_F2 },
                        { .key = JWIN_KEY_F3, .name = "FK03", .backup_ks = XK_F3 },
                        { .key = JWIN_KEY_F4, .name = "FK04", .backup_ks = XK_F4 },
                        { .key = JWIN_KEY_F5, .name = "FK05", .backup_ks = XK_F5 },
                        { .key = JWIN_KEY_F6, .name = "FK06", .backup_ks = XK_F6 },
                        { .key = JWIN_KEY_F7, .name = "FK07", .backup_ks = XK_F7 },
                        { .key = JWIN_KEY_F8, .name = "FK08", .backup_ks = XK_F8 },
                        { .key = JWIN_KEY_F9, .name = "FK09", .backup_ks = XK_F9 },
                        { .key = JWIN_KEY_F10, .name = "FK10", .backup_ks = XK_F10 },
                        { .key = JWIN_KEY_F11, .name = "FK11", .backup_ks = XK_F11 },
                        { .key = JWIN_KEY_F12, .name = "FK12", .backup_ks = XK_F12 },
                        { .key = JWIN_KEY_ESC, .name = "ESC", .backup_ks = XK_Escape },
                        { .key = JWIN_KEY_SPACE, .name = "SPCE", .backup_ks = XK_space },
                };

        for (unsigned code = scan_min; code <= scan_max; ++code)
        {
            jwin_keycode key = JWIN_KEY_NONE;

            //  Try and find the matching name in the key_table
            for (unsigned i = 0; i < sizeof(key_table) / sizeof(*key_table); ++i)
            {
                if (!key_table[i].found &&
                    strncmp(desc->names->keys[code].name, key_table[i].name, XkbKeyNameLength) == 0)
                {
                    key = key_table[i].key;
                    key_table[i].found = 1;
                    break;
                }
            }

            if (key == JWIN_KEY_NONE)
            {
                //  None of the names matched, check key aliases
                for (unsigned j = 0; j < desc->names->num_key_aliases; ++j)
                {
                    if (strncmp(desc->names->key_aliases[j].real, desc->names->keys[code].name, XkbKeyNameLength) != 0)
                    {
                        //  Alias at index "j" does not match the key at index "code"
                        continue;
                    }
                    for (unsigned i = 0; i < sizeof(key_table) / sizeof(*key_table); ++i)
                    {
                        if (!key_table[i].found &&
                            strncmp(desc->names->key_aliases[j].alias, key_table[i].name, XkbKeyNameLength) == 0)
                        {
                            key = key_table[i].key;
                            key_table[i].found = 1;
                            break;
                        }
                    }
                }
            }

            ctx->keymap[code] = key;
        }
        XkbFreeNames(desc, XkbKeyNamesMask | XkbKeyAliasesMask, True);
        XkbFreeKeyboard(desc, 0, True);


        //  Check if it is possible to map back some scancodes "backwards"
        int ks_per_keycode;
        KeySym* const key_syms = XGetKeyboardMapping(dpy, scan_min, (int) (scan_max + 1u - scan_min), &ks_per_keycode);
        if (!key_syms)
        {
            REPORT_ERROR(ctx, "Could not get XLib keyboard map");
            res = JWIN_RESULT_NO_PLATFORM;
            XCloseDisplay(dpy);
            goto failed;
        }

        for (unsigned code = scan_min; code <= scan_max; ++code)
        {
            jwin_keycode key = ctx->keymap[code];
            if (key == 0)
            {
                for (unsigned i = 0; i < sizeof(key_table) / sizeof(*key_table); ++i)
                {
                    for (unsigned j = 0; j < (unsigned) ks_per_keycode; ++j)
                    {
                        if (!key_table[i].found && key_table[i].backup_ks == key_syms[ks_per_keycode * i + j])
                        {
                            key = key_table[i].key;
                            key_table[i].found = 2;
                            break;
                        }
                    }
                }

                ctx->keymap[code] = key;
            }

            //  Add reverse lookup table
            ctx->reverse_keymap[key] = code;
        }

        XFree(key_syms);
    }


    //  Load xlib atoms
    //      String format
    ctx->atoms.utf8 = XInternAtom(dpy, "UTF8_STRING", False);
    ctx->atoms.null = XInternAtom(dpy, "NULL", False);

    //  ICCCM atoms for clipboard
    ctx->atoms.targets = XInternAtom(dpy, "TARGETS", False);
    ctx->atoms.multiple = XInternAtom(dpy, "MULTIPLE", False);
    ctx->atoms.primary = XInternAtom(dpy, "PRIMARY", False);
    ctx->atoms.incr = XInternAtom(dpy, "INCR", False);
    ctx->atoms.clipboard = XInternAtom(dpy, "CLIPBOARD", False);

    //  Drag and drop atoms
    ctx->atoms.dnd_aware = XInternAtom(dpy, "XdndAware", False);
    ctx->atoms.dnd_enter = XInternAtom(dpy, "XdndEnter", False);
    ctx->atoms.dnd_position = XInternAtom(dpy, "XdndPosition", False);
    ctx->atoms.dnd_status = XInternAtom(dpy, "XdndStatus", False);
    ctx->atoms.dnd_action_copy = XInternAtom(dpy, "XdndActionCopy", False);
    ctx->atoms.dnd_drop = XInternAtom(dpy, "XdndDrop", False);
    ctx->atoms.dnd_finished = XInternAtom(dpy, "XdndFinished", False);
    ctx->atoms.dnd_selection = XInternAtom(dpy, "XdndSelection", False);
    ctx->atoms.dnd_type_list = XInternAtom(dpy, "XdndTypeList", False);
    ctx->atoms.dnd_text_uri_list = XInternAtom(dpy, "text/uri-list", False);


    //  Window property atoms
    ctx->atoms.wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    ctx->atoms.wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
    ctx->atoms.net_supported = XInternAtom(dpy, "_NET_SUPPORTED", False);
    ctx->atoms.net_supporting_wm_check = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", False);
    ctx->atoms.net_wm_icon = XInternAtom(dpy, "_NET_WM_ICON", False);
    ctx->atoms.net_wm_ping = XInternAtom(dpy, "_NET_WM_PING", False);
    ctx->atoms.net_wm_pid = XInternAtom(dpy, "_NET_WM_PID", False);
    ctx->atoms.net_wm_name = XInternAtom(dpy, "_NET_WM_NAME", False);
    ctx->atoms.net_wm_icon_name = XInternAtom(dpy, "_NET_WM_ICON_NAME", False);
    ctx->atoms.net_wm_bypass_compositor = XInternAtom(dpy, "_NET_WM_BYPASS_COMPOSITOR", False);
    ctx->atoms.net_wm_window_opacity = XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False);
    ctx->atoms.motif_wm_hints = XInternAtom(dpy, "_MOTIF_WM_HINTS", False);


    //  Load some data for quick access
    ctx->screen = DefaultScreen(dpy);
    ctx->root = RootWindow(dpy, ctx->screen);

    //  Open FDs for notification pipe
    if (pipe(ctx->notification_pipes) < 0)
    {
        REPORT_ERROR(ctx, "Could not open notification pipe, reason: %s", strerror(errno));
        res = JWIN_RESULT_NO_PLATFORM;
        XCloseDisplay(dpy);
        goto failed;
    }

    //  Input pipe
    int desc_flags = fcntl(ctx->notification_pipes[0], F_GETFD, 0);
    int stat_flags = fcntl(ctx->notification_pipes[0], F_GETFL, 0);
    if (desc_flags == -1 || stat_flags == -1 ||
        fcntl(ctx->notification_pipes[0], F_SETFL, stat_flags | O_NONBLOCK) == -1
        || fcntl(ctx->notification_pipes[0], F_SETFD, desc_flags | FD_CLOEXEC))
    {
        REPORT_ERROR(ctx, "Could not set pipe properties, reason: %s", strerror(errno));
        //  Failed setting the appropriate flags
        close(ctx->notification_pipes[0]);
        close(ctx->notification_pipes[1]);
        res = JWIN_RESULT_NO_PLATFORM;
        XCloseDisplay(dpy);
        goto failed;
    }
    //  Output pipe
    desc_flags = fcntl(ctx->notification_pipes[1], F_GETFD, 0);
    stat_flags = fcntl(ctx->notification_pipes[1], F_GETFL, 0);
    if (desc_flags == -1 || stat_flags == -1 ||
        fcntl(ctx->notification_pipes[1], F_SETFL, stat_flags | O_NONBLOCK) == -1
        || fcntl(ctx->notification_pipes[1], F_SETFD, desc_flags | FD_CLOEXEC))
    {
        REPORT_ERROR(ctx, "Could not set pipe properties, reason: %s", strerror(errno));
        //  Failed setting the appropriate flags
        close(ctx->notification_pipes[0]);
        close(ctx->notification_pipes[1]);
        res = JWIN_RESULT_NO_PLATFORM;
        XCloseDisplay(dpy);
        goto failed;
    }


    //  Create input method
    if (XSupportsLocale() && ctx->atoms.utf8)
    {
        XSetLocaleModifiers("");
    }

    ctx->input_method = XOpenIM(dpy, NULL, NULL, NULL);
    if (!ctx->input_method)
    {
        REPORT_ERROR(ctx, "Could not set open XLib input method");
        //  Failed creating the input method
        close(ctx->notification_pipes[0]);
        close(ctx->notification_pipes[1]);
        res = JWIN_RESULT_NO_PLATFORM;
        XCloseDisplay(dpy);
        goto failed;
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
    close(ctx->notification_pipes[0]);
    close(ctx->notification_pipes[1]);
    XCloseIM(ctx->input_method);
    XCloseDisplay(ctx->dpy);
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

unsigned find_window_index(const jwin_context* ctx, Window wnd)
{
    unsigned pos = 0, len = ctx->window_count, step = len - len / 2;
    while (step > 8)
    {
        if (ctx->window_array[pos + step]->hwnd > wnd)
        {
            len = step;
        }
        else
        {
            pos += step;
            len -= step;
        }
        step = len - len / 2;
    }
    for (unsigned i = pos; i < pos + len; ++i)
    {
        if (ctx->window_array[i]->hwnd > wnd)
        {
            return i - 1;
        }
    }
    if (ctx->window_array[pos + len - 1]->hwnd == wnd)
    {
        return pos + len - 1;
    }
    return pos + len;
}

jwin_window* INTERNAL_find_window_from_xlib_handle(const jwin_context* ctx, Window wnd)
{
    if (ctx->window_count == 0)
    {
        return NULL;
    }
    //  ctx->window_array is sorted, so use binary search
    unsigned pos = find_window_index(ctx, wnd);
//    assert(ctx->window_array[pos]->hwnd == wnd);
    return ctx->window_array[pos]->hwnd == wnd ? ctx->window_array[pos] : NULL;
}

jwin_result jwin_context_handle_event(jwin_context* ctx)
{
    jwin_event_custom event_custom;

    long bytes_read = read(ctx->notification_pipes[PIPE_OUTPUT], &event_custom, sizeof(event_custom));
    if (bytes_read == sizeof(event_custom))
    {
        jwin_window* win = event_custom.base.window;
        assert(event_custom.base.context == ctx);
        assert(event_custom.base.type >= JWIN_EVENT_TYPE_CUSTOM);
        void
        (* callback)(const jwin_event_custom*, void*) = win->event_handlers[JWIN_EVENT_TYPE_CUSTOM].callback.custom;
        if (callback)
        {
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .custom = event_custom });
            callback(&event_custom, win->event_handlers[JWIN_EVENT_TYPE_CUSTOM].param);
            goto end;
        }
    }

    //  Process the XOrg events
    XPending(ctx->dpy);

    if (QLength(ctx->dpy))
    {
        XEvent e;
        XNextEvent(ctx->dpy, &e);
        //  Find what window it came from
        jwin_window* last_wnd = INTERNAL_find_window_from_xlib_handle(ctx, e.xany.window);
        assert(last_wnd->hwnd == e.xany.window);

        //  Process the event
        INTERNAL_process_xlib_event(ctx, last_wnd, &e);
    }

    end:
    XFlush(ctx->dpy);

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
    //  See if any custom events were sent
    jwin_event_custom event_custom;
    for (;;)
    {
        long bytes_read = read(ctx->notification_pipes[PIPE_OUTPUT], &event_custom, sizeof(event_custom));
        if (bytes_read == sizeof(event_custom))
        {
            jwin_window* win = event_custom.base.window;
            assert(event_custom.base.context == ctx);
            assert(event_custom.base.type >= JWIN_EVENT_TYPE_CUSTOM);
            void
            (* callback)(const jwin_event_custom*, void*) = win->event_handlers[JWIN_EVENT_TYPE_CUSTOM].callback.custom;
            if (callback)
            {
                CALL_EVENT_HOOKS(win, (jwin_event_any) { .custom = event_custom });
                callback(&event_custom, win->event_handlers[JWIN_EVENT_TYPE_CUSTOM].param);
            }
        }
        else if (!bytes_read || errno == EAGAIN || errno == EWOULDBLOCK)
        {
            break;
        }
        else
        {
            REPORT_ERROR(ctx, "read returned %ld instead of %zu, errno is %d (%s)", bytes_read, sizeof(event_custom),
                         errno,
                         strerror(errno));
            break;
        }
    }

    //  Process the XOrg events
    XPending(ctx->dpy);

    while (QLength(ctx->dpy))
    {
        XEvent e;
        XNextEvent(ctx->dpy, &e);
        //  Find what window it came from

        jwin_window* window = INTERNAL_find_window_from_xlib_handle(ctx, e.xany.window);
        if (!window || window->hwnd != e.xany.window)
        {
            //  Window was already destroyed
            continue;
        }

        //  Process the event
        INTERNAL_process_xlib_event(ctx, window, &e);
    }

    XFlush(ctx->dpy);

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

    //  Find the place where to insert the pointer in the array
    if (ctx->window_count == 0)
    {
        //  Only one
        ctx->window_array[0] = win;
        ctx->window_count = 1;
    }
    else
    {
        unsigned pos;
        if (ctx->window_count < 8)
        {
            //  Small number, so just do it really simply
            for (pos = 0; pos < ctx->window_count; ++pos)
            {
                if (ctx->window_array[pos]->hwnd > win->hwnd)
                {
                    break;
                }
            }
        }
        else
        {
            pos = find_window_index(ctx, win->hwnd);
        }

        memmove(
                ctx->window_array + pos + 1, ctx->window_array + pos,
                sizeof(jwin_window*) * (ctx->window_count - pos));
        ctx->window_array[pos] = win;
        ctx->window_count += 1;
    }

    return JWIN_RESULT_SUCCESS;
}

jwin_result INTERNAL_remove_window_from_context(jwin_context* ctx, const jwin_window* win)
{
    const unsigned pos = find_window_index(ctx, win->hwnd);

    //  Window was not in the context's window array
    if (ctx->window_array[pos]->hwnd != win->hwnd)
    {
        REPORT_ERROR(ctx,
                     "Window was not found in the context (element at pos = %u had a handle %08lX, but %08lX was needed",
                     pos, ctx->window_array[pos]->hwnd, win->hwnd);
        return JWIN_RESULT_BAD_CONTEXT;
    }

    memmove(
            ctx->window_array + pos, ctx->window_array + pos + 1,
            sizeof(jwin_window*) * (ctx->window_count - pos - 1));
    ctx->window_array[ctx->window_count] = (void*) 0xCCCCCCCCCCCCCCCC;
    ctx->window_count -= 1;


    return JWIN_RESULT_SUCCESS;
}

static long get_system_milli(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return 1000 * (ts.tv_sec) + (ts.tv_nsec / 1000000);
}

jwin_result INTERNAL_wait_for_any(const jwin_context* ctx, int ms_timeout)
{
    nfds_t count = 2;
    struct pollfd fds[2] =
            {
                    { .fd = ConnectionNumber(ctx->dpy), .events = POLLIN },
                    { .fd = ctx->notification_pipes[PIPE_OUTPUT], .events = POLLIN }
            };

    while (!XPending(ctx->dpy))
    {
        const long base = get_system_milli();
        const int res = poll(fds, count, ms_timeout);
        int prev_errno = errno;
        if (res > 0)
        {
            return JWIN_RESULT_SUCCESS;
        }
        else
        {
            if (prev_errno != EINTR && prev_errno != EAGAIN)
            {
                assert(res == -1);
                REPORT_ERROR(ctx, "Error occurred with a call poll( &{ %d, %d }, %lu, %d) -> %s",
                             ConnectionNumber(ctx->dpy),
                             ctx->notification_pipes[PIPE_OUTPUT], count, ms_timeout, strerror(prev_errno));
                return JWIN_RESULT_BAD_WAIT;
            }
            else if (ms_timeout > 0)
            {
                int dt = (int) (get_system_milli() - base);
                if (dt > ms_timeout)
                {
                    return JWIN_RESULT_TIMED_OUT;
                }
                ms_timeout -= dt;
            }
        }
    }

    return JWIN_RESULT_SUCCESS;
}

static inline jwin_keycode xlib_keycode_to_jwin_keycode(const jwin_context* ctx, unsigned keycode)
{
    if (keycode >= JWIN_KEY_LAST)
    {
        return JWIN_KEY_NONE;
    }
    return ctx->keymap[keycode];
}

static inline jwin_mod_state_type xlib_mods_to_jwin_mods(unsigned state)
{
    jwin_mod_state_type mods =
            (state & ShiftMask ? JWIN_MOD_STATE_TYPE_SHIFT : 0) |
            (state & ControlMask ? JWIN_MOD_STATE_TYPE_CTRL : 0) |
            (state & Mod1Mask ? JWIN_MOD_STATE_TYPE_ALT : 0) |
            (state & Mod4Mask ? JWIN_MOD_STATE_TYPE_SUPER : 0) |
            (state & LockMask ? JWIN_MOD_STATE_TYPE_CAPSLOCK : 0) |
            (state & Mod2Mask ? JWIN_MOD_STATE_TYPE_NUMLOCK : 0);
    return mods;
}

static inline jwin_mouse_button_type xlib_button_to_jwin_button(unsigned button)
{
    switch (button)
    {
    case Button1: return JWIN_MOUSE_BUTTON_TYPE_LEFT;
    case Button2: return JWIN_MOUSE_BUTTON_TYPE_MIDDLE;
    case Button3: return JWIN_MOUSE_BUTTON_TYPE_RIGHT;
    case Button4: return JWIN_MOUSE_BUTTON_TYPE_SCROLL_UP;
    case Button5: return JWIN_MOUSE_BUTTON_TYPE_SCROLL_DN;
    case 8: return JWIN_MOUSE_BUTTON_TYPE_M4;
    case 9: return JWIN_MOUSE_BUTTON_TYPE_M5;
    default: return JWIN_MOUSE_BUTTON_TYPE_NONE;
    }
}

static inline int should_process_event(const jwin_window* win, const jwin_event_type type)
{
    return win->event_handlers[type].callback.any != NULL || win->event_hook != NULL || win->ctx->event_hook != NULL;
}

jwin_result INTERNAL_process_xlib_event(jwin_context* ctx, jwin_window* win, XEvent* event)
{
    unsigned saved_keycode = (event->type == KeyPress || event->type == KeyRelease) ? event->xkey.keycode : 0;
    Bool filtered = XFilterEvent(event, None);

    if (event->type == ctx->xkb_event_code + XkbEventCode)
    {
        XkbEvent* kbe = (XkbEvent*) event;
        if (kbe->any.xkb_type == XkbStateNotify && kbe->state.changed & XkbGroupStateMask)
        {
            ctx->group = kbe->state.group;
        }
        goto end;
    }

    switch (event->type)
    {
    case ReparentNotify:
    {
        win->parent_hwnd = event->xreparent.parent;
    }
        break;

    case KeyPress:
        if (saved_keycode < JWIN_KEY_LAST)
        {
            const int repeated = ctx->key_state[saved_keycode] == JWIN_KEY_STATE_DOWN;
            ctx->key_state[saved_keycode] = JWIN_KEY_STATE_DOWN;

            void (* kp_callback)(
                    const jwin_event_key_press*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_KEY_PRESS].callback.key_press;
            const jwin_keycode key = xlib_keycode_to_jwin_keycode(ctx, saved_keycode);
            const jwin_mod_state_type mods = xlib_mods_to_jwin_mods(event->xkey.state);

            jwin_event_key_press ev =
                    {
                            .base = { .type = JWIN_EVENT_TYPE_KEY_PRESS, .context = ctx, .window = win },
                            .keycode = key,
                            .mods = mods,
                            .repeated = repeated,
                    };
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .key_press = ev });
            if (kp_callback)
            {
                kp_callback(&ev, win->event_handlers[JWIN_EVENT_TYPE_KEY_PRESS].param);
            }

            void (* kc_callback)(
                    const jwin_event_key_char*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_KEY_CHARACTER].callback.key_char;
            if (!filtered && kc_callback)
            {
                int count;
                Status s;
                char static_buffer[128];
                char* ptr = NULL;
                count = Xutf8LookupString(win->ic, &event->xkey, static_buffer, sizeof(static_buffer) - 1, NULL, &s);
                if (s == XBufferOverflow)
                {
                    ptr = ctx->allocator_callbacks.alloc(ctx->allocator_callbacks.state, count + 1);
                    if (ptr)
                    {
                        count = Xutf8LookupString(win->ic, &event->xkey, ptr, count, NULL, &s);
                    }
                    else
                    {
                        REPORT_ERROR(ctx, "Could not allocate %zu + 1 bytes for buffer needed by Xutf8LookupString",
                                     (size_t) count);
                    }
                }
                else
                {
                    ptr = static_buffer;
                }
                ptr[count] = 0;

                if (s == XLookupChars || s == XLookupBoth)
                {
                    //  Assuming that Xutf8LookupString always returns vaid UTF-8, it is quite simple to determine
                    //  how many bytes there are per codepoint
                    char buffer[5] = { 0 };
                    char* out = buffer;
                    for (const char* pos = ptr; *pos; ++pos)
                    {
                        assert(out < buffer + sizeof(buffer));
                        if ((*pos & (0xC0)) == 0x80)
                        {
                            //  Continuation of previous char
                            *out = *pos;
                            ++out;
                        }
                        else
                        {
                            if (out != buffer)
                            {
                                *out = 0;
                                jwin_event_key_char e =
                                        {
                                                .base = { .type = JWIN_EVENT_TYPE_KEY_CHARACTER, .window = win, .context = ctx },
                                                .utf8 = buffer,
                                        };
                                CALL_EVENT_HOOKS(win, (jwin_event_any) { .key_char = e });
                                if (kc_callback)
                                {
                                    kc_callback(&e, win->event_handlers[JWIN_EVENT_TYPE_KEY_CHARACTER].param);
                                }
                            }
                            out = buffer;
                            *out = *pos;
                            ++out;
                        }
                    }

                    *out = 0;
                    jwin_event_key_char e =
                            {
                                    .base = { .type = JWIN_EVENT_TYPE_KEY_CHARACTER, .window = win, .context = ctx },
                                    .utf8 = buffer,
                            };
                    CALL_EVENT_HOOKS(win, (jwin_event_any) { .key_char = e });
                    if (kc_callback)
                    {
                        kc_callback(&e, win->event_handlers[JWIN_EVENT_TYPE_KEY_CHARACTER].param);
                    }
                }


                if (ptr != static_buffer)
                {
                    ctx->allocator_callbacks.free(ctx->allocator_callbacks.state, ptr);
                    ptr = NULL;
                }
            }
        }
        break;

    case KeyRelease:
        if (saved_keycode < JWIN_KEY_LAST)
        {
            ctx->key_state[saved_keycode] = JWIN_KEY_STATE_UP;

            void (* kr_callback)(
                    const jwin_event_key_release*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_KEY_RELEASE].callback.key_release;
            const jwin_keycode key = xlib_keycode_to_jwin_keycode(ctx, saved_keycode);
            const jwin_mod_state_type mods = xlib_mods_to_jwin_mods(event->xkey.state);

            jwin_event_key_release e =
                    {
                            .base = { .type = JWIN_EVENT_TYPE_KEY_RELEASE, .context = ctx, .window = win },
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
        break;

    case ButtonPress:
        if (should_process_event(win, JWIN_EVENT_TYPE_MOUSE_PRESS) ||
            should_process_event(win, JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS))
        {
            void (* double_press)(
                    const jwin_event_mouse_button_double_press*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].callback.mouse_button_double_press;
            void (* press)(
                    const jwin_event_mouse_button_press*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].callback.mouse_button_press;
            jwin_mouse_button_type button;
            if (((button = xlib_button_to_jwin_button(event->xbutton.button)) != JWIN_MOUSE_BUTTON_TYPE_NONE))
            {
                const jwin_mod_state_type mods = xlib_mods_to_jwin_mods(event->xbutton.state);
                const unsigned long dt = event->xbutton.time - win->button_press_times[event->xbutton.button - Button1];
                win->button_press_times[event->xbutton.button - Button1] = event->xbutton.time;
                if (dt < win->double_click_time && (button != JWIN_MOUSE_BUTTON_TYPE_SCROLL_UP && button != JWIN_MOUSE_BUTTON_TYPE_SCROLL_DN))
                {
                    //  It was a double click
                    jwin_event_mouse_button_double_press e =
                            {
                                    .base = { .type = JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS, .context = ctx, .window = win },
                                    .mods = mods,
                                    .x = event->xbutton.x,
                                    .y = event->xbutton.y,
                                    .button = button,
                            };
                    CALL_EVENT_HOOKS(win, (jwin_event_any) { .double_press = e });
                    if (double_press)
                    {
                        double_press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_DOUBLE_PRESS].param);
                    }
                    else
                    {
                        //  Regular single click
                        jwin_event_mouse_button_press ev =
                                {
                                        .base = { .type = JWIN_EVENT_TYPE_MOUSE_PRESS, .context = ctx, .window = win },
                                        .mods = mods,
                                        .x = event->xbutton.x,
                                        .y = event->xbutton.y,
                                        .button = button,
                                };
                        CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_press = ev });
                        if (press)
                        {
                            press(&ev, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].param);
                        }
                    }
                }
                else
                {
                    //  Regular single click
                    jwin_event_mouse_button_press e =
                            {
                                    .base = { .type = JWIN_EVENT_TYPE_MOUSE_PRESS, .context = ctx, .window = win },
                                    .mods = mods,
                                    .x = event->xbutton.x,
                                    .y = event->xbutton.y,
                                    .button = button,
                            };
                    CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_press = e });
                    if (press)
                    {
                        press(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_PRESS].param);
                    }
                }
            }
        }
        break;

    case ButtonRelease:
        if (should_process_event(win, JWIN_EVENT_TYPE_MOUSE_RELEASE))
        {
            void (* release)(
                    const jwin_event_mouse_button_release*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].callback.mouse_button_release;
            jwin_mouse_button_type button;
            const jwin_mod_state_type mods = xlib_mods_to_jwin_mods(event->xbutton.state);
            if ((button = xlib_button_to_jwin_button(event->xbutton.button)) != JWIN_MOUSE_BUTTON_TYPE_NONE)
            {
                jwin_event_mouse_button_release e =
                        {
                                .base = { .type = JWIN_EVENT_TYPE_MOUSE_RELEASE, .context = ctx, .window = win },
                                .mods = mods,
                                .x = event->xbutton.x,
                                .y = event->xbutton.y,
                                .button = button,
                        };
                CALL_EVENT_HOOKS(win, (jwin_event_any) { .button_release = e });
                if ((release))
                {
                    release(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_RELEASE].param);
                }
            }
        }
        break;

    case MotionNotify:
        if (should_process_event(win, JWIN_EVENT_TYPE_MOUSE_MOVE))
        {
            const int x = event->xmotion.x;
            const int y = event->xmotion.y;
            void (* motion)(
                    const jwin_event_mouse_motion*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_MOVE].callback.mouse_motion;
            jwin_event_mouse_motion e =
                    {
                            .base = { .type = JWIN_EVENT_TYPE_MOUSE_MOVE, .context = ctx, .window = win },
                            .mods = xlib_mods_to_jwin_mods(event->xmotion.state),
                            .x = x,
                            .y = y,
                    };
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .mouse_motion = e });
            if (motion)
            {
                motion(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_MOVE].param);
            }
        }
        break;

    case EnterNotify:
        if (should_process_event(win, JWIN_EVENT_TYPE_MOUSE_ENTER))
        {
            {
                void (* enter)(
                        const jwin_event_mouse_enter*,
                        void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_ENTER].callback.mouse_enter;
                jwin_event_mouse_enter e =
                        {
                                .type = JWIN_EVENT_TYPE_MOUSE_ENTER,
                                .context = ctx,
                                .window = win,
                        };
                CALL_EVENT_HOOKS(win, (jwin_event_any) { .mouse_enter = e });
                if (enter)
                {
                    enter(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_ENTER].param);
                }
            }
            const int x = event->xcrossing.x;
            const int y = event->xcrossing.y;
            void (* motion)(
                    const jwin_event_mouse_motion*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_MOUSE_MOVE].callback.mouse_motion;
            jwin_event_mouse_motion e =
                    {
                            .base = { .type = JWIN_EVENT_TYPE_MOUSE_MOVE, .context = ctx, .window = win },
                            .mods = xlib_mods_to_jwin_mods(event->xcrossing.state),
                            .x = x,
                            .y = y,
                    };
            CALL_EVENT_HOOKS(win, (jwin_event_any) { .mouse_motion = e });
            if (motion)
            {
                motion(&e, win->event_handlers[JWIN_EVENT_TYPE_MOUSE_MOVE].param);
            }
        }
        break;

    case LeaveNotify:
        if (should_process_event(win, JWIN_EVENT_TYPE_MOUSE_LEAVE))
        {
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
        break;

    case FocusIn:
        if ((event->xfocus.mode != NotifyGrab && event->xfocus.mode != NotifyUngrab))
        {
            void (* gain)(
                    const jwin_event_focus_gain*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_FOCUS_GAIN].callback.focus_gain;
            XSetICFocus(win->ic);
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
        break;

    case FocusOut:
        if (event->xfocus.mode != NotifyGrab && event->xfocus.mode != NotifyUngrab)
        {
            void (* lose)(
                    const jwin_event_focus_gain*,
                    void*) = win->event_handlers[JWIN_EVENT_TYPE_FOCUS_LOSE].callback.focus_lose;
            XUnsetICFocus(win->ic);
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
            memset(ctx->key_state, 0, sizeof(ctx->key_state));
        }
        break;

    case Expose:
        if (should_process_event(win, JWIN_EVENT_TYPE_MOUSE_RELEASE))
        {
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
        }
        break;

    case ConfigureNotify:
        {
            const XConfigureEvent* cfg = &event->xconfigure;
            if ((unsigned) cfg->width != win->width || (unsigned) cfg->height != win->height)
            {
                void (* resize)(
                        const jwin_event_resize*, void*) = win->event_handlers[JWIN_EVENT_TYPE_RESIZE].callback.resize;
                jwin_event_resize e =
                        {
                                .base = { .type = JWIN_EVENT_TYPE_RESIZE, .context = ctx, .window = win },
                                .width = cfg->width,
                                .height = cfg->height,
                        };
                CALL_EVENT_HOOKS(win, (jwin_event_any) { .resize = e });
                if (resize)
                {
                    resize(&e, win->event_handlers[JWIN_EVENT_TYPE_RESIZE].param);
                }

                win->width = cfg->width;
                win->height = cfg->height;
            }

            //  Check position
            int x = cfg->x, y = cfg->y;
            if (!cfg->send_event && win->parent_hwnd != ctx->root)
            {
                const XErrorHandler eh = XSetErrorHandler(x_error_handler);
                Window unused;
                XTranslateCoordinates(ctx->dpy, win->parent_hwnd, ctx->root, x, y, &x, &y, &unused);
                XSetErrorHandler(eh);
                if (x_error_code == BadWindow)
                {
                    break;
                }
            }

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
        break;

    case ClientMessage:
    {
        const XClientMessageEvent* cli = &event->xclient;
        if (filtered || cli->message_type == None)
        {
            break;
        }
        else if (cli->message_type == ctx->atoms.wm_protocols)
        {
            const Atom protocol = cli->data.l[0];
            if (protocol == ctx->atoms.wm_delete_window)
            {
                //  Window was asked to close
                int (* close)(
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
            else if (protocol == ctx->atoms.net_wm_ping)
            {
                //  Must ping back to wm
                XEvent reply =
                        {
                                .xclient = { .window = ctx->root }
                        };
                XSendEvent(ctx->dpy, ctx->root, False, SubstructureNotifyMask | SubstructureRedirectMask, &reply);
            }
        }
        //  Here would be checking for different client messages associated with DND
    }
        break;

    case PropertyNotify:
    {
        //  This mostly deals with WM messages
        const XPropertyEvent* prop = &event->xproperty;
        if (prop->state != PropertyNewValue)
        {
            break;
        }

    }
        break;

    case DestroyNotify:break;
    }

    end:
    return JWIN_RESULT_SUCCESS;
}

void jwin_context_set_event_hook(jwin_context* ctx, void (* hook)(const jwin_event_any*, void*), void* param)
{
    ctx->event_hook = hook;
    ctx->event_param = param;
}


Display* jwin_context_native_xlib(jwin_context* ctx)
{
    return ctx->dpy;
}

xcb_connection_t* jwin_contex_native_xcb(jwin_context* ctx)
{
    return ctx->xcb_connection;
}
