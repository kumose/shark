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



#include <map>

#include <google/protobuf/io/printer.h>

#include <shark/builder/enum.h>
#include <shark/utility/helpers.h>

#include <shark/uri.h>

namespace shark {
    EnumSkbGenerator::EnumSkbGenerator(const google::protobuf::EnumDescriptor *descriptor,
                                 const std::string &dllexport_decl)
        : descriptor_(descriptor),
          _dllexport_decl(dllexport_decl) {
    }

    EnumSkbGenerator::~EnumSkbGenerator() {
    }

    void EnumSkbGenerator::generate_definition(google::protobuf::io::Printer *printer, const google::protobuf::Descriptor* parent) {
        if (!parent) {
            return;
        }

        std::map<std::string, std::string> vars;
        vars["shortname"] = std::string(descriptor_->name());
        vars["domain"] = message_domain_without_namespace(parent);

        printer->Print(vars, "using $shortname$ = $domain$::$shortname$;\n\n");
    }

    void EnumSkbGenerator::generate_inline_declarations(google::protobuf::io::Printer* printer,
                                                 const google::protobuf::Descriptor* parent) {
    }

    void EnumSkbGenerator::generate_inline_definition(google::protobuf::io::Printer* printer,
                                               const google::protobuf::Descriptor* parent) {
    }



} // namespace shark
