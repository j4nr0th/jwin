cmake_minimum_required(VERSION 3.26)
project(jwin C)

set(CMAKE_C_STANDARD 99)

list(APPEND JWIN_PUBLIC_HEADERS
        source/jwin.h
        source/common.h
        source/events.h
        source/window.h
        source/error.h
        source/context.h
        source/jwin_vk.h
)

add_library(jwin
        source/xlib/context_xlib.c
        source/error.c
        source/xlib/common_xlib.h
        source/xlib/context_xlib.h
        source/internal.c
        source/internal.h
        source/events.c
        source/xlib/window_xlib.c
        source/xlib/window_xlib.h
        ${JWIN_PUBLIC_HEADERS}
        source/xlib/vk_xlib.c
)

if (WIN32)
    target_link_libraries(jwin PRIVATE)
else ()
    target_link_libraries(jwin PRIVATE X11 X11-xcb)
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    target_compile_options(jwin PRIVATE -Wall -Wextra -Werror)
endif ()

target_compile_definitions(jwin PRIVATE JWIN_BUILD_LIBRARY)

enable_testing()
add_executable(base_test test/base_test.c
        test/test_common.h)
target_link_libraries(base_test PRIVATE jwin)
add_test(NAME base_test COMMAND base_test)

add_executable(vk_test test/vk_test.c
        test/test_common.h)
target_link_libraries(vk_test PRIVATE jwin vulkan)
add_test(NAME vulkan_test COMMAND vk_test)

add_executable(multi_window_test test/multiwin.c)
target_link_libraries(multi_window_test PRIVATE jwin)
add_test(NAME multi_window_test COMMAND multi_window_test)
