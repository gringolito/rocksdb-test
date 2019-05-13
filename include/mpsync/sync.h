// Copyright © 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#pragma(once)

#include <functional>

#include "mpsync/db.h"
#include "mpsync/middleware.h"

namespace mpsync {

class Sync final {
   public:
    explicit Sync(Middleware *mw, DB *db);
    ~Sync() = default;

    void RegisterServerFound(std::function<void(Pid &)> on_server_found_event);
    void UnregisterServerFound();
    void RegisterServerLost(std::function<void(Pid &)> on_server_lost_event);
    void UnregisterServerLost();

    const Sync &operator=(const Sync &) = delete;
    Sync &&operator=(Sync &&) = delete;
    Sync(const Sync &) = delete;
    Sync(Sync &&) = delete;

   private:
    Middleware *mw_;
    DB *db_;
};

}  // namespace mpsync