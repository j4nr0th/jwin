Table 1: Main requirement table

| id        | requirement                                                                                                               | status        | where                              | what method          |
|-----------|---------------------------------------------------------------------------------------------------------------------------|---------------|------------------------------------|----------------------|
| JWIN-MR-1 | Use the library to create window and receive all events without the need to include any platform headers                  | met           | base_test.c and multiwin.c         | inspection and tests |
| JWIN-MR-2 | Be able to be used to provide access to resources needed by Vulkan to create VkSurfaceKHR                                 | met           | vk_test.c, definition in jwin_vk.h | inspection and tests |
| JWIN-MR-3 | Allow for a choice of a memory allocator                                                                                  | met           | context.h                          | inspection           |
| JWIN-MR-4 | Allow for creation of bordered and borderless windows                                                                     | not met       | -                                  | -                    |
| JWIN-MR-5 | Allow windows to respond to events specified in Table 2 with it's callbacks and window/context event hook (when provided) | met           | window.h, context.h, and all tests | test and inspection  |
| JWIN-MR-6 | Allow for query of information in the Table 3 at runtime                                                                  | partially met | window.h                           | inspection           |

Table 2: Event specification table

| event trigger                                | event enum value                          | information                                   |
|----------------------------------------------|-------------------------------------------|-----------------------------------------------|
| mouse entered window                         | JWIN_EVENT_TYPE_MOUSE_ENTER               | type, window, context                         |
| mouse left window                            | JWIN_EVENT_TYPE_MOUSE_LEAVE               | type, window, context                         |
| mouse moved while window is focused          | JWIN_EVENT_TYPE_MOUSE_MOVE                | type, window, context, x, y, mods             |
| mouse button was pressed                     | JWIN_EVENT_TYPE_MOUSE_BUTTON_PRESS        | type, window, context, button, x, y, mods     |
| mouse button was pressed in quick succession | JWIN_EVENT_TYPE_MOUSE_BUTTON_DOUBLE_PRESS | type, window, context, button, x, y, mods     |
| mouse button was released                    | JWIN_EVENT_TYPE_MOUSE_BUTTON_RELEASE      | type, window, context, button, x, y, mods     |
| keyboard key was pressed                     | JWIN_EVENT_TYPE_KEY_DOWN                  | type, window, context, keycode, was repeated  |
| keyboard key was released                    | JWIN_EVENT_TYPE_KEY_UP                    | type, window, context, keycode                |
| key press generated a character input        | JWIN_EVENT_TYPE_KEY_CHARACTER             | type, window, context, UTF8 encoded character |
| window was requested to close                | JWIN_EVENT_TYPE_CLOSE                     | type, window, context                         |
| window is about to be destroyed              | JWIN_EVENT_TYPE_DESTROY                   | type, window, context                         |
| window's contents should be redrawn          | JWIN_EVENT_TYPE_REFRESH                   | type, window, context                         |
| window's size was changed                    | JWIN_EVENT_TYPE_RESIZE                    | type, window, context, new width, new height  |
| window's position was changed                | JWIN_EVNET_TYPE_MOVE                      | type, window, context, new x, new y           |

Table 3: Window information available for query

| property            | type     | function (header)                   |
|---------------------|----------|-------------------------------------|
| window's width      | unsigned | jwin_window_get_size (window.h)     |
| window's height     | unsigned | jwin_window_get_size (window.h)     |
| window's x position | int      | jwin_window_get_position (window.h) |
| window's y position | int      | jwin_window_get_position (window.h) |
