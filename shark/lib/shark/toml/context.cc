#include <shark/toml/context.h>
#include <shark/toml/types.h>
#include <shark/toml/version.h>

namespace xconfig {
    inline namespace
    TOML11_INLINE_VERSION_NAMESPACE {
        struct type_config;
        struct ordered_type_config;

        namespace detail {
            extern template class context<::xconfig::type_config>;
            extern template class context<::xconfig::ordered_type_config>;
        } // detail
    } // TOML11_INLINE_VERSION_NAMESPACE
} // xconfig
