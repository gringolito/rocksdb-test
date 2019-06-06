// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "stubs/linux/middleware.h"

#include <unistd.h>

#include <sys/inotify.h>

#include "utils/assert.h"
#include "utils/debug.h"
#include "utils/file.h"

namespace mpsync {
namespace stubs {

LinuxMiddleware::LinuxMiddleware() : Middleware(), inotify_(0), watch_(0)
{
    debug_enter();
}

LinuxMiddleware::~LinuxMiddleware()
{
    debug_enter();

    if (watch_ > 0) {
        inotify_rm_watch(inotify_, watch_);
        close(watch_);
    }
    if (inotify_ > 0) {
        close(inotify_);
    }
}

void LinuxMiddleware::WatchServerPid()
{
    debug_enter();

    inotify_ = inotify_init1(IN_NONBLOCK);
    assert_errno(inotify_ > 0);

    std::string pid_file(kPidPath + listening_server_._name);
    if (!utils::File::Exists(pid_file)) {
        assert_debug(utils::File::Create(kPidPath, listening_server_._name),
                     "Failed to create server PID file");
    }

    debug("inotify_add_watch(%s)", pid_file.c_str());
    watch_ = inotify_add_watch(inotify_, pid_file.c_str(),
                               IN_MODIFY | IN_CREATE | IN_DELETE_SELF | IN_CLOSE_WRITE);
    assert_errno(watch_ > 0);

    SubscribeToFdEvents(inotify_, [this](int) { ReadInotifyEvent(); });
}

void LinuxMiddleware::ReadInotifyEvent()
{
    debug_enter();
    ssize_t size;
    const char *ptr;
    char buffer[BUFSIZ] __attribute__((aligned(__alignof__(inotify_event))));

    do {
        size = read(inotify_, buffer, sizeof buffer);
        if (size == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more events to read
                break;
            } else {
                fault_debug_errno("Failed to read inotify event");
            }
        }

        const inotify_event *event;
        const char *buffer_end = buffer + size;
        for (ptr = buffer; ptr < buffer_end; ptr += sizeof(*event) + event->len) {
            event = reinterpret_cast<const inotify_event *>(ptr);
            if (event->wd == watch_) {
                ProcessInotifyEvent(event);
            }
        }
    } while (size > 0);
}

void LinuxMiddleware::ProcessInotifyEvent(const inotify_event *event)
{
    debug_enter();

    if (event->mask & (IN_CREATE | IN_MODIFY)) {
        ProcessPidFileEvent();
    } else if (event->mask & (IN_DELETE_SELF | IN_CLOSE_WRITE)) {
        ServerLost();
    }
}

}  // namespace stubs
}  // namespace mpsync