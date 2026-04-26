#ifndef TOML11_LITERAL_FWD_HPP
#define TOML11_LITERAL_FWD_HPP

#include <shark/toml/location.h>
#include <shark/toml/types.h>
#include <shark/toml/version.h> // IWYU pragma: keep for TOML11_HAS_CHAR8_T

namespace xconfig
{
inline namespace TOML11_INLINE_VERSION_NAMESPACE
{

namespace detail
{
// implementation
::xconfig::value literal_internal_impl(location loc);
} // detail

inline namespace literals
{
inline namespace toml_literals
{

::xconfig::value operator""_toml(const char* str, std::size_t len);

#if defined(TOML11_HAS_CHAR8_T)
// value of u8"" literal has been changed from char to char8_t and char8_t is
// NOT compatible to char
::xconfig::value operator"" _toml(const char8_t* str, std::size_t len);
#endif

} // toml_literals
} // literals
} // TOML11_INLINE_VERSION_NAMESPACE
} // toml
#endif // TOML11_LITERAL_FWD_HPP
