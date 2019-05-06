#include <cassert>
#include <iostream>

#include "rocksdb/db.h"

int main() {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb", &db);
    std::cout << status.ToString() << std::endl;
    assert(status.ok());

    return 0;
}