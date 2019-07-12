// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include <iostream>

#include "gtest/gtest.h"

#include "stubs/middleware.h"

#include "common.h"
#include "middleware_testing.h"

namespace mpsync {
namespace test {

class Middleware : public testing::Test {
   public:
    Middleware() : server_(kTestProcess), mw_(new stubs::Middleware())
    {
    }

    virtual ~Middleware()
    {
        delete mw_;
    }

   protected:
    Server server_;
    mpsync::Middleware *mw_;
    Pid server_pid_;
    std::set<Pid> multiple_server_pids_;
    bool keeprunning_;
};

TEST_F(Middleware, ServerFound)
{
    mw_->SubscribeToServer(kTestProcess, [this](Pid &&server) {
                                             server_pid_ = std::move(server);
                                             keeprunning_ = false;
                                         },
                           [](Pid &&) { ASSERT_FALSE(true) << "Not expecting ServerLost"; });

    server_.Start();

    keeprunning_ = true;
    EXPECT_EQ(true, mw_->LoopWhile(&keeprunning_));
    EXPECT_FALSE(keeprunning_);
    EXPECT_NE(Pid{}, server_pid_);
}

TEST_F(Middleware, ServerFoundMultiple)
{
    mw_->SubscribeToServer(kTestProcess, [this](Pid &&server) {
                                             EXPECT_NE(Pid{}, server);
                                             multiple_server_pids_.emplace(std::move(server));
                                             keeprunning_ = false;
                                         },
                           [](Pid &&) { ASSERT_FALSE(true) << "Not expecting ServerLost"; });

    server_.Start();
    server_.Start();

    keeprunning_ = true;
    EXPECT_EQ(true, mw_->LoopWhile(&keeprunning_));
    EXPECT_FALSE(keeprunning_);

    keeprunning_ = true;
    EXPECT_EQ(true, mw_->LoopWhile(&keeprunning_));
    EXPECT_FALSE(keeprunning_);

    EXPECT_EQ(2u, multiple_server_pids_.size());
}

TEST_F(Middleware, ServerUnpublished)
{
}

TEST_F(Middleware, ServerDies)
{
    mw_->SubscribeToServer(kTestProcess, [this](Pid &&server) {
                                             server_pid_ = std::move(server);
                                             keeprunning_ = false;
                                         },
                           [this](Pid &&server) {
                               EXPECT_EQ(server_pid_, server);
                               keeprunning_ = false;
                           });

    server_.Start();

    keeprunning_ = true;
    ASSERT_EQ(true, mw_->LoopWhile(&keeprunning_));
    ASSERT_FALSE(keeprunning_);
    ASSERT_NE(Pid{}, server_pid_);

    server_.Stop();

    keeprunning_ = true;
    EXPECT_EQ(true, mw_->LoopWhile(&keeprunning_));
    EXPECT_FALSE(keeprunning_);
}

TEST_F(Middleware, ServerRestart)
{
}

TEST_F(Middleware, SendSignal)
{
}

} /* namespace test */
} /* namespace mpsync */