// Copyright © 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "mpsync/db.h"

#include <sys/stat.h>
#include <cassert>
#include <iostream>

#include "rocksdb/db.h"

namespace mpsync {

DB::DB(const std::string &db_name, const Options &options)
{
    mkdir(kDBBasePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    switch (options.operation_mode) {
        case OperationMode::Master:
            assert(OpenMasterMode(kDBBasePath + db_name));
            break;
        case OperationMode::Slave:
            OpenSlaveMode(kDBBasePath + db_name);
            break;
    }
}

DB::~DB()
{
    delete db_;
}

bool DB::Put(const std::string &key, const std::string &value)
{
    if (!db_) {
        return false;
    }

    rocksdb::Status status = db_->Put(rocksdb::WriteOptions(), key, value);
    std::cout << "db->Put([" << key << "] = " << value << ") = " << status.ToString() << std::endl;
    return status.ok();
}

bool DB::Get(const std::string &key, std::string &value)
{
    if (!db_) {
        return false;
    }

    rocksdb::Status status = db_->Get(rocksdb::ReadOptions(), key, &value);
    std::cout << "db->Get(" << key << ") = " << status.ToString() << std::endl;
    return status.ok();
}

bool DB::Delete(const std::string &key)
{
    if (!db_) {
        return false;
    }

    rocksdb::Status status = db_->Delete(rocksdb::WriteOptions(), key);
    std::cout << "db->Delete(" << key << ") = " << status.ToString() << std::endl;
    return status.ok();
}

bool DB::OpenMasterMode(const std::string &db_path)
{
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db_);
    std::cout << "DB::Open(" << db_path << ") = " << status.ToString() << std::endl;
    return status.ok();
}

bool DB::OpenSlaveMode(const std::string &db_path)
{
    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(rocksdb::Options(), db_path, &db_);
    std::cout << "DB::OpenForReadOnly(" << db_path << ") = " << status.ToString() << std::endl;
    return status.ok();
}

} /* mpsync namespace */