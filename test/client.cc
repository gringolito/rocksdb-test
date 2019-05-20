// Copyright © 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#include "mpsync/sync.h"
#include "stubs/middleware.h"

namespace mpsync {
namespace test {

static const ProcessSignature kTestProcess{._name = "test", ._signature = nullptr};

class Client final {
   public:
    Client()
    {
        mw_ = new stubs::Middleware();
        sync_ = new Sync(mw_, kTestProcess);
        sync_->RegisterServerFound([this](const Pid &) { return; });
        sync_->RegisterServerLost([this](const Pid &) { return; });
    }

    ~Client()
    {
        delete sync_;
        delete mw_;
    }

    void Loop() {
        bool keeprunning = true;
        mw_->LoopWhile(&keeprunning);
    }

   private:
    Middleware *mw_;
    Sync *sync_;
};

}  // namespace test
}  // namespace mpsync

int main() {
    auto *client = new mpsync::test::Client();

    client->Loop();

    delete client;
    return 0;
}