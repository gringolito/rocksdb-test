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

#include <fstream>
#include <set>
#include <unordered_map>

#include "mpsync/middleware.h"

struct inotify_event;  //!< Forward declaration
struct pollfd; //!< Forward declaration

namespace mpsync {
namespace stubs {

class Middleware final : public mpsync::Middleware {
   public:
    Middleware();
    ~Middleware();
    void PublishServer(const ProcessSignature &server_signature) override;
    void UnpublishServer() override;
    void RegisterToServer(const ProcessSignature &server_signature,
                          OnServerFoundCb on_server_found_event,
                          OnServerLostCb on_server_lost_event) override;
    void SubscribeToFdEvents(int fd, OnFdEventCb on_fd_event) override;
    void UnsubscribeFromFdEvents(int fd) override;
    bool LoopWhile(bool *keeprunning) override;

   private:
    ProcessSignature myself_;
    ProcessSignature listening_server_;
    OnServerFoundCb on_server_found_cb_;
    OnServerLostCb on_server_lost_cb_;
    Pid server_pid_;
    std::ofstream server_pid_file_;
    int inotify_;
    int watch_;
    bool server_is_found_;
    bool server_published_;
    bool first_poll_call_;
    std::unordered_map<int, std::function<void()>> fd_callbacks_;
    std::set<int> poll_fds_;

    void WatchServerPid();
    void ReadInotifyEvent();
    void ProcessInotifyEvent(const inotify_event *event);
    void ProcessPidFileEvent();
    Pid ReadPid();
    void ServerLost();
    void ServerFound(Pid &&pid);
    bool Poll();
    bool ProcessPollEvent(const pollfd *fd);
};

}  // namespace stubs
}  // namespace mpsync

#endif  // MPSYNC_INCLUDE_STUBS_LINUX_MIDDLEWARE_H_