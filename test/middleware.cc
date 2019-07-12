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

TEST_F(Middleware, ServerFoundBeforeSubscription)
{
    server_.Start();

    mw_->SubscribeToServer(kTestProcess, [this](Pid &&server) {
                                             server_pid_ = std::move(server);
                                             keeprunning_ = false;
                                         },
                           [](Pid &&) { GTEST_FAIL() << "Not expecting ServerLost"; });

    keeprunning_ = true;
    EXPECT_EQ(true, mw_->LoopWhile(&keeprunning_));
    EXPECT_FALSE(keeprunning_);
    EXPECT_NE(Pid{}, server_pid_);
}

TEST_F(Middleware, ServerFoundAfterSubscription)
{
    mw_->SubscribeToServer(kTestProcess, [this](Pid &&server) {
                                             server_pid_ = std::move(server);
                                             keeprunning_ = false;
                                         },
                           [](Pid &&) { GTEST_FAIL() << "Not expecting ServerLost"; });

    server_.Start();

    keeprunning_ = true;
    EXPECT_EQ(true, mw_->LoopWhile(&keeprunning_));
    EXPECT_FALSE(keeprunning_);
    EXPECT_NE(Pid{}, server_pid_);
}

TEST_F(Middleware, ServerFoundMultiple)
{
    mw_->SubscribeToServer(kTestProcess,
                           [this](Pid &&server) {
                               EXPECT_NE(Pid{}, server);
                               EXPECT_TRUE(multiple_server_pids_.emplace(std::move(server)).second);
                               keeprunning_ = false;
                           },
                           [](Pid &&) { GTEST_FAIL() << "Not expecting ServerLost"; });

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
    server_.Start();

    bool server_found = false;
    bool server_lost = false;

    mw_->SubscribeToServer(kTestProcess, [this, &server_found](Pid &&server) {
                                             server_pid_ = std::move(server);
                                             server_found = true;
                                             keeprunning_ = false;
                                         },
                           [this, &server_lost](Pid &&server) {
                               EXPECT_EQ(server_pid_, server);
                               server_lost = true;
                               keeprunning_ = false;
                           });

    keeprunning_ = true;
    EXPECT_EQ(true, mw_->LoopWhile(&keeprunning_));
    EXPECT_FALSE(keeprunning_);
    EXPECT_NE(Pid{}, server_pid_);
    EXPECT_TRUE(server_found);
    EXPECT_FALSE(server_lost);

    Pid old_pid = server_pid_;
    server_.Restart();

    keeprunning_ = true;
    EXPECT_EQ(true, mw_->LoopWhile(&keeprunning_));
    EXPECT_FALSE(keeprunning_);
    EXPECT_TRUE(server_found);
    EXPECT_TRUE(server_lost);

    keeprunning_ = true;
    EXPECT_EQ(true, mw_->LoopWhile(&keeprunning_));
    EXPECT_FALSE(keeprunning_);
    EXPECT_TRUE(server_found);
    EXPECT_TRUE(server_lost);
    EXPECT_NE(old_pid, server_pid_);
}

TEST_F(Middleware, SendSignal)
{
}

} /* namespace test */
} /* namespace mpsync */