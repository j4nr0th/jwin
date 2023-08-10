//
// Created by jan on 6.8.2023.
//

#ifndef JWIN_INTERNAL_H
#define JWIN_INTERNAL_H

#include "common.h"

extern const jwin_allocator_callbacks JWIN_DEFAULT_ALLOCATOR_CALLBACKS;

static inline int CHECK_ALLOCATOR_CALLBACKS(const jwin_allocator_callbacks* ptr)
{ return ((ptr)->free && (ptr)->alloc && (ptr)->realloc); }


#endif //JWIN_INTERNAL_H
