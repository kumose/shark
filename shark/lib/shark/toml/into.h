#ifndef TOML11_INTO_HPP
#define TOML11_INTO_HPP
#include <shark/toml/version.h>

namespace shark {
    inline namespace
    TOML11_INLINE_VERSION_NAMESPACE {
        template<typename T>
        struct into;
        // {
        //     static shark::Value into_toml(const T& user_defined_type)
        //     {
        //         // User-defined conversions ...
        //     }
        // };
    } // TOML11_INLINE_VERSION_NAMESPACE
} // shark
#endif // TOML11_INTO_HPP
