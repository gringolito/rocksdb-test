// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
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

Sync::Sync(Middleware *mw, ProcessSignature process, bool db_owner) : mw_(mw)
{
    DB::Options options;
    options.operation_mode = db_owner ? DB::OperationMode::Master : DB::OperationMode::Slave;
    db_ = new DB(process._name, options);
}

Sync::~Sync()
{
    delete db_;
}

}  // namespace mpsync