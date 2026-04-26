#ifndef TOML11_INTO_HPP
#define TOML11_INTO_HPP
#include <shark/toml/version.h>

namespace xconfig {
    inline namespace
    TOML11_INLINE_VERSION_NAMESPACE {
        template<typename T>
        struct into;
        // {
        //     static xconfig::value into_toml(const T& user_defined_type)
        //     {
        //         // User-defined conversions ...
        //     }
        // };
    } // TOML11_INLINE_VERSION_NAMESPACE
} // xconfig
#endif // TOML11_INTO_HPP
