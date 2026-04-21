// SPDX-License-Identifier: BSD-3-Clause
// Based on Google Protobuf (https://github.com/protocolbuffers/protobuf) and protobuf-c
// (https://github.com/protobuf-c/protobuf-c)
// Copyright 2008 Google Inc., 2008-2025 protobuf-c authors. Modifications for C++ generation.
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


#pragma once


#include <memory>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/stubs/common.h>
#include <shark/idl/shark_options.pb.h>
#include <turbo/utility/status.h>
#include <shark/generator/helpers.h>
#include <shark/generator/global_state.h>
namespace shark {
    class FieldGenerator {
    public:
        explicit FieldGenerator(const google::protobuf::FieldDescriptor *descriptor, std::string message) : descriptor_(descriptor), _message(std::move(message)) {
            variables_["message"] = _message;
            variables_["default_init"] = "";
            _option = descriptor_->options();
            _ext_option = descriptor_->options().GetExtension(idl::shark_field);
            if (descriptor_->options().deprecated()) {
                variables_["deprecated"] = "[[deprecated]]";
            }
            GlobalState::instance().registry(descriptor_);
        }

        virtual ~FieldGenerator();

        // Generate definitions to be included in the structure.
        virtual void generate_members(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_members_declares(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_members_inline_implementations(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_trans_parse_pb_implementations(google::protobuf::io::Printer *printer) const = 0;
        virtual void generate_trans_to_pb_implementations(google::protobuf::io::Printer *printer) const = 0;

        virtual void generate_move_ctor_define(google::protobuf::io::Printer *printer) const = 0;
        virtual void generate_copy_ctor_define(google::protobuf::io::Printer *printer) const = 0;

        // Generate a static initializer for this field.
        virtual void GenerateDescriptorInitializer(google::protobuf::io::Printer *printer) const = 0;

        virtual void GenerateDefaultValueImplementations(google::protobuf::io::Printer *printer) const {
        }

        virtual std::string get_default_value() const = 0;

        // Generate members to initialize this field from a static initializer
        virtual void GenerateStaticInit(google::protobuf::io::Printer *printer) const = 0;

        void set_default(const std::string value, bool cap = false) {
            if (value.empty()) {
                return;
            }
            if (cap) {
                variables_["default_init"] = turbo::str_cat("{\"", value, "\"}");
                return;
            }

            variables_["default_init"] = turbo::str_cat("{", value, "}");

        }

    protected:
        void GenerateDescriptorInitializerGeneric(google::protobuf::io::Printer *printer,
                                                  bool optional_uses_has,
                                                  const std::string &type_macro,
                                                  const std::string &descriptor_addr) const;

        const google::protobuf::FieldDescriptor *descriptor_;
        std::string _message;
        std::map<std::string, std::string> variables_;
        idl::SharkFieldOptions _ext_option;
        google::protobuf::FieldOptions _option;
    };

    // Convenience class which constructs FieldGenerators for a Descriptor.
    class FieldGeneratorMap {
    public:
        explicit FieldGeneratorMap(const google::protobuf::Descriptor *descriptor, std::string message);

        ~FieldGeneratorMap();

        const FieldGenerator &get(const google::protobuf::FieldDescriptor *field) const;

    private:
        const google::protobuf::Descriptor *descriptor_;
        std::vector<std::unique_ptr<FieldGenerator>> field_generators_;

        static std::unique_ptr<FieldGenerator> MakeGenerator(const google::protobuf::FieldDescriptor *field, std::string message);
    };
} // namespace shark
