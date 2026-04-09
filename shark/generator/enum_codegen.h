#pragma once

#include <string>

#include "google/protobuf/descriptor.h"

namespace shark::generator {

struct GeneratedEnumFiles {
  std::string header;
  std::string source;
};

GeneratedEnumFiles generate_enum_files(const google::protobuf::FileDescriptor& file);

}  // namespace shark::generator
