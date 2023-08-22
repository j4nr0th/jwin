cmake_minimum_required(VERSION 3.26)
project(jwin C)

find_package(Vulkan REQUIRED)

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


if (WIN32)
    list(APPEND JWIN_PLATFORM_FILES 
        source/win32/context_win32.c
        source/win32/common_win32.h
        source/win32/context_win32.h
        source/win32/window_win32.c
        source/win32/window_win32.h
        source/win32/vk_win32.c)
else ()
    list(APPEND JWIN_PLATFORM_FILES 
        source/xlib/context_xlib.c
        source/xlib/common_xlib.h
        source/xlib/context_xlib.h
        source/xlib/window_xlib.c
        source/xlib/window_xlib.h
        source/xlib/vk_xlib.c)
endif ()


add_library(jwin
        source/error.c
        source/internal.c
        source/internal.h
        source/events.c
        ${JWIN_PLATFORM_FILES}
        ${JWIN_PUBLIC_HEADERS}
)


target_link_libraries(jwin PRIVATE "${Vulkan_LIBRARY}")
target_include_directories(jwin PRIVATE "${Vulkan_INCLUDE_DIR}")
if (WIN32)
   if (BUILD_SHARED_LIBS)
    target_compile_definitions(jwin PRIVATE BUILDING_DLL)
   endif ()
else ()
    target_link_libraries(jwin PRIVATE X11 X11-xcb)
endif ()
target_compile_definitions(jwin PRIVATE JWIN_BUILD_LIBRARY)

if (CMAKE_C_COMPILER_ID STREQUAL GNU)
    target_compile_options(jwin PRIVATE -Wall -Wextra -Werror)
endif ()


enable_testing()
add_executable(base_test test/base_test.c
        test/test_common.h)
   target_include_directories(base_test PRIVATE "${Vulkan_INCLUDE_DIR}")
target_link_libraries(base_test PRIVATE jwin)
add_test(NAME base_test COMMAND base_test)

add_executable(vk_test test/vk_test.c
        test/test_common.h)
   target_include_directories(vk_test PRIVATE "${Vulkan_INCLUDE_DIR}")
target_link_libraries(vk_test PRIVATE jwin)
add_test(NAME vulkan_test COMMAND vk_test)

add_executable(multi_window_test test/multiwin.c)
target_link_libraries(multi_window_test PRIVATE jwin)
   target_include_directories(multi_window_test PRIVATE "${Vulkan_INCLUDE_DIR}")
add_test(NAME multi_window_test COMMAND multi_window_test)
