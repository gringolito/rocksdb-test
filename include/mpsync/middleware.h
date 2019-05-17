// Copyright © 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_INCLUDE_MPSYNC_MIDDLEWARE_H_
#define MPSYNC_INCLUDE_MPSYNC_MIDDLEWARE_H_

#include <functional>

#include "mpsync/pid.h"

namespace mpsync {

using OnServerFoundCb = std::function<void(const Pid &)>;
using OnServerLostCb = std::function<void(const Pid &)>;
using OnFdEventCb = std::function<void(int fd)>;

class Middleware {
   public:
    virtual ~Middleware() = default;
    virtual void PublishServer(const ProcessSignature &server_signature) = 0;
    virtual void UnpublishServer() = 0;
    virtual void RegisterToServer(const ProcessSignature &server_signature,
                                  OnServerFoundCb on_server_found_event,
                                  OnServerLostCb on_server_lost_event) = 0;
    virtual void SubscribeToFdEvents(int fd, OnFdEventCb on_fd_event) = 0;
    virtual void UnsubscribeFromFdEvents(int fd) = 0;
    virtual void LoopWhile(bool *keeprunning) = 0;
};

}  // namespace mpsync

#endif  // MPSYNC_INCLUDE_MPSYNC_MIDDLEWARE_H_