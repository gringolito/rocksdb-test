// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include <unistd.h>

#include <iostream>

#include "mpsync/sync.h"
#include "stubs/middleware.h"

#include "common.h"

namespace mpsync {
namespace test {

class Client final {
   public:
    Client() : mw_(new stubs::Middleware()), sync_(new SyncClient(mw_, kTestProcess))

    {
        sync_->RegisterServerFound([this](Pid &&server) {
            printf("ServerFound(%zu)\n", server._pid);
            server_pid_ = server;
        });
        sync_->RegisterServerLost([this](Pid &&server) {
            printf("ServerLost(%zu)\n", server._pid);
            server_pid_ = {};
        });
        mw_->SubscribeToFdEvents(STDIN_FILENO, [this](int) { ReadCli(); });
    }

    ~Client()
    {
        delete sync_;
        delete mw_;
    }

    void Loop()
    {
        bool keeprunning = true;
        mw_->LoopWhile(&keeprunning);
    }

   private:
    Middleware *mw_;
    SyncClient *sync_;
    Pid server_pid_;

    void ReadCli()
    {
        std::string command;
        std::cin >> command;
        mw_->SendSignal(server_pid_, kSignalName, command);
    }
};

}  // namespace test
}  // namespace mpsync

int main()
{
    auto *client = new mpsync::test::Client();

    client->Loop();

    delete client;
    return 0;
}