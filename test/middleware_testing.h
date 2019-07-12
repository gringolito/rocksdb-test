// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_TEST_MIDDLEWARE_TESTING_H_
#define MPSYNC_TEST_MIDDLEWARE_TESTING_H_

#include <algorithm>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include "stubs/middleware.h"

namespace mpsync {
namespace test {

/* Service / Signals definitions */
static constexpr uint32_t kMiddlewareTestingService{ 64 };  // 0 to 63 is system reserved
static constexpr uint32_t kSignalResquestToStopService{(kMiddlewareTestingService << 16) | 1 };
static const Signal kSignalResquestToStop{ ._name = "RequestToStop",
                                           ._signature = &kSignalResquestToStopService };
static constexpr uint32_t kSignalResquestToDieService{(kMiddlewareTestingService << 16) | 2 };
static const Signal kSignalResquestToDie{ ._name = "RequestToDie",
                                          ._signature = &kSignalResquestToDieService };

class Server final {
   public:
    explicit Server(const ProcessSignature &server) : server_(server)
    {
    }

    ~Server()
    {
        Stop();
    }

    void Start()
    {
        pid_t pid = fork();

        if (pid == 0) {
            Run();
            exit(0);
        } else {
            ASSERT_GT(pid, 0) << "Server fork failed!";
            forked_pids_.push_back(pid);
        }
    }

    void Stop()
    {
        for (auto pid : forked_pids_) {
            EXPECT_EQ(0, kill(pid, SIGTERM));
        }
        forked_pids_.clear();
    }

    void Stop(Pid &&server_pid)
    {
        EXPECT_EQ(0, kill(server_pid._pid, SIGTERM));
        std::remove(forked_pids_.begin(), forked_pids_.end(), server_pid._pid);
    }

    void Restart()
    {
        auto num_servers = forked_pids_.size();
        Stop();
        while (num_servers--) {
            Start();
        }
    }

    void Restart(Pid &&server_pid)
    {
        Stop(std::move(server_pid));
        Start();
    }

    void Kill()
    {
        Stop();
    }

    void Kill(Pid &&server_pid)
    {
        Stop(std::move(server_pid));
    }

   private:
    void Run()
    {
        bool keeprunning = true;
        Middleware *mw = new stubs::Middleware();
        mw->RegisterToSignal(kSignalResquestToStop,
                             [&keeprunning](Pid &&, std::string &&) { keeprunning = false; });
        mw->RegisterToSignal(kSignalResquestToDie,
                             [](Pid &&, std::string &&) { exit(EXIT_FAILURE); });
        mw->PublishServer(server_);
        mw->LoopWhile(&keeprunning);
        delete mw;
    }

    ProcessSignature server_;
    std::vector<pid_t> forked_pids_;
};

} /* namespace test */
} /* namespace mpsync */

#endif /* MPSYNC_TEST_MIDDLEWARE_TESTING_H_ */