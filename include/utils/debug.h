// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#ifndef MPSYNC_INCLUDE_UTILS_DEBUG_H_
#define MPSYNC_INCLUDE_UTILS_DEBUG_H_

#include <cstdio>
#include <cstring>

static constexpr bool kDebugEnabled = true;

#define debug_enterp(_fmt, ...)                                               \
    do {                                                                      \
        if (kDebugEnabled) printf("%s(" _fmt ")\n", __func__, ##__VA_ARGS__); \
    } while (0)

#define debug(_fmt, ...)                                                       \
    do {                                                                       \
        if (kDebugEnabled) printf("%s() " _fmt "\n", __func__, ##__VA_ARGS__); \
    } while (0)

#define debug_enter() debug("")

#define debug_errno() debug("%d (%s)", errno, strerror(errno))

#endif /* MPSYNC_INCLUDE_UTILS_DEBUG_H_ */