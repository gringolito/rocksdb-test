// Copyright © 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#pragma(once)

#include <unordered_map>

#include "mpsync/middleware.h"

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
    void LoopWhile(bool *keeprunning) override;

   private:
    ProcessSignature myself_;
    ProcessSignature listening_server_;
    OnServerFoundCb on_server_found_cb_;
    OnServerLostCb on_server_lost_cb_;
    Pid server_pid_;
    int inotify_;
    int watch_;
    bool server_was_found_;
    bool server_was_lost_;
    std::unordered_map<int, OnFdEventCb> fd_callbacks_;

    void WatchServerPid();
    void InotifyEvent(int watch);
    void ReadPid();
    void ServerLost();
    void ServerFound(uint64_t pid);
};

}  // namespace stubs
}  // namespace mpsync