#include "shark/generator/enum_codegen.h"

#include <sstream>
#include <string>

#include "shark/idl/shark_options.pb.h"

namespace shark::generator {
namespace {

using ::google::protobuf::EnumDescriptor;
using ::google::protobuf::EnumOptions;
using ::google::protobuf::FieldDescriptor;
using ::google::protobuf::FileDescriptor;
using ::google::protobuf::FileOptions;
using ::google::protobuf::Descriptor;

using ::shark::idl::EnumStringSource;
using ::shark::idl::EnumUnderlyingType;
using ::shark::idl::SharkEnumOptions;
using ::shark::idl::SharkFieldOptions;
using ::shark::idl::SharkFileOptions;

struct EnumGenConfig {
  EnumUnderlyingType underlying = ::shark::idl::ENUM_UNDERLYING_TYPE_U32;
  EnumStringSource string_source = ::shark::idl::ENUM_STRING_SOURCE_CUSTOM_TABLE;
  bool bit_flags = false;
  bool gen_to_string = true;
  bool gen_from_string = true;
};

std::string ToCppType(EnumUnderlyingType type) {
  switch (type) {
    case ::shark::idl::ENUM_UNDERLYING_TYPE_U8:
      return "std::uint8_t";
    case ::shark::idl::ENUM_UNDERLYING_TYPE_U16:
      return "std::uint16_t";
    case ::shark::idl::ENUM_UNDERLYING_TYPE_U64:
      return "std::uint64_t";
    case ::shark::idl::ENUM_UNDERLYING_TYPE_U32:
    case ::shark::idl::ENUM_UNDERLYING_TYPE_UNSPECIFIED:
    default:
      return "std::uint32_t";
  }
}

EnumGenConfig ResolveConfig(const FileOptions& file_options, const EnumOptions& enum_options) {
  EnumGenConfig cfg;
  if (file_options.HasExtension(::shark::idl::shark_file)) {
    const SharkFileOptions& fo = file_options.GetExtension(::shark::idl::shark_file);
    if (fo.enum_underlying_type() != ::shark::idl::ENUM_UNDERLYING_TYPE_UNSPECIFIED) {
      cfg.underlying = fo.enum_underlying_type();
    }
    if (fo.enum_string_source() != ::shark::idl::ENUM_STRING_SOURCE_UNSPECIFIED) {
      cfg.string_source = fo.enum_string_source();
    }
  }
  if (enum_options.HasExtension(::shark::idl::shark_enum)) {
    const SharkEnumOptions& eo = enum_options.GetExtension(::shark::idl::shark_enum);
    if (eo.underlying_type() != ::shark::idl::ENUM_UNDERLYING_TYPE_UNSPECIFIED) {
      cfg.underlying = eo.underlying_type();
    }
    if (eo.string_source() != ::shark::idl::ENUM_STRING_SOURCE_UNSPECIFIED) {
      cfg.string_source = eo.string_source();
    }
    cfg.bit_flags = eo.bit_flags();
    cfg.gen_to_string = eo.gen_to_string();
    cfg.gen_from_string = eo.gen_from_string();
  }
  return cfg;
}

void emit_bit_ops(const EnumDescriptor& en, std::ostringstream& h) {
  const std::string name = en.name();
  h << "inline " << name << " operator|(" << name << " lhs, " << name << " rhs) {\n";
  h << "  using U = std::underlying_type_t<" << name << ">;\n";
  h << "  return static_cast<" << name << ">(static_cast<U>(lhs) | static_cast<U>(rhs));\n";
  h << "}\n";
  h << "inline " << name << " operator&(" << name << " lhs, " << name << " rhs) {\n";
  h << "  using U = std::underlying_type_t<" << name << ">;\n";
  h << "  return static_cast<" << name << ">(static_cast<U>(lhs) & static_cast<U>(rhs));\n";
  h << "}\n";
  h << "inline " << name << " operator^(" << name << " lhs, " << name << " rhs) {\n";
  h << "  using U = std::underlying_type_t<" << name << ">;\n";
  h << "  return static_cast<" << name << ">(static_cast<U>(lhs) ^ static_cast<U>(rhs));\n";
  h << "}\n";
  h << "inline " << name << " operator~(" << name << " v) {\n";
  h << "  using U = std::underlying_type_t<" << name << ">;\n";
  h << "  return static_cast<" << name << ">(~static_cast<U>(v));\n";
  h << "}\n";
}

void emit_enum(const FileDescriptor& file, const EnumDescriptor& en, std::ostringstream& h, std::ostringstream& cc) {
  const EnumGenConfig cfg = ResolveConfig(file.options(), en.options());

  h << "enum class " << en.name() << " : " << ToCppType(cfg.underlying) << " {\n";
  for (int i = 0; i < en.value_count(); ++i) {
    const auto* value = en.value(i);
    h << "  " << value->name() << " = " << value->number() << ",\n";
  }
  h << "};\n\n";

  if (cfg.bit_flags) {
    emit_bit_ops(en, h);
    h << "\n";
  }

  if (cfg.gen_to_string) {
    h << "std::string to_string(" << en.name() << " value);\n";
    cc << "std::string to_string(" << en.name() << " value) {\n";
    if (cfg.string_source == ::shark::idl::ENUM_STRING_SOURCE_PROTO_REFLECTION) {
      cc << "  const auto* desc = ::google::protobuf::DescriptorPool::generated_pool()"
            "->FindEnumTypeByName(\""
         << en.full_name() << "\");\n";
      cc << "  if (desc != nullptr) {\n";
      cc << "    const auto* v = desc->FindValueByNumber(static_cast<int>(value));\n";
      cc << "    if (v != nullptr) return std::string(v->name());\n";
      cc << "  }\n";
      cc << "  return std::to_string(static_cast<int>(value));\n";
    } else {
      cc << "  switch (value) {\n";
      for (int i = 0; i < en.value_count(); ++i) {
        const auto* value = en.value(i);
        cc << "    case " << en.name() << "::" << value->name() << ": return \"" << value->name() << "\";\n";
      }
      cc << "    default: return std::to_string(static_cast<int>(value));\n";
      cc << "  }\n";
    }
    cc << "}\n\n";
  }

  if (cfg.gen_from_string) {
    h << "bool from_string(std::string_view name, " << en.name() << "* out);\n";
    cc << "bool from_string(std::string_view name, " << en.name() << "* out) {\n";
    cc << "  if (out == nullptr) return false;\n";
    if (cfg.string_source == ::shark::idl::ENUM_STRING_SOURCE_PROTO_REFLECTION) {
      cc << "  const auto* desc = ::google::protobuf::DescriptorPool::generated_pool()"
            "->FindEnumTypeByName(\""
         << en.full_name() << "\");\n";
      cc << "  if (desc == nullptr) return false;\n";
      cc << "  const auto* v = desc->FindValueByName(std::string(name));\n";
      cc << "  if (v == nullptr) return false;\n";
      cc << "  *out = static_cast<" << en.name() << ">(v->number());\n";
      cc << "  return true;\n";
    } else {
      for (int i = 0; i < en.value_count(); ++i) {
        const auto* value = en.value(i);
        cc << "  if (name == \"" << value->name() << "\") { *out = " << en.name() << "::" << value->name()
           << "; return true; }\n";
      }
      cc << "  return false;\n";
    }
    cc << "}\n\n";
  }
}

void emit_message_enum_field_notes(const Descriptor& msg, std::ostringstream& cc) {
  for (int i = 0; i < msg.field_count(); ++i) {
    const FieldDescriptor* field = msg.field(i);
    if (field->cpp_type() != FieldDescriptor::CPPTYPE_ENUM) continue;
    if (!field->options().HasExtension(::shark::idl::shark_field)) continue;
    const SharkFieldOptions& fo = field->options().GetExtension(::shark::idl::shark_field);
    cc << "// enum-field: " << msg.full_name() << "." << field->name()
       << " bit_flags=" << (fo.enum_bit_flags() ? "true" : "false")
       << " use_alias=" << (fo.enum_use_alias_name() ? "true" : "false")
       << "\n";
  }
  for (int i = 0; i < msg.nested_type_count(); ++i) {
    emit_message_enum_field_notes(*msg.nested_type(i), cc);
  }
}

void emit_enums_recursive(const FileDescriptor& file, const Descriptor& msg, std::ostringstream& h,
                        std::ostringstream& cc) {
  for (int i = 0; i < msg.enum_type_count(); ++i) {
    emit_enum(file, *msg.enum_type(i), h, cc);
  }
  for (int i = 0; i < msg.nested_type_count(); ++i) {
    emit_enums_recursive(file, *msg.nested_type(i), h, cc);
  }
}

}  // namespace

GeneratedEnumFiles generate_enum_files(const FileDescriptor& file) {
  GeneratedEnumFiles out;
  std::ostringstream h;
  std::ostringstream cc;

  h << "#pragma once\n";
  h << "#include <cstdint>\n";
  h << "#include <string>\n";
  h << "#include <string_view>\n";
  h << "#include <type_traits>\n\n";
  h << "namespace shark::sk {\n\n";

  cc << "#include \"" << file.name().substr(0, file.name().find_last_of('.')) << ".runtime.h\"\n";
  cc << "#include <google/protobuf/descriptor.h>\n";
  cc << "#include <string>\n\n";
  cc << "namespace shark::sk {\n\n";

  for (int i = 0; i < file.enum_type_count(); ++i) {
    emit_enum(file, *file.enum_type(i), h, cc);
  }
  for (int i = 0; i < file.message_type_count(); ++i) {
    emit_enums_recursive(file, *file.message_type(i), h, cc);
    emit_message_enum_field_notes(*file.message_type(i), cc);
  }

  h << "}  // namespace shark::sk\n";
  cc << "\n}  // namespace shark::sk\n";

  out.header = h.str();
  out.source = cc.str();
  return out;
}

}  // namespace shark::generator
