// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "utils/dir.h"

#include <set>
#include <functional>

#include <sys/stat.h>

namespace mpsync {
namespace utils {

/* Create directories with 0775 permitions */
constexpr mode_t kDefaultDirectoryCreateMode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

bool Dir::Exists(const std::string &path)
{
    struct stat fstat;
    if (stat(path.c_str(), &fstat)) {
        return false;
    }
    return S_ISDIR(fstat.st_mode);
}

bool Dir::Create(const std::string &path)
{
    std::set<std::string> create_paths_stack = { path };

    /* Find first existing path in tree to start creating for it */
    auto parent_path = path.substr(0, path.find_last_of('/'));
    while (!Exists(parent_path)) {
        create_paths_stack.insert(parent_path);
        parent_path = path.substr(0, parent_path.find_last_of('/'));
    }

    for (const auto &create_path : create_paths_stack) {
        if (Exists(create_path)) {
            /* Ignore if path is already create by another MakeDirectory() call.
             * If we receive double slashed paths or relative paths this will happen. */
            continue;
        }
        if (!MakeDirectory(create_path, kDefaultDirectoryCreateMode)) {
            return false;
        }
    }

    return true;
}

bool Dir::MakeDirectory(const std::string &path, mode_t mode)
{
    umask(0);
    return mkdir(path.c_str(), mode) == 0;
}

}  // namespace utils
}  // namespace mpsync