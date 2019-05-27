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

class Server final {
   public:
    Server()
    {
        mw_ = new stubs::Middleware();
        sync_ = new SyncServer(mw_, kTestProcess);
    }

    ~Server()
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
    SyncServer *sync_;
};

}  // namespace test
}  // namespace mpsync

int main()
{
    auto *server = new mpsync::test::Server();

    server->Loop();

    delete server;
    return 0;
}