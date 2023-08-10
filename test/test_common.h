//
// Created by jan on 10.8.2023.
//

#ifndef JWIN_TEST_COMMON_H
#define JWIN_TEST_COMMON_H

#ifndef NDEBUG
#   ifdef __GNUC__
#       define ASSERTION_FAIL_COMMAND __builtin_trap()
#   endif
#else
#   define ASSERTION_FAIL_COMMAND exit(EXIT_FAILURE)
#endif

#ifndef ASSERTION_FAIL_COMMAND
#error  macro ASSERTION_FAIL_COMMAND was undefined
#endif
#define ASSERT(x) if (!(x)) { fprintf(stderr, "Failed assertion \"%s\"\n", #x); ASSERTION_FAIL_COMMAND;} (void)0

#define JWIN_TEST_CALL(x) {res = (x); fprintf(stdout, "%s -> %s\n", #x, jwin_result_to_str(res));}(void)0

#endif //JWIN_TEST_COMMON_H
