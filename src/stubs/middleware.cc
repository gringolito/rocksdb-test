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

#include "tipcc.h"

#include "stubs/linux/middleware.h"
#include "utils/assert.h"
#include "utils/debug.h"

namespace mpsync {
namespace stubs {

#define get_service_type(_a) *(static_cast<uint32_t *>(_a))

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

Middleware *Middleware::Build()
{
#ifdef MPSYNC_STUBS_LINUX
    return new LinuxMiddleware();
#else
#warning "No stub middleware definition found, stub application may not run."
    return nullptr;
#endif
}

Middleware::Middleware()
    : listening_server_({}),
      myself_({}),
      on_server_found_cb_(nullptr),
      on_server_lost_cb_(nullptr),
      me_({}),
      server_pid_({}),
      server_is_found_(false),
      server_published_(false),
      first_poll_call_(true)
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
}

void Middleware::PublishServer(const ProcessSignature &server_signature)
{
    debug_enterp("{name %s sig %d}", server_signature._name.c_str(),
                 *(static_cast<uint32_t *>(server_signature._signature)));

    myself_ = server_signature;

    int server_service_ = tipc_socket(get_service_type(server_signature._signature));
    assert_errno(tipc_sock_non_block(service_socket));
    SubscribeToFdEvents(server_service_, [](int service) {
        debug("%d", service);
    });
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
    WatchServerPid();
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

void Middleware::SendSignal(const Pid &pid, const Signal &signal, const std::string &content)
{
    SignalMessage message{};
    message.pid = me_;
    message.content = content;

    assert_debug(PidIsAlive(pid), "Can not send a signal to a dead process (PID %zu)", pid._pid);
    MessageQueue signal_queue(MessageQueue::Mode::Sender, pid, signal._name);
    assert_debug(signal_queue.Send(message.Serialize()), "%s", signal_queue.LastError().c_str());
}

void Middleware::RegisterToSignal(const Signal &signal, OnSignalReceivedCb &&on_signal_received)
{
    debug_enterp("%s", signal._name.c_str());

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
}

void Middleware::ProcessPidFileEvent()
{
    debug_enter();

    auto pid = ReadPid();
    if (pid._pid != 0) {
        if (server_pid_._pid == pid._pid) {
            return;
        }

        /* A new server was found, so notify that the latest was lost */
        if (server_pid_._pid != 0) {
            ServerLost();
        }

        if (PidIsAlive(pid)) {
            ServerFound(std::move(pid));
        }
    } else {
        ServerLost();
    }
}

bool Middleware::PidIsAlive(const Pid &pid)
{
    debug_enterp("%zu", pid._pid);

    return !(kill(pid._pid, 0) == -1 && errno == ESRCH);
}

Pid Middleware::ReadPid()
{
    debug_enter();

    std::ifstream pid_file(kPidPath + listening_server_._name, std::ofstream::binary);
    Pid pid;
    pid_file >> pid._pid;
    return pid;
}

void Middleware::ServerLost()
{
    debug_enter();

    if (!server_is_found_) {
        debug("not server_is_found_");
        return;
    }

    if (on_server_lost_cb_) {
        on_server_lost_cb_(std::move(server_pid_));
    }

    server_pid_ = {};
    server_is_found_ = false;
}

void Middleware::ServerFound(Pid &&pid)
{
    debug_enterp("%zu", pid._pid);

    server_is_found_ = true;
    server_pid_ = pid;

    if (on_server_found_cb_) {
        on_server_found_cb_(std::move(pid));
    }
}

bool Middleware::Poll()
{
    debug_enter();

    if (first_poll_call_) {
        first_poll_call_ = false;
        if (!listening_server_._name.empty()) {
            ProcessPidFileEvent();
        }
    }

    std::vector<pollfd> fds;
    fds.resize(poll_fds_.size());

    std::transform(poll_fds_.begin(), poll_fds_.end(), fds.begin(), [](int fd) -> pollfd {
        return pollfd{ .fd = fd, .events = POLLIN | POLLPRI, .revents = 0 };
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