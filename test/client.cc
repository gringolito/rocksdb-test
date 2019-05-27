// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#include "mpsync/sync.h"
#include "stubs/middleware.h"

#include "common.h"

namespace mpsync {
namespace test {

class Client final {
   public:
    Client()
    {
        mw_ = stubs::Middleware::Build();
        sync_ = new SyncClient(mw_, kTestProcess);
        sync_->RegisterServerFound([](const Pid &p) { printf("ServerFound(%zu)\n", p._pid); });
        sync_->RegisterServerLost([](const Pid &p) { printf("ServerLost(%zu)\n", p._pid); });
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