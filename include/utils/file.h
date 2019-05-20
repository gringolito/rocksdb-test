// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_INCLUDE_UTILS_FILE_H_
#define MPSYNC_INCLUDE_UTILS_FILE_H_

#include <string>

namespace mpsync {
namespace utils {

class File {
   public:
    static bool Exists(const std::string &file);
    static bool Create(const std::string &file);
    static bool Create(const std::string &path, const std::string &filename);
};

}  // namespace utils
}  // namespace mpsync

#endif  // MPSYNC_INCLUDE_UTILS_FILE_H_
