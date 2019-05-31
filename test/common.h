// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_TEST_COMMON_H_
#define MPSYNC_TEST_COMMON_H_

#include "mpsync/pid.h"
#include "mpsync/signal.h"

namespace mpsync {

static const ProcessSignature kTestProcess{ ._name = "test", ._signature = nullptr };

static const Signal kSignalName{ ._name = "name", ._signature = nullptr };
static const Signal kSignalPosition{ ._name = "position", ._signature = nullptr };
static const Signal kSignalTimes{ ._name = "times", ._signature = nullptr };
static const Signal kSignalValid{ ._name = "valid", ._signature = nullptr };

}  // namespace mpsync

#endif  // MPSYNC_TEST_COMMON_H_