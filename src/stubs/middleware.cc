// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "stubs/middleware.h"

#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>

#include <algorithm>
#include <vector>
#include <fstream>

#include <sstream>

extern "C" {
#include "tipcc.h"
}

#include "utils/assert.h"
#include "utils/debug.h"

namespace mpsync {
namespace stubs {

static constexpr uint32_t kTipcLowerPort = 0;
static constexpr uint32_t kTipcUpperPort = ~0;
static constexpr int kTipcWaitServer = -1;

#define get_service_type(_a) *(static_cast<const uint32_t *>(_a))

/*
static std::ostream &operator<<(std::ostream &os, const struct tipc_event &evt)
{
    os << "tipc_event = {\n"
       << "  .event = " << evt.event << "\n"
       << "  .found_lower = " << evt.found_lower << "\n"
       << "  .found_upper = " << evt.found_upper << "\n"
       << "  .port = {\n"
       << "    .ref = " << evt.port.ref << "\n"
       << "    .node = " << evt.port.node << "\n"
       << "  }\n"
       << "  .s = {\n"
       << "    .seq = {\n"
       << "      .type = " << evt.s.seq.type << "\n"
       << "      .lower = " << evt.s.seq.lower << "\n"
       << "      .upper = " << evt.s.seq.upper << "\n"
       << "    }\n"
       << "    .timeout = " << evt.s.timeout << "\n"
       << "    .filter = " << evt.s.filter << "\n"
       << "    .usr_handle = {" << std::hex;
    for (auto c : evt.s.usr_handle) os << " 0x" << unsigned(c);
    os << " }\n"
       << "  }\n"
       << "}";

    return os;
}

static void DumpTipcEvent(int service)
{
    struct tipc_event evt;
    assert_errno(recv(service, &evt, sizeof(evt), 0) == sizeof(evt));
    std::stringstream ss;
    ss << evt;
    debug("\n%s", ss.str().c_str());
}
 */

struct SignalMessage {
    Pid pid;
    std::string content;

    std::string Serialize()
    {
        std::string serialized;
        serialized.reserve(sizeof(pid) + content.size());
        serialized.append(reinterpret_cast<const char *>(&pid), sizeof(pid));
        serialized.append(content);
        return serialized;
    }

