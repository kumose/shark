// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//



#include <memory>
#include <utility>
#include <vector>

#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <shark/idl/shark_options.pb.h>
#include <shark/generator/tml_file.h>
#include <shark/generator/generator.h>
#include <shark/utility/helpers.h>

namespace shark {
    // Parses a set of comma-delimited name/value pairs, e.g.:
    //   "foo=bar,baz,qux=corge"
    // parses to the pairs:
    //   ("foo", "bar"), ("baz", ""), ("qux", "corge")
    void ParseOptions(const std::string &text, std::vector<std::pair<std::string, std::string> > *output) {
        std::vector<std::string> parts = turbo::str_split(text, ",", turbo::SkipEmpty());

        for (unsigned i = 0; i < parts.size(); i++) {
            std::string::size_type equals_pos = parts[i].find_first_of('=');
            std::pair<std::string, std::string> value;
            if (equals_pos == std::string::npos) {
                value.first = parts[i];
                value.second = "";
            } else {
                value.first = parts[i].substr(0, equals_pos);
                value.second = parts[i].substr(equals_pos + 1);
            }
            output->push_back(value);
        }
    }

    SharkGenerator::SharkGenerator() {
    }

    SharkGenerator::~SharkGenerator() {
    }

    bool SharkGenerator::Generate(const google::protobuf::FileDescriptor *file,
                                  const std::string &parameter,
                                  google::protobuf::compiler::OutputDirectory *output_directory,
                                  std::string *error) const {
        if (file->options().GetExtension(idl::shark_file).no_generate())
            return true;

        std::vector<std::pair<std::string, std::string> > options;
        ParseOptions(parameter, &options);

        // -----------------------------------------------------------------
        // parse generator options

        // TODO(kenton):  If we ever have more options, we may want to create a
        //   class that encapsulates them which we can pass down to all the
        //   generator classes.  Currently we pass dllexport_decl down to all of
        //   them via the constructors, but we don't want to have to add another
        //   constructor parameter for every option.

        // If the dllexport_decl option is passed to the compiler, we need to write
        // it in front of every symbol that should be exported if this .proto is
        // compiled into a Windows DLL.  E.g., if the user invokes the protocol
        // compiler as:
        //   protoc --cpp_out=dllexport_decl=FOO_EXPORT:outdir foo.proto
        // then we'll define classes like this:
        //   class FOO_EXPORT Foo {
        //     ...
        //   }
        // FOO_EXPORT is a macro which should expand to __declspec(dllexport) or
        // __declspec(dllimport) depending on what is being compiled.
        std::string dllexport_decl;

        for (unsigned i = 0; i < options.size(); i++) {
            if (options[i].first == "dllexport_decl") {
                dllexport_decl = options[i].second;
            } else {
                *error = "Unknown generator option: " + options[i].first;
                return false;
            }
        }

        // -----------------------------------------------------------------
        GlobalState::instance().registry(file);
        {
            std::string basename = StripProto(file->name());
            basename.append(".tml");

            FileRtGenerator file_generator(file, dllexport_decl);

            // Generate header.
            {
                std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> output(
                    output_directory->Open(basename + ".h"));
                google::protobuf::io::Printer printer(output.get(), '$');
                file_generator.generate_header(&printer);
            }

            // Generate cc file.
            {
                std::unique_ptr<google::protobuf::io::ZeroCopyOutputStream> output(
                    output_directory->Open(basename + ".cc"));
                google::protobuf::io::Printer printer(output.get(), '$');
                file_generator.generate_source(&printer);
            }
        }

        return true;
    }
} // namespace shark
