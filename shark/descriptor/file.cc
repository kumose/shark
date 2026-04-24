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

#include <shark/descriptor/file.h>

namespace shark {

    // See generator.cc for the meaning of dllexport_decl.

    DescriptorFileGenerator::DescriptorFileGenerator(const google::protobuf::FileDescriptor *file,
                                 const std::string &dllexport_decl)
        : SubFileGeneratorBase(file) {
        for (int i = 0; i < file->message_type_count(); i++) {
            auto ptr = std::make_unique<MetaMessageGenerator>(file->message_type(i), dllexport_decl);
            message_generators_.push_back(std::move(ptr));
            if (!_have_one_of) {
                _have_one_of = message_generators_[i]->have_oneof();
            }
        }

    }


    void DescriptorFileGenerator::generate_fwd_typedef(google::protobuf::io::Printer *printer) {
        // Generate forward declarations of classes.
        for (int i = 0; i < _file->message_type_count(); i++) {
            message_generators_[i]->generate_fwd_typedef(printer);
        }
    }

    void DescriptorFileGenerator::generate_definition(google::protobuf::io::Printer *printer) {
        printer->Print("\n");

        // Generate class definitions.
        printer->Print("\n/// --- messages --- \n\n");
        for (int i = 0; i < _file->message_type_count(); i++) {
            message_generators_[i]->generate_definition(printer);
        }

    }

    void DescriptorFileGenerator::generate_inline_implement(google::protobuf::io::Printer *printer) {
        printer->Print("\n///////////////////////////////////////////////////////////\n");
        for (int i = 0; i < _file->message_type_count(); i++) {
            message_generators_[i]->generate_inline_implement(printer);
        }
    }

    void DescriptorFileGenerator::generate_static_variable(google::protobuf::io::Printer *printer) {

        for (int i = 0; i < _file->message_type_count(); i++) {
            message_generators_[i]->generate_static_variable(printer);
        }
    }

    void DescriptorFileGenerator::generate_static_functions(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < _file->message_type_count(); i++) {
            message_generators_[i]->generate_static_functions(printer);
        }

    }

    void DescriptorFileGenerator::generate_implement(google::protobuf::io::Printer *printer) {
        for (int i = 0; i < _file->message_type_count(); i++) {
            message_generators_[i]->generate_implement(printer);
        }
    }

}  // namespace shark