    void Deserialize(std::string &&serialized)
    {
        assert_debug(serialized.size() >= sizeof(pid), "Invalid serialized message size: %zu",
                     serialized.size());
        auto pid_str = serialized.substr(0, sizeof(pid));
        memcpy(&pid, pid_str.c_str(), sizeof(pid));
        content = serialized.substr(sizeof(pid));
    }
};

Middleware::Middleware()
    : myself_({}),
      listening_server_({}),
      on_server_found_cb_(nullptr),
      on_server_lost_cb_(nullptr),
      me_({}),
      server_is_found_(false),
      server_published_(false),
      server_subscribed_(false)
{
    debug_enter();
    me_._pid = tipc_own_node();
}

Middleware::~Middleware()
{
    debug_enter();

    if (server_published_) {
        UnpublishServer();
    }
    if (server_subscribed_) {
        UnsubscribeFromServer();
    }
}

void Middleware::PublishServer(const ProcessSignature &server_signature)
{
    debug_enterp("{name %s sig %d}", server_signature._name.c_str(),
                 get_service_type(server_signature._signature));

    myself_ = server_signature;

    server_service_ = tipc_socket(SOCK_RDM);
    assert_errno(server_service_ > 0);
    assert_errno(tipc_sock_non_block(server_service_));
    assert_errno(tipc_bind(server_service_, get_service_type(server_signature._signature),
                           kTipcLowerPort, kTipcUpperPort, 0) == 0);

    struct tipc_addr sockid;
    tipc_sockaddr(server_service_, &sockid);
    char s_buffer[64];
    tipc_ntoa(&sockid, s_buffer, sizeof s_buffer);
    char c_buffer[64];
    debug("Bound server '%s' socket %s to %s", server_signature._name.c_str(),
          tipc_ntoa(&sockid, c_buffer, sizeof c_buffer),
          tipc_rtoa(get_service_type(server_signature._signature), kTipcLowerPort, kTipcUpperPort,
                    0, s_buffer, sizeof s_buffer));

    SubscribeToFdEvents(server_service_,
                        [](int service) { debug("Server service(%d) event received", service); });
}

void Middleware::UnpublishServer()
{
    debug_enter();

    if (!server_published_) {
        return;
    }

    UnsubscribeFromFdEvents(server_service_);
    tipc_close(server_service_);
    server_published_ = false;
}

void Middleware::SubscribeToServer(const ProcessSignature &server_signature,
                                   OnServerFoundCb &&on_server_found_event,
                                   OnServerLostCb &&on_server_lost_event)
{
    debug_enterp("%s", server_signature._name.c_str());

    listening_server_ = server_signature;
    on_server_found_cb_ = on_server_found_event;
    on_server_lost_cb_ = on_server_lost_event;

    subscription_service_ = tipc_topsrv_conn(0);
    assert_errno(subscription_service_ > 0);
    assert_errno(tipc_sock_non_block(subscription_service_));
    assert_errno(tipc_srv_subscr(subscription_service_,
                                 get_service_type(listening_server_._signature), kTipcLowerPort,
                                 kTipcUpperPort, true, kTipcWaitServer) == 0);

    struct tipc_addr sockid;
    tipc_sockaddr(subscription_service_, &sockid);
    char buffer[64];
    debug("Get TIPC top server socket at %s", tipc_ntoa(&sockid, buffer, sizeof buffer));

    SubscribeToFdEvents(subscription_service_,
                        [this](int service) { ProcessServerEvent(service); });

    server_subscribed_ = true;
}

void Middleware::UnsubscribeFromServer()
{
    debug_enter();

    if (!server_subscribed_) {
        return;
    }

    UnsubscribeFromFdEvents(subscription_service_);
    tipc_close(subscription_service_);
    server_subscribed_ = false;
}

void Middleware::SubscribeToFdEvents(int fd, OnFdEventCb &&on_fd_event)
{
    debug_enterp("%d", fd);

    fd_callbacks_[fd] = [fd, on_fd_event]() { on_fd_event(fd); };
    poll_fds_.insert(fd);
}

void Middleware::UnsubscribeFromFdEvents(int fd)
{
    debug_enterp("%d", fd);

    auto fd_search = fd_callbacks_.find(fd);
    if (fd_search != fd_callbacks_.end()) {
        poll_fds_.erase(fd);
        fd_callbacks_.erase(fd_search);
    }
}

bool Middleware::LoopWhile(bool *keeprunning)
{
    debug_enter();

    while (*keeprunning) {
        if (!Poll()) {
            return false;
        }
    }

    return true;
}

void Middleware::SendSignal(const Pid & /* pid */, const Signal & /* signal */,
                            const std::string & /* content */)
{
    /*
        SignalMessage message{};
        message.pid = me_;
        message.content = content;

        assert_debug(PidIsAlive(pid), "Can not send a signal to a dead process (PID %zu)",
       pid._pid);
        MessageQueue signal_queue(MessageQueue::Mode::Sender, pid, signal._name);
        assert_debug(signal_queue.Send(message.Serialize()), "%s",
       signal_queue.LastError().c_str());
    */
}

void Middleware::RegisterToSignal(const Signal &signal,
                                  OnSignalReceivedCb && /* on_signal_received */)
{
    debug_enterp("%s", signal._name.c_str());
    /*
        auto ret = signal_queues_.emplace(
            signal, MessageQueue(MessageQueue::Mode::Receiver, me_, signal._name));
        auto &queue = ret.first->second;

        SubscribeToFdEvents(queue.GetDescriptor(), [on_signal_received, &queue](int) {
            std::string received;
            assert_debug(queue.Receive(received), "%s", queue.LastError().c_str());
            SignalMessage message{};
            message.Deserialize(std::move(received));
            on_signal_received(std::move(message.pid), std::move(message.content));
        });
    */
}

void Middleware::ProcessServerEvent(int service)
{
    debug_enterp("%d", service);

    struct tipc_addr server = {};
    bool up;
    bool expired;
    assert_errno(tipc_srv_evt(service, nullptr, &server, &up, &expired) == 0);
    char buffer[64];
    debug("Get server socket at %s", tipc_ntoa(&server, buffer, sizeof buffer));
    debug("Server status: %s", expired ? "Expired" : up ? "Up" : "Down");
    assert_debug(!expired, "Subscription expired, waiting forever");

    Pid pid{};
    pid._pid = server.instance;
    if (pid._pid != 0) {
        if (up) {
            ServerFound(std::move(pid));
        } else {
            ServerLost(std::move(pid));
        }
    }
}

bool Middleware::PidIsAlive(const Pid &pid)
{
    debug_enterp("%zu", pid._pid);

    // return !(kill(pid._pid, 0) == -1 && errno == ESRCH);
    return true;
}

void Middleware::ServerLost(Pid &&pid)
{
    debug_enterp("%zu", pid._pid);

    if (!server_is_found_) {
        debug("not server_is_found_");
        return;
    }

    if (server_pids_.erase(pid) == 0) {
        return;
    }

    if (on_server_lost_cb_) {
        on_server_lost_cb_(std::move(pid));
    }

    server_is_found_ = server_pids_.empty();
}

void Middleware::ServerFound(Pid &&pid)
{
    debug_enterp("%zu", pid._pid);

    if (!server_pids_.insert(pid).second) {
        debug("not server_pids_.insert()");
        return;
    }

    server_is_found_ = !server_pids_.empty();

    if (on_server_found_cb_) {
        on_server_found_cb_(std::move(pid));
    }
}

bool Middleware::Poll()
{
    debug_enter();

    std::vector<pollfd> fds;
    fds.resize(poll_fds_.size());

    std::transform(poll_fds_.begin(), poll_fds_.end(), fds.begin(), [](int fd) -> pollfd {
        return pollfd{ .fd = fd, .events = POLLIN | POLLPRI | POLLHUP, .revents = 0 };
    });

    int ret = poll(fds.data(), fds.size(), -1 /* no timeout */);
    if (ret < 0) {
        return false;
    }

    for (const auto &fd : fds) {
        if (!ProcessPollEvent(&fd)) {
            return false;
        }
    }

    return true;
}

bool Middleware::ProcessPollEvent(const pollfd *fd)
{
    debug_enterp("%d", fd->fd);

    /** Process input events in fd */
    if (fd->revents & (POLLIN | POLLPRI)) {
        fd_callbacks_.at(fd->fd)();
    }

    /** Process errors in fd */
    if (fd->revents & (POLLERR | POLLHUP | POLLNVAL)) {
        debug_errno();
        return false;
    }

    return true;
}

}  // namespace stubs
}  // namespace mpsync