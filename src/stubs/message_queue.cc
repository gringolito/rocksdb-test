// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "stubs/message_queue.h"

#include <sstream>

#include "sys/stat.h"

#include "utils/assert.h"
#include "utils/debug.h"
#include "utils/dir.h"

namespace mpsync {
namespace stubs {

static const std::string kMessageQueueBaseName{ "/stubmw." };
static constexpr int kSenderFlags = O_NONBLOCK | O_WRONLY;
static constexpr int kReceiverFlags = O_NONBLOCK | O_CREAT | O_EXCL | O_RDONLY;
static constexpr mode_t kMessageQueueMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

MessageQueue::MessageQueue(Mode mode, const Pid &pid, const std::string &name)
    : mode_(mode), queue_path_(kMessageQueueBaseName + std::to_string(pid._pid) + "." + name)
{
    debug_enterp("%s", queue_path_.c_str());
    switch (mode) {
        case Mode::Sender:
            queue_ = mq_open(queue_path_.c_str(), kSenderFlags);
            break;
        case Mode::Receiver:
            queue_ = mq_open(queue_path_.c_str(), kReceiverFlags, kMessageQueueMode, nullptr);
            break;
        case Mode::None:
            fault_debug("Invalid MessageQueue::Mode::None received on MessageQueue creation");
            queue_ = -1;
            break;
    }

    assert_errno(queue_ > 0);
    mq_attr attributes;
    assert_errno(mq_getattr(queue_, &attributes) == 0);
    msg_size_ = attributes.mq_msgsize;
}

MessageQueue::MessageQueue(MessageQueue &&other)
{
    debug_enterp("%s", other.queue_path_.c_str());
    *this = std::move(other);
}

MessageQueue &MessageQueue::operator=(MessageQueue &&other)
{
    debug_enterp("%s", other.queue_path_.c_str());
    if (this != &other) {
        this->mode_ = other.mode_;
        other.mode_ = Mode::None;
        this->queue_path_ = std::move(other.queue_path_);
        this->queue_ = other.queue_;
        other.queue_ = 0;
        this->msg_size_ = other.msg_size_;
        other.msg_size_ = 0;
        this->last_error_ = std::move(other.last_error_);
    }
    return *this;
}

MessageQueue::~MessageQueue()
{
    debug_enterp("%s", queue_path_.c_str());
    if (queue_ > 0) {
        mq_close(queue_);
        if (mode_ == Mode::Receiver) {
            mq_unlink(queue_path_.c_str());
        }
    }
}

int MessageQueue::GetDescriptor() const
{
    return queue_;
}

bool MessageQueue::Send(const std::string &message)
{
    assert_debug(message.size() <= msg_size_, "message too big: size %zu (maximum: %zu)",
                 message.size(), msg_size_);
    if (mq_send(queue_, message.c_str(), message.size(), 0) != 0) {
        std::stringstream err;
        err << "queue " << queue_ << " (" << queue_path_ << ") message send failed (message size "
            << message.size() << "): " << errno << " (" << strerror(errno) << ")";
        last_error_ = err.str();
        return false;
    }
    return true;
}

bool MessageQueue::Receive(std::string &message)
{
    char buffer[msg_size_];
    auto size = mq_receive(queue_, buffer, sizeof(buffer), nullptr);
    if (size < 0) {
        std::stringstream err;
        err << "queue " << queue_ << " (" << queue_path_ << ") message receive failed: " << errno
            << " (" << strerror(errno) << ")";
        last_error_ = err.str();
        return false;
    }
    message = std::string(buffer, size);
    return true;
}

std::string MessageQueue::LastError()
{
    return std::move(last_error_);
}

}  // namespace stubs
}  // namespace mpsync