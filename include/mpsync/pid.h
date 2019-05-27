// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_INCLUDE_MPSYNC_PID_H_
#define MPSYNC_INCLUDE_MPSYNC_PID_H_

#include <cstdint>
#include <string>

namespace mpsync {

struct Pid {
    uint64_t _pid;
};

struct ProcessSignature {
    std::string _name;
    const void *_signature;
};

}  // namespace mpsync

#endif  // MPSYNC_INCLUDE_MPSYNC_PID_H_