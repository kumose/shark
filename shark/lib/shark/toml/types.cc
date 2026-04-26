#include <shark/toml/value.h>
#include <shark/toml/types.h>
#include <shark/toml/version.h>

namespace xconfig {
    inline namespace
    TOML11_INLINE_VERSION_NAMESPACE {
        template class basic_value<type_config>;
        template class basic_value<ordered_type_config>;
    } // TOML11_INLINE_VERSION_NAMESPACE
} // xconfig
