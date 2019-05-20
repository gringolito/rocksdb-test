// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "stubs/middleware.h"

#include <unistd.h>
#include <cstdio>
#include <cassert>
#include <poll.h>

#include <algorithm>
#include <fstream>
#include <vector>

#include <sys/inotify.h>
#include <sys/types.h>

#include "utils/file.h"

namespace mpsync {
namespace stubs {

static const std::string kPidPath{"/tmp/stubmw/"};

Middleware::Middleware()
    : myself_({}),
      listening_server_({}),
      on_server_found_cb_(nullptr),
      on_server_lost_cb_(nullptr),
      server_pid_({}),
      inotify_(0),
      watch_(0),
      server_is_found_(false),
      server_published_(false)
{
}

Middleware::~Middleware()
{
    if (server_published_) {
        UnpublishServer();
    }

    if (watch_ > 0) {
        inotify_rm_watch(inotify_, watch_);
        close(watch_);
    }
    if (inotify_ > 0) {
        close(inotify_);
    }
}

void Middleware::PublishServer(const ProcessSignature &server_signature)
{
    myself_ = server_signature;

    std::ofstream pid_file(kPidPath + myself_._name, std::ofstream::binary | std::ofstream::trunc);
    pid_file << getpid() << std::endl;
    pid_file.close();
    server_published_ = true;
}

void Middleware::UnpublishServer()
{
    if (!server_published_) {
        return;
    }

    std::ofstream pid_file(kPidPath + myself_._name, std::ofstream::binary | std::ofstream::trunc);
    pid_file << 0 << std::endl;
    pid_file.close();
    server_published_ = false;
}

void Middleware::RegisterToServer(const ProcessSignature &server_signature,
                                  OnServerFoundCb on_server_found_event,
                                  OnServerLostCb on_server_lost_event)
{
    listening_server_ = server_signature;
    on_server_found_cb_ = on_server_found_event;
    on_server_lost_cb_ = on_server_lost_event;
    WatchServerPid();
}

void Middleware::SubscribeToFdEvents(int fd, OnFdEventCb on_fd_event)
{
    fd_callbacks_[fd] = [fd, on_fd_event]() { on_fd_event(fd); };
    poll_fds_.insert(fd);
}

void Middleware::UnsubscribeFromFdEvents(int fd)
{
    auto fd_search = fd_callbacks_.find(fd);
    if (fd_search != fd_callbacks_.end()) {
        poll_fds_.erase(fd);
        fd_callbacks_.erase(fd_search);
    }
}

bool Middleware::LoopWhile(bool *keeprunning)
{
    while (*keeprunning) {
        if (!Poll()) {
            return false;
        }
    }

    return true;
}

void Middleware::WatchServerPid()
{
    inotify_ = inotify_init1(IN_NONBLOCK);
    assert(inotify_ > 0);

    std::string pid_file(kPidPath + listening_server_._name);
    if (!utils::File::Exists(pid_file)) {
        assert(utils::File::Create(kPidPath, listening_server_._name));
    }

    watch_ = inotify_add_watch(inotify_, pid_file.c_str(),
                               IN_MODIFY | IN_CREATE | IN_DELETE_SELF);
    assert(watch_ > 0);

    SubscribeToFdEvents(watch_, [this](int watch) { ReadInotifyEvent(watch); });
}

void Middleware::ReadInotifyEvent(int watch)
{
    ssize_t size;
    const char *ptr;
    char buffer[BUFSIZ] __attribute__((aligned(__alignof__(inotify_event))));

    do {
        size = read(watch, buffer, sizeof buffer);
        if (size == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more events to read
                break;
            } else {
                perror("Failed to read inotify event");
                abort();
            }
        }

        const inotify_event *event;
        const char *buffer_end = buffer + size;
        for (ptr = buffer; ptr < buffer_end; ptr += sizeof(*event) + event->len) {
            event = reinterpret_cast<const inotify_event *>(ptr);
            ProcessInotifyEvent(event);
        }
    } while (size > 0);
}

void Middleware::ProcessInotifyEvent(const inotify_event *event)
{
    if (event->mask & (IN_CREATE | IN_MODIFY)) {
        auto pid = ReadPid();
        if (pid._pid != 0) {
            if (server_pid_._pid == pid._pid) {
                return;
            }

            /* A new server was found, so notify that the latest was lost */
            if (server_pid_._pid != 0) {
                ServerLost();
            }
            ServerFound(std::move(pid));
        } else {
            ServerLost();
        }
    } else if (event->mask & IN_DELETE_SELF) {
        ServerLost();
    }
}

Pid Middleware::ReadPid()
{
    std::ifstream pid_file(kPidPath + listening_server_._name, std::ofstream::binary);
    Pid pid;
    pid_file >> pid._pid;
    return pid;
}

void Middleware::ServerLost()
{
    if (!server_is_found_) {
        return;
    }

    if (on_server_lost_cb_) {
        on_server_lost_cb_(server_pid_);
    }

    server_pid_ = {};
    server_is_found_ = false;
}

void Middleware::ServerFound(Pid &&pid)
{
    server_is_found_ = true;
    server_pid_ = std::move(pid);

    if (on_server_found_cb_) {
        on_server_found_cb_(server_pid_);
    }
}

bool Middleware::Poll()
{
    std::vector<pollfd> fds;
    fds.resize(poll_fds_.size());

    std::transform(poll_fds_.begin(), poll_fds_.end(), fds.begin(), [](int fd) -> pollfd {
        return pollfd{.fd = fd, .events = POLLIN | POLLPRI, .revents = 0};
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
    /** Process input events in fd */
    if (fd->revents & (POLLIN | POLLPRI)) {
        fd_callbacks_.at(fd->fd)();
    }

    /** Process errors in fd */
    if (fd->revents & (POLLERR | POLLHUP | POLLNVAL)) {
        return false;
    }

    return true;
}

}  // namespace stubs
}  // namespace mpsync