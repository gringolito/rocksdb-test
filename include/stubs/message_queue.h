// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_INCLUDE_STUBS_MESSAGE_QUEUE_H_
#define MPSYNC_INCLUDE_STUBS_MESSAGE_QUEUE_H_

#include <string>

#include <mqueue.h>

#include "mpsync/pid.h"

namespace mpsync {
namespace stubs {

class MessageQueue final {
   public:
    enum class Mode {
        Sender,
        Receiver
    };

    MessageQueue(Mode mode, const Pid &pid, const std::string &name);
    ~MessageQueue();
    int GetDescriptor() const;
    bool Send(const std::string &message) const;
    bool Receive(std::string &message) const;

   private:
    Mode mode_;
    std::string queue_path_;
    mqd_t queue_;
    size_t msg_size_;
};

}  // namespace stubs
}  // namespace mpsync

#endif  // MPSYNC_INCLUDE_STUBS_MESSAGE_QUEUE_H_