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
#include <shark/cpp/code_writer.h>
#include <shark/option/message.pb.h>
#include <shark/shark.pb.h>
#include <turbo/strings/string_builder.h>

namespace shark {

    class KmgenOption;
    class MessageOption;
    class FieldOption;

    class FieldGenerator {
    public:
        FieldGenerator() = default;

        ~FieldGenerator() = default;

        void generate_field_setter(const KmgenOption *file,
                              const MessageOption *message,
                              const FieldOption *field,
                              MessageBlock *hmessage, NamespaceBlock *nsblock);

        void generate_field_getter(const KmgenOption *file,
                              const MessageOption *message,
                              const FieldOption *field,
                              MessageBlock *hmessage, NamespaceBlock *nsblock);

        void generate_field_variable(const KmgenOption *file,
                              const MessageOption *message,
                              const FieldOption *field,
                              MessageBlock *hmessage, NamespaceBlock *nsblock);

        void generate_field_pb_serialize(const MessageOption *message, const FieldOption *field, LinesBuilder *builder, const std::string &pb_var);
        void generate_field_pb_deserialize(const MessageOption *message, const FieldOption *field, LinesBuilder *builder, const std::string &pb_var);
    private:

        void generate_field_normal_setter(const KmgenOption *file,
                      const MessageOption *message,
                      const FieldOption *field,
                      MessageBlock *hmessage, NamespaceBlock *nsblock);

        void generate_field_atomic_setter(const KmgenOption *file,
                      const MessageOption *message,
                      const FieldOption *field,
                      MessageBlock *hmessage, NamespaceBlock *nsblock);

        void generate_field_repeat_setter(const KmgenOption *file,
                      const MessageOption *message,
                      const FieldOption *field,
                      MessageBlock *hmessage, NamespaceBlock *nsblock);

        void generate_field_normal_getter(const KmgenOption *file,
                      const MessageOption *message,
                      const FieldOption *field,
                      MessageBlock *hmessage, NamespaceBlock *nsblock);
        void generate_field_atomic_getter(const KmgenOption *file,
              const MessageOption *message,
              const FieldOption *field,
              MessageBlock *hmessage, NamespaceBlock *nsblock);

        void generate_field_repeat_getter(const KmgenOption *file,
              const MessageOption *message,
              const FieldOption *field,
              MessageBlock *hmessage, NamespaceBlock *nsblock);


        void process_mutex(const std::string &prefix, const FieldOption *field, LinesBuilder *builder);
    public:
        std::string gen_setter_name(const std::string &field_name) {
            return "set_" + field_name;
        }

        std::string gen_getter_name(const std::string &field_name) {
            return "get_" + field_name;
        }

        std::string gen_append_name(const std::string &field_name) {
            return "append_" + field_name;
        }

        std::string gen_variable_name(const std::string &field_name) {
            return "_" + field_name;
        }

        std::string gen_variable_mutex_name(const std::string &field_name) {
            return "_" + field_name + "_mutex";
        }

    };

}  // namespace shark
