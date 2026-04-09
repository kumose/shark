// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <shark/cpp/code_writer.h>

namespace shark {

    class KmgenOption;
    class MessageOption;

    class CppGenerator : public google::protobuf::compiler::CodeGenerator {
    public:
        static const std::string kLicense;
        static const std::string kGenWarning;
        static const std::string kPragma;
    public:

        bool Generate(const google::protobuf::FileDescriptor *file,
                      const std::string &parameter,
                      google::protobuf::compiler::GeneratorContext *,
                      std::string *error) const override;

    private:
        void generate_prologue(const google::protobuf::FileDescriptor* file,
                    CodeWriter* writer) const;

        void GenerateCodeFor(const KmgenOption* file,
            const MessageOption* msg, NamespaceBlock* hns, NamespaceBlock* sns) const;
    };

}  // namespace shark
