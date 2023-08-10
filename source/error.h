//
// Created by jan on 6.8.2023.
//

#ifndef JWIN_ERROR_H
#define JWIN_ERROR_H

#include "common.h"

enum jwin_result_T
{
    JWIN_RESULT_SUCCESS = 0,

    JWIN_RESULT_SHOULD_CLOSE,

    JWIN_RESULT_BAD_ALLOCATOR,
    JWIN_RESULT_BAD_ALLOC,
    JWIN_RESULT_NO_PLATFORM,
    JWIN_RESULT_BAD_CONTEXT,
    JWIN_RESULT_NO_WINDOW,
    JWIN_RESULT_BAD_EVENT_TYPE,
    JWIN_RESULT_BAD_WAIT,
    JWIN_RESULT_TIMED_OUT,

    JWIN_RESULT_VK_FAILED,

    JWIN_RESULT_COUNT,
};

typedef enum jwin_result_T jwin_result;

JWIN_API const char* jwin_result_msg_str(jwin_result res);

JWIN_API const char* jwin_result_to_str(jwin_result res);

#ifndef JWIN_SUCCESS
#define JWIN_SUCCESS(x) ((x) == JWIN_RESULT_SUCCESS)
#endif  // JWIN_SUCCESS

#endif //JWIN_ERROR_H
