//
// Created by jan on 6.8.2023.
//

#include "internal.h"
#include <assert.h>
#include <malloc.h>

#ifndef _WIN32
static const char* const test_ptr = "Don't let your memes be dreams";
#else
#define test_ptr ((void*)0xBadBabeBeefCafe)
#endif

static void* allocate(void* state, uint64_t size)
{
    assert(state == test_ptr);
    return malloc(size);
}

static void* reallocate(void* state, void* ptr, uint64_t new_size)
{
    assert(state == test_ptr);
    return realloc(ptr, new_size);
}

static void deallocate(void* state, void* ptr)
{
    assert(state == test_ptr);
    free(ptr);
}

const jwin_allocator_callbacks JWIN_DEFAULT_ALLOCATOR_CALLBACKS =
        {
                .alloc = allocate,
                .realloc = reallocate,
                .free = deallocate,
                .state = (void*) test_ptr,
        };
