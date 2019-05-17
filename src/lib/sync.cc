// Copyright © 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "mpsync/sync.h"

#include "mpsync/db.h"

namespace mpsync {

Sync::Sync(Middleware *mw, ProcessSignature process) : mw_(mw)
{
    DB::Options options;
    options.operation_mode = DB::OperationMode::Master;
    db_ = new DB(process._name, options);

    mw_->RegisterToServer(process, [this](const Pid &p) { OnServerFoundEvent(p); },
                          [this](const Pid &p) { OnServerLostEvent(p); });
}

Sync::~Sync()
{
    delete db_;
}

void Sync::RegisterServerFound(OnServerFoundCb on_server_found_event)
{
    on_server_found_cb_ = on_server_found_event;
}

void Sync::UnregisterServerFound()
{
    on_server_found_cb_ = nullptr;
}

void Sync::RegisterServerLost(OnServerLostCb on_server_lost_event)
{
    on_server_lost_cb_ = on_server_lost_event;
}

void Sync::UnregisterServerLost()
{
    on_server_lost_cb_ = nullptr;
}

void Sync::OnServerFoundEvent(const Pid &pid)
{
    if (on_server_found_cb_) {
        on_server_found_cb_(pid);
    }
}

void Sync::OnServerLostEvent(const Pid &pid)
{
    if (on_server_lost_cb_) {
        on_server_lost_cb_(pid);
    }
}

}  // namespace mpsync