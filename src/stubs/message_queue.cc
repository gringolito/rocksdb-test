// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "stubs/message_queue.h"

#include <cassert>
#include <cstring>

#include "sys/stat.h"

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
    switch (mode) {
        case Mode::Sender:
            queue_ = mq_open(queue_path_.c_str(), kSenderFlags);
            break;
        case Mode::Receiver: {
            queue_ = mq_open(queue_path_.c_str(), kReceiverFlags, kMessageQueueMode, nullptr);
            break;
        }
    }

    assert(queue_ > 0);
    mq_attr attributes;
    assert(mq_getattr(queue_, &attributes) == 0);
    msg_size_ = attributes.mq_msgsize;
}

MessageQueue::~MessageQueue()
{
    mq_close(queue_);
    if (mode_ == Mode::Receiver) {
        mq_unlink(queue_path_.c_str());
    }
}

int MessageQueue::GetDescriptor() const
{
    return queue_;
}

bool MessageQueue::Send(const std::string &message) const
{
    assert(message.size() <= msg_size_);
    return mq_send(queue_, message.c_str(), message.size(), 0) == 0;
}

bool MessageQueue::Receive(std::string &message) const
{
    char buffer[msg_size_];
    auto size = mq_receive(queue_, buffer, sizeof(buffer), nullptr);
    if (size < 0) {
        return false;
    }
    message = std::string(buffer, size);
    return true;
}

}  // namespace stubs
}  // namespace mpsync