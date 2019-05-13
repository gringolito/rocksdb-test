// Copyright © 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#pragma(once)

#include <string>

namespace rocksdb {
class DB; /*!< Forward declaration */
}

namespace mpsync {

static const std::string kDBBasePath = "/tmp/dbs/";

class DB final {
   public:
    enum class OperationMode {
        Master,
        Slave,
    };

    struct Options {
        OperationMode operation_mode;
    };

    DB(const std::string &db_name, const Options &options);
    ~DB();

    bool Put(const std::string &key, const std::string &value);
    bool Get(const std::string &key, std::string &value);
    bool Delete(const std::string &key);

   private:
    rocksdb::DB *db_;

    bool OpenMasterMode(const std::string &db_path);
    bool OpenSlaveMode(const std::string &db_path);
};

}  // namespace mpsync