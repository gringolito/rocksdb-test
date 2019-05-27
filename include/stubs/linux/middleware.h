// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_INCLUDE_STUBS_LINUX_MIDDLEWARE_H_
#define MPSYNC_INCLUDE_STUBS_LINUX_MIDDLEWARE_H_

#include "stubs/middleware.h"

struct inotify_event;  //!< Forward declaration

namespace mpsync {
namespace stubs {

class LinuxMiddleware final : public mpsync::stubs::Middleware {
   public:
    LinuxMiddleware();
    ~LinuxMiddleware();

   private:
    int inotify_;
    int watch_;
    void WatchServerPid() override;
    void ReadInotifyEvent();
    void ProcessInotifyEvent(const inotify_event *event);
};

}  // namespace stubs
}  // namespace mpsync

#endif  // MPSYNC_INCLUDE_STUBS_LINUX_MIDDLEWARE_H_