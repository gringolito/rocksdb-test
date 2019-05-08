// Copyright © 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#include <cassert>
#include <iostream>

#include "mpsync/db.h"

namespace mpsync {

void DelFromDB(std::string &&key)
{
    DB::Options options;
    options.operation_mode = DB::OperationMode::Master;
    DB db("test", options);

    assert(db.Delete(key));

    std::cout << " -- Releasing DB LOCK --" << std::endl;
}

} /* mpsync namespace */

int main(int argc, const char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <key>" << std::endl;
        return 1;
    }

    mpsync::DelFromDB(argv[1]);

    return 0;
}