// Copyright (c) 2019 Filipe Utzig <filipeutzig@gmail.com>
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <filipeutzig@gmail.com> wrote this file. As long as you retain this notice
// you can do whatever you want with this stuff. If we meet some day, and you
// think this stuff is worth it, you can buy me a beer in return. Filipe Utzig.
// ----------------------------------------------------------------------------

#ifndef MPSYNC_INCLUDE_MPSYNC_SIGNAL_H_
#define MPSYNC_INCLUDE_MPSYNC_SIGNAL_H_

#include <string>
#include <tuple>

namespace mpsync {

struct Signal {
    std::string _name;
    const void *_signature;
};

static inline bool operator==(const Signal &lhs, const Signal &rhs)
{
    return std::tie(lhs._name, lhs._signature) == std::tie(rhs._name, rhs._signature);
}

}  // namespace mpsync

namespace std {

template <>
struct hash<mpsync::Signal> {
    std::size_t operator()(const mpsync::Signal &s) const noexcept
    {
        return std::hash<std::string>()(s._name);
    }
};

}  // namespace std

#endif  // MPSYNC_INCLUDE_MPSYNC_SIGNAL_H_