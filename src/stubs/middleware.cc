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

#include <fstream>

namespace mpsync {
namespace stubs {

static const std::string kPidPath{"/tmp/stubmw/"};
static constexpr auto kInotifyEventSize = sizeof(inotify_event) + NAME_MAX + 1;

Middleware::Middleware()
{
}

Middleware::~Middleware()
{
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

    std::ofstream pid_file(kPidPath + server_signature._name,
                           std::ofstream::binary | std::ofstream::trunc);
    pid_file << getpid() << std::endl;
    pid_file.close();
}

void Middleware::UnpublishServer()
{
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

    SubscribeToFdEvents(watch_, [this](int watch) { InotifyEvent(watch); });
}

void Middleware::InotifyEvent(int watch)
{
    inotify_event *event = malloc(kInotifyEventSize);

    ssize_t size;
    do {
        size = read(watch, event, kInotifyEventSize);
        if (size == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more events to read
                break;
            }
            else {
                perror("Failed to read inotify event");
                abort();
            }

            if (event->mask & (IN_CREATE | IN_MODIFY) {
                pid = ReadPid();
                if (pid._pid != 0) {
                    if (server_pid_._pid == pid._pid) {
                        continue;
                    }

                    /* A new server was found, so notify that the latest was lost */
                    if (server_pid_._pid != 0) {
                        ServerLost();
                    }
                    ServerFound(pid);
                }
                else {
                    ServerLost();
                }
            }
            else if (event->mask & IN_DELETE_SELF) {
                ServerLost();
            }
        }
    } while (size > 0);
}  // namespace stubs

Pid Middleware::ReadPid()
{
    std::ifstream pid_file(kPidPath + server_signature._name, std::ofstream::binary);
    Pid pid;
    pid_file >> pid._pid;
    return pid;
}

void Middleware::ServerLost()
{
    if (server_pid._pid == 0) {
        return;
    }

    if (on_server_lost_cb) {
        on_server_lost_cb(server_pid_);
    }

    server_pid_ = {};
    server_was_lost_ = true;
}

void Middleware::ServerFound(Pid &&pid)
{
    server_was_found_ = true;
    server_pid_ = std::move(pid);
    if (on_server_found_cb) {
        on_server_found_cb(server_pid_);
    }
}

}  // namespace stubs
}  // namespace mpsync