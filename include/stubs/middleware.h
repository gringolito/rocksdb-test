// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_INCLUDE_STUBS_MIDDLEWARE_H_
#define MPSYNC_INCLUDE_STUBS_MIDDLEWARE_H_

#include <set>
#include <unordered_map>

#include "mpsync/middleware.h"
#include "stubs/message_queue.h"

struct pollfd;  //!< Forward declaration

namespace mpsync {
namespace stubs {

static const std::string kPidPath{ "/tmp/stubmw/pids/" };

class Middleware : public mpsync::Middleware {
   public:
    Middleware();
    virtual ~Middleware();
    void PublishServer(const ProcessSignature &server_signature) final;
    void UnpublishServer() final;
    void SubscribeToServer(const ProcessSignature &server_signature,
                           OnServerFoundCb &&on_server_found_event,
                           OnServerLostCb &&on_server_lost_event) final;
    void UnsubscribeFromServer() final;
    void SubscribeToFdEvents(int fd, OnFdEventCb &&on_fd_event) final;
    void UnsubscribeFromFdEvents(int fd) final;
    bool LoopWhile(bool *keeprunning) final;
    void SendSignal(const Pid &pid, const Signal &signal, const std::string &content) final;
    void RegisterToSignal(const Signal &signal, OnSignalReceivedCb &&on_signal_received) final;

   private:
    ProcessSignature myself_;
    ProcessSignature listening_server_;
    OnServerFoundCb on_server_found_cb_;
    OnServerLostCb on_server_lost_cb_;
    Pid me_;
    std::set<Pid> server_pids_;
    int server_service_;
    int subscription_service_;
    bool server_is_found_;
    bool server_published_;
    bool server_subscribed_;
    std::unordered_map<int, std::function<void()>> fd_callbacks_;
    std::unordered_map<Signal, MessageQueue> signal_queues_;
    std::set<int> poll_fds_;
    bool PidIsAlive(const Pid &pid);
    void ServerFound(Pid &&pid);
    void ServerLost(Pid &&pid);
    bool Poll();
    bool ProcessPollEvent(const pollfd *fd);
    void ProcessServerEvent(int service);
};

}  // namespace stubs
}  // namespace mpsync

#endif  // MPSYNC_INCLUDE_STUBS_MIDDLEWARE_H_