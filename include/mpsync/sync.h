// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_INCLUDE_MPSYNC_SYNC_H_
#define MPSYNC_INCLUDE_MPSYNC_SYNC_H_

#include <functional>

#include "mpsync/middleware.h"

namespace mpsync {

class DB;  //!< Forward declaration

class Sync {
   public:
    Sync(Middleware *mw, ProcessSignature process, bool db_owner);
    virtual ~Sync();

    const Sync &operator=(const Sync &) = delete;
    Sync &&operator=(Sync &&) = delete;
    Sync(const Sync &) = delete;
    Sync(Sync &&) = delete;

   protected:
    Middleware *mw_;
    DB *db_;
};

class SyncClient final : public Sync {
   public:
    SyncClient(Middleware *mw, ProcessSignature process);
    ~SyncClient();

    void RegisterServerFound(OnServerFoundCb on_server_found_event);
    void UnregisterServerFound();
    void RegisterServerLost(OnServerLostCb on_server_lost_event);
    void UnregisterServerLost();

    const SyncClient &operator=(const SyncClient &) = delete;
    SyncClient &&operator=(SyncClient &&) = delete;
    SyncClient(const SyncClient &) = delete;
    SyncClient(SyncClient &&) = delete;

   private:
    OnServerFoundCb on_server_found_cb_;
    OnServerLostCb on_server_lost_cb_;

    void OnServerFoundEvent(Pid &&pid);
    void OnServerLostEvent(Pid &&pid);
};

class SyncServer final : public Sync {
   public:
    SyncServer(Middleware *mw, ProcessSignature process);
    ~SyncServer();

    const SyncServer &operator=(const SyncServer &) = delete;
    SyncServer &&operator=(SyncServer &&) = delete;
    SyncServer(const SyncServer &) = delete;
    SyncServer(SyncServer &&) = delete;
};

}  // namespace mpsync

#endif  // MPSYNC_INCLUDE_MPSYNC_SYNC_H_