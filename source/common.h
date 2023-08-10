//
// Created by jan on 6.8.2023.
//

#ifndef JWIN_COMMON_H
#define JWIN_COMMON_H
//  This header includes common standard library headers needed by ALL and includes common typedefs

#include <stdint.h>

typedef struct jwin_context_T jwin_context;

typedef struct jwin_window_T jwin_window;

typedef enum jwin_keycode_T jwin_keycode;

typedef uint_least32_t jwin_unicode;

typedef struct jwin_event_T jwin_event;

struct jwin_allocator_callbacks_T
{
    void* (*alloc)(void* state, uint64_t size);
    void* (*realloc)(void* state, void* ptr, uint64_t new_size);
    void (*free)(void* state, void* ptr);
    void* state;
};
typedef struct jwin_allocator_callbacks_T jwin_allocator_callbacks;

struct jwin_error_callbacks_T
{
    void (*report)(const char* msg, const char* file, int line, const char* function, void* state);
    void* state;
};
typedef struct jwin_error_callbacks_T jwin_error_callbacks;

enum
{
    JWIN_POSITION_DONT_CARE = -1
};

#include "error.h"

#endif //JWIN_COMMON_H
