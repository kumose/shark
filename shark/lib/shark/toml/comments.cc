#include <shark/toml/fwd/comments_fwd.h> // IWYU pragma: keep
#include <shark/toml/version.h>

namespace shark {
    inline namespace
    TOML11_INLINE_VERSION_NAMESPACE {
        bool operator==(const preserve_comments &lhs, const preserve_comments &rhs) {
            return lhs.comments == rhs.comments;
        }

        bool operator!=(const preserve_comments &lhs, const preserve_comments &rhs) {
            return lhs.comments != rhs.comments;
        }

        bool operator<(const preserve_comments &lhs, const preserve_comments &rhs) {
            return lhs.comments < rhs.comments;
        }

        bool operator<=(const preserve_comments &lhs, const preserve_comments &rhs) {
            return lhs.comments <= rhs.comments;
        }

        bool operator>(const preserve_comments &lhs, const preserve_comments &rhs) {
            return lhs.comments > rhs.comments;
        }

        bool operator>=(const preserve_comments &lhs, const preserve_comments &rhs) {
            return lhs.comments >= rhs.comments;
        }

        void swap(preserve_comments &lhs, preserve_comments &rhs) {
            lhs.swap(rhs);
            return;
        }

        void swap(preserve_comments &lhs, std::vector<std::string> &rhs) {
            lhs.comments.swap(rhs);
            return;
        }

        void swap(std::vector<std::string> &lhs, preserve_comments &rhs) {
            lhs.swap(rhs.comments);
            return;
        }

        std::ostream &operator<<(std::ostream &os, const preserve_comments &com) {
            for (const auto &c: com) {
                if (c.front() != '#') {
                    os << '#';
                }
                os << c << '\n';
            }
            return os;
        }
    } // TOML11_INLINE_VERSION_NAMESPACE
} // toml11
