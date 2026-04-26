#ifndef TOML11_FROM_HPP
#define TOML11_FROM_HPP
#include <shark/toml/version.h>

namespace xconfig
{
inline namespace TOML11_INLINE_VERSION_NAMESPACE
{

template<typename T>
struct from;
// {
//     static T from_toml(const xconfig::value& v)
//     {
//         // User-defined conversions ...
//     }
// };

} // TOML11_INLINE_VERSION_NAMESPACE
} // xconfig
#endif // TOML11_FROM_HPP
