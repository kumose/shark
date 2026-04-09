#include <cstdint>
#include <iostream>
#include <string>

#include "tests/proto/enum_runtime.runtime.h"

namespace {

template <typename E>
constexpr std::underlying_type_t<E> to_underlying(E value) {
  return static_cast<std::underlying_type_t<E>>(value);
}

}  // namespace

int main() {
  using shark::sk::Color;
  using shark::sk::Permission;
  using shark::sk::from_string;
  using shark::sk::to_string;

  if (sizeof(Permission) != 1) {
    std::cerr << "Permission must use 1-byte underlying type\n";
    return 1;
  }

  Permission rw = Permission::PERMISSION_READ | Permission::PERMISSION_WRITE;
  if (to_underlying(rw) != 3) {
    std::cerr << "bitwise or failed for Permission enum\n";
    return 1;
  }

  if (to_string(Permission::PERMISSION_EXECUTE) != "PERMISSION_EXECUTE") {
    std::cerr << "custom-table to_string failed for Permission\n";
    return 1;
  }

  Color parsed = Color::COLOR_UNSPECIFIED;
  if (!from_string("COLOR_GREEN", &parsed) || parsed != Color::COLOR_GREEN) {
    std::cerr << "from_string failed for Color\n";
    return 1;
  }

  if (to_string(Color::COLOR_RED) != "COLOR_RED") {
    std::cerr << "reflection to_string failed for Color\n";
    return 1;
  }

  std::cout << "enum runtime pass\n";
  return 0;
}
