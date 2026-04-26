
#include <shark/toml/fwd/color_fwd.h>
#include <shark/toml/version.h>

#include <ostream>

namespace xconfig {
    inline  namespace
    TOML11_INLINE_VERSION_NAMESPACE {
        namespace color {
            // put ANSI escape sequence to ostream
             namespace ansi {
                 std::ostream &reset(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[00m"; }
                    return os;
                }

                 std::ostream &bold(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[01m"; }
                    return os;
                }

                 std::ostream &grey(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[30m"; }
                    return os;
                }

                 std::ostream &gray(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[30m"; }
                    return os;
                }

                 std::ostream &red(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[31m"; }
                    return os;
                }

                 std::ostream &green(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[32m"; }
                    return os;
                }

                 std::ostream &yellow(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[33m"; }
                    return os;
                }

                 std::ostream &blue(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[34m"; }
                    return os;
                }

                 std::ostream &magenta(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[35m"; }
                    return os;
                }

                 std::ostream &cyan(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[36m"; }
                    return os;
                }

                 std::ostream &white(std::ostream &os) {
                    if (detail::color_status().should_color()) { os << "\033[37m"; }
                    return os;
                }
            } // ansi
        } // color
    } // TOML11_INLINE_VERSION_NAMESPACE
} // xconfig