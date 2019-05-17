// Copyright Â© 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "stubs/middleware.h"

#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cassert>

#include <fstream>

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
    fd_callbacks_[fd] = on_fd_event;
    // add to poll fds
}

void Middleware::UnsubscribeFromFdEvents(int fd)
{
    auto fd_search = fd_callbacks_.find(fd);
    if (fd_search != fd_callbacks_.end()) {
        // remove from poll fds
        fd_callbacks_.erase(fd_search);
    }
}

void Middleware::LoopWhile(bool *keeprunning)
{
    while (*keeprunning) {
        // poll();
    }
}

void Middleware::WatchServerPid()
{
    inotify_ = inotify_init1(IN_NONBLOCK);
    assert(inotify_ > 0);
    watch_ = inotify_add_watch(inotify_, std::string(kPidPath + listening_server_._name).c_str(),
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

}  // namespace stubs
}  // namespace mpsync