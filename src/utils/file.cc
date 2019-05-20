// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------
#include "utils/file.h"

#include <sys/stat.h>

#include <fstream>

#include "utils/dir.h"

namespace mpsync {
namespace utils {

bool File::Exists(const std::string &file)
{
    struct stat fstat;
    if (stat(file.c_str(), &fstat)) {
        return false;
    }
    return S_ISREG(fstat.st_mode);
}

bool File::Create(const std::string &file)
{
    auto base = file.find_last_of('/');
    return Create(file.substr(0, base), file.substr(base + 1));
}

bool File::Create(const std::string &path, const std::string &filename)
{
    if (!Dir::Exists(path)) {
        if (!Dir::Create(path)) {
            return false;
        }
    }

    std::ofstream f(path + '/' + filename, std::ofstream::binary | std::ofstream::trunc);
    return f.good();
}

}  // namespace utils
}  // namespace mpsync