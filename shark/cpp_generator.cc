#include <string>

#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/io/printer.h"
#include "google/protobuf/io/zero_copy_stream.h"

#include "shark/generator/enum_codegen.h"

namespace {

class SharkCppGenerator final : public google::protobuf::compiler::CodeGenerator {
 public:
  bool Generate(const google::protobuf::FileDescriptor* file, const std::string&,
                google::protobuf::compiler::GeneratorContext* context, std::string* error) const override {
    if (file == nullptr) {
      *error = "input file descriptor is null";
      return false;
    }

    const auto generated = shark::generator::generate_enum_files(*file);
    const std::string base = file->name().substr(0, file->name().find_last_of('.'));

    {
      std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> out(context->Open(base + ".runtime.h"));
      google::protobuf::io::Printer printer(out.get(), '$');
      printer.Print("$content$", "content", generated.header);
    }

    {
      std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> out(context->Open(base + ".runtime.cc"));
      google::protobuf::io::Printer printer(out.get(), '$');
      printer.Print("$content$", "content", generated.source);
    }
    return true;
  }
};

}  // namespace

int main(int argc, char** argv) {
  SharkCppGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
