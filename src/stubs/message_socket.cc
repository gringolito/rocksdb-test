// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "stubs/message_socket.h"

#include <unistd.h>

#include <sstream>
#include <tuple>

#include "sys/socket.h"
#include "sys/types.h"

#include "utils/assert.h"
#include "utils/debug.h"

namespace mpsync {
namespace stubs {

// static constexpr int kSenderFlags = O_NONBLOCK | O_WRONLY;
// static constexpr int kReceiverFlags = O_NONBLOCK | O_CREAT | O_EXCL | O_RDONLY;

MessageSocket::MessageSocket(Mode mode, const Pid &pid, const std::string &name)
    : mode_(mode), socket_address_(name)
{
    debug_enterp("%s", name.c_str());
    std::ignore = pid;
    switch (mode) {
        case Mode::Sender:
            break;
        case Mode::Receiver:
            break;
        case Mode::None:
            fault_debug("Invalid MessageSocket::Mode::None received on MessageSocket creation");
            socket_ = -1;
            break;
    }

    assert_errno(socket_ > 0);
}

MessageSocket::MessageSocket(MessageSocket &&other)
{
    debug_enterp("%s", other.socket_address_.c_str());
    *this = std::move(other);
}

MessageSocket &MessageSocket::operator=(MessageSocket &&other)
{
    debug_enterp("%s", other.socket_address_.c_str());
    if (this != &other) {
        this->mode_ = other.mode_;
        other.mode_ = Mode::None;
        this->socket_address_ = std::move(other.socket_address_);
        this->socket_ = other.socket_;
        other.socket_ = 0;
        this->msg_size_ = other.msg_size_;
        other.msg_size_ = 0;
        this->last_error_ = std::move(other.last_error_);
    }
    return *this;
}

MessageSocket::~MessageSocket()
{
    debug_enterp("%s", socket_address_.c_str());
    if (socket_ > 0) {
        close(socket_);
    }
}

int MessageSocket::GetDescriptor() const
{
    return socket_;
}

bool MessageSocket::Send(const std::string &message)
{
    assert_debug(message.size() <= msg_size_, "message too big: size %zu (maximum: %zu)",
                 message.size(), msg_size_);
    std::stringstream err;
    err << "socket " << socket_ << " (" << socket_address_ << ") message send failed (message size "
        << message.size() << "): " << errno << " (" << strerror(errno) << ")";
    last_error_ = err.str();
    return false;
}

bool MessageSocket::Receive(std::string &message)
{
    std::stringstream err;
    err << "socket " << socket_ << " (" << socket_address_ << ") message receive failed: " << errno
        << " (" << strerror(errno) << ")";
    last_error_ = err.str();
    return false;
}

std::string MessageSocket::LastError()
{
    return std::move(last_error_);
}

}  // namespace stubs
}  // namespace mpsync