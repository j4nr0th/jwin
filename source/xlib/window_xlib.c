//
// Created by jan on 7.8.2023.
//

#include <X11/Xatom.h>
#include <unistd.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "window_xlib.h"
#include "context_xlib.h"
#include "../window.h"
#include "../events.h"


static int x_error_code;

static int x_error_handler(Display* dpy, XErrorEvent* e)
{
    (void) dpy;
    x_error_code = e->error_code;
    return 0;
}

jwin_result jwin_window_create(jwin_context* ctx, const jwin_window_create_info* create_info, jwin_window** p_out)
{
    jwin_window* const this = ctx->allocator_callbacks.alloc(ctx->allocator_callbacks.state, sizeof(*this));
    if (!this)
    {
        REPORT_ERROR(ctx, "Could not allocate memory for the window");
        return JWIN_RESULT_BAD_ALLOC;
    }

    Display* dpy = ctx->dpy;
    const Colormap cmap = XCreateColormap(dpy, ctx->root, DefaultVisual(dpy, ctx->screen), AllocNone);
    jwin_window_create_info info = *create_info;
    create_info = NULL;
    if (!info.title)
    {
        info.title = "jwin-program";
    }
    if (info.xorg.double_click_time_ms == 0)
    {
        info.xorg.double_click_time_ms = 250;
    }

    XSetWindowAttributes wa =
            {
                    .colormap = cmap,
                    .event_mask = StructureNotifyMask |
                                  KeyPressMask | KeyReleaseMask |
                                  ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                                  ExposureMask | FocusChangeMask | VisibilityChangeMask |
                                  EnterWindowMask | LeaveWindowMask | PropertyChangeMask,
            };
    const XErrorHandler eh = XSetErrorHandler(x_error_handler);
    int x = info.x != JWIN_POSITION_DONT_CARE ? info.x : 0;
    int y = info.y != JWIN_POSITION_DONT_CARE ? info.y : 0;
    const Window hwnd = XCreateWindow(
            dpy, ctx->root, x, y, info.width,
            info.height, 0, DefaultDepth(dpy, ctx->screen), InputOutput, DefaultVisual(dpy, ctx->screen),
            CWEventMask | CWColormap, &wa);
    (void) XSetErrorHandler(eh);
    if (!hwnd)
    {
        XFreeColormap(dpy, cmap);
        char buffer[256];
        XGetErrorText(dpy, x_error_code, buffer, sizeof(buffer));
        REPORT_ERROR(ctx, "Could not create XLib window, reason: %s", buffer);
        ctx->allocator_callbacks.free(ctx->allocator_callbacks.state, this);
        return JWIN_RESULT_NO_WINDOW;
    }

    //  Protocol support
    Atom window_protocols[] =
            {
                    ctx->atoms.wm_delete_window,
                    ctx->atoms.net_wm_ping,
            };
    XSetWMProtocols(dpy, hwnd, window_protocols, sizeof(window_protocols) / sizeof(*window_protocols));

    //  Process id
    const pid_t pid = getpid();
    XChangeProperty(dpy, hwnd, ctx->atoms.net_wm_pid, XA_CARDINAL, 32, PropModeReplace, (const unsigned char*) &pid, 1);

    //  WM_HINTS
    XWMHints hints;
    hints.flags = StateHint;
    hints.initial_state = NormalState;
    XSetWMHints(dpy, hwnd, &hints);

    //  WM_NORMAL_HINTS
    XSizeHints sz_hints = { 0 };
    if (info.fixed_size)
    {
        sz_hints.flags |= (PMinSize | PMaxSize);
        sz_hints.min_width = (sz_hints.max_width = (int) info.width);
        sz_hints.min_height = (sz_hints.max_height = (int) info.height);
    }
    if (info.x != JWIN_POSITION_DONT_CARE || info.y != JWIN_POSITION_DONT_CARE)
    {
        sz_hints.flags |= PPosition;
        sz_hints.x = x;
        sz_hints.y = y;
    }
    sz_hints.flags |= PWinGravity;
    sz_hints.win_gravity = StaticGravity;
    XSetNormalHints(dpy, hwnd, &sz_hints);

    //  Class hints
    XClassHint class_hint = { 0 };
    if (info.xorg.instance_name && info.xorg.class_name)
    {
        class_hint.res_name = (char*) info.xorg.instance_name;
        class_hint.res_class = (char*) info.xorg.class_name;
    }
    else
    {
        const char* res_name = getenv("RESOURCE_NAME");
        if (res_name && *res_name)
        {
            class_hint.res_name = (char*) res_name;
        }
        else if (info.title && *info.title)
        {
            class_hint.res_name = (char*) info.title;
        }
        else
        {
            class_hint.res_name = (char*) "jwin-program";
        }

        if (info.title && *info.title)
        {
            class_hint.res_name = (char*) info.title;
        }
        else
        {
            class_hint.res_name = (char*) "jwin-program";
        }
    }
    XSetClassHint(dpy, hwnd, &class_hint);

    //  Create input context
    XIC ic = XCreateIC(
            ctx->input_method,
//                       XNInputStyle, XIMPreeditNothing|XIMStatusNothing,
//                       XNClientWindow, hwnd,
//                       XNFocusWindow, hwnd,
            XNPreeditAttributes, XIMPreeditNone, XNStatusAttributes, XIMStatusNone, XNInputStyle,
            XIMStatusNone | XIMPreeditNone,
            NULL);
    if (ic)
    {
        XWindowAttributes attibs;
        XGetWindowAttributes(dpy, hwnd, &attibs);
        long add = 0;
        if (XGetICValues(ic, XNFilterEvents, &add, NULL) == NULL)
        {
            XSelectInput(dpy, hwnd, attibs.your_event_mask | add);
        }
    }

    //  Set title
    XStoreName(dpy, hwnd, info.title);
    XSetIconName(dpy, hwnd, info.title);

    //  Recheck window geometry
    Window root_ret;
    unsigned depth_ret, border_ret;
    XGetGeometry(dpy, hwnd, &root_ret, &this->x, &this->y, &this->width, &this->height, &depth_ret, &border_ret);

    this->ctx = ctx;
    memset(this->event_handlers, 0, sizeof(this->event_handlers));

    this->hwnd = hwnd;
    this->cmap = cmap;

    this->is_minimized = 1;
    memset(this->button_press_times, 0, sizeof(this->button_press_times));
    this->ic = ic;
    this->parent_hwnd = ctx->root;
    this->double_click_time = info.xorg.double_click_time_ms;

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

        XDestroyIC(ic);
        XDestroyWindow(dpy, hwnd);
        XFreeColormap(dpy, cmap);
        ctx->allocator_callbacks.free(ctx->allocator_callbacks.state, this);
        return res;
    }

    *p_out = this;
    XFlush(dpy);
    return JWIN_RESULT_SUCCESS;
}

jwin_result jwin_window_destroy(jwin_window* win)
{
    jwin_context* ctx = win->ctx;
    void
    (* dtor)(const jwin_event_destroy*, void*) = win->event_handlers[JWIN_EVENT_TYPE_DESTROY].callback.destroy;
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
    jwin_result res = INTERNAL_remove_window_from_context(ctx, win);
    if (res != JWIN_RESULT_SUCCESS)
    {
        REPORT_ERROR(ctx, "Could not remove the window from the context");
        return res;
    }

    XDestroyIC(win->ic);
    XDestroyWindow(ctx->dpy, win->hwnd);
    XFreeColormap(ctx->dpy, win->cmap);
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

    write(win->ctx->notification_pipes[PIPE_INPUT], event, sizeof(*event));

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
    XStoreName(win->ctx->dpy, win->hwnd, new_title);
    return JWIN_RESULT_SUCCESS;
}

void jwin_window_show(jwin_window* win)
{
    win->is_minimized = 0;
    XMapWindow(win->ctx->dpy, win->hwnd);
}

void jwin_window_hide(jwin_window* win)
{
    win->is_minimized = 1;
    XUnmapWindow(win->ctx->dpy, win->hwnd);
}

jwin_result jwin_window_ask_to_close(jwin_window* win)
{
    win->should_close = 1;
    return JWIN_RESULT_SUCCESS;
}

void jwin_window_get_size(jwin_window* win, unsigned int* p_width, unsigned int* p_height)
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

Window jwin_contex_native_window_xlib(jwin_window* win)
{
    return win->hwnd;
}

xcb_window_t jwin_contex_native_window_xcb(jwin_window* win)
{
    return (xcb_window_t)win->hwnd;
}

