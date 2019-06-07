// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_INCLUDE_STUBS_MESSAGE_SOCKET_H_
#define MPSYNC_INCLUDE_STUBS_MESSAGE_SOCKET_H_

#include <string>

#include "mpsync/pid.h"

namespace mpsync {
namespace stubs {

class MessageSocket final {
   public:
    enum class Mode {
        None,
        Sender,
        Receiver
    };

    MessageSocket(Mode mode, const Pid &pid, const std::string &name);
    MessageSocket(MessageSocket &&);
    MessageSocket &operator=(MessageSocket &&);
    MessageSocket(const MessageSocket &) = delete;
    MessageSocket &operator=(const MessageSocket &) = delete;
    ~MessageSocket();
    int GetDescriptor() const;
    bool Send(const std::string &message);
    bool Receive(std::string &message);
    std::string LastError();

   private:
    Mode mode_;
    std::string socket_address_;
    int socket_;
    size_t msg_size_;
    std::string last_error_;
};

}  // namespace stubs
}  // namespace mpsync

#endif  // MPSYNC_INCLUDE_STUBS_MESSAGE_SOCKET_H_