// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#ifndef MPSYNC_INCLUDE_UTILS_ASSERT_H_
#define MPSYNC_INCLUDE_UTILS_ASSERT_H_

#include <cstring>
#include <err.h>

#define fault_debug(_fmt, ...) \
    errx(1, "%s:%d %s()\nfault condition: " _fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define fault_errno() fault_debug("%d (%s)", errno, strerror(errno))

#define fault_debug_errno(_fmt, ...) \
    fault_debug(_fmt ": %d (%s)", ##__VA_ARGS__, errno, strerror(errno))

#define __err(_cond, _fmt, ...)                                                              \
    errx(1, "%s:%d %s()\nassertation failed: " _cond "\nmessage: " _fmt, __FILE__, __LINE__, \
         __func__, ##__VA_ARGS__)

#define assert_debug(_cond, _fmt, ...)          \
    do {                                        \
        if (!(_cond)) {                         \
            __err(#_cond, _fmt, ##__VA_ARGS__); \
        }                                       \
    } while (0)

#define assert_errno(_cond) assert_debug((_cond), "%d (%s)", errno, strerror(errno))

#define assert_debug_errno(_cond, _fmt, ...) \
    assert_debug((_cond), _fmt ": %d (%s)", ##__VA_ARGS__, errno, strerror(errno))

#endif /* MPSYNC_INCLUDE_UTILS_ASSERT_H_ */