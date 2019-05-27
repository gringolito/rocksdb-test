// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#include <unistd.h>

#include <cassert>
#include <iostream>

#include "mpsync/db.h"

namespace mpsync {

std::string CreateJson() {
    return "";
}

void PutIntoDB(std::string &&key, std::string &&value)
{
    DB::Options options;
    options.operation_mode = DB::OperationMode::Master;
    DB db("test", options);

    assert(db.Put(key, value));
    assert(db.Put("json", CreateJson()));

    sleep(15);
    std::cout << " -- Releasing DB LOCK --" << std::endl;
}

} /* mpsync namespace */

int main(int argc, const char *argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <key> <value>" << std::endl;
        return 1;
    }

    mpsync::PutIntoDB(argv[1], argv[2]);

    return 0;
}