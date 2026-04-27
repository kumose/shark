#ifndef TOML11_EXCEPTION_HPP
#define TOML11_EXCEPTION_HPP

#include <exception>

#include <shark/toml/version.h>

namespace shark
{
inline namespace TOML11_INLINE_VERSION_NAMESPACE
{

struct exception : public std::exception
{
  public:
    virtual ~exception() noexcept override = default;
    virtual const char* what() const noexcept override {return "";}
};

} // TOML11_INLINE_VERSION_NAMESPACE
} // shark
#endif // TOMl11_EXCEPTION_HPP
