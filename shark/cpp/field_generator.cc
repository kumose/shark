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

#include <shark/cpp/field_generator.h>
#include <turbo/strings/string_builder.h>

namespace shark {


    void FieldGenerator::generate_field_setter(const KmgenOption *file,
                              const MessageOption *message,
                              const FieldOption *field,
                              MessageBlock *hmessage, NamespaceBlock *nsblock) {
        if (field->is_atomic()) {
            generate_field_atomic_setter(file, message, field, hmessage, nsblock);
            return;
        }

        if (field->is_repeated() && !field->is_map()) {
            generate_field_repeat_setter(file, message, field, hmessage, nsblock);
            return;
        }
        generate_field_normal_setter(file, message, field, hmessage, nsblock);
    }

    void FieldGenerator::generate_field_repeat_setter(const KmgenOption *file,
                      const MessageOption *message,
                      const FieldOption *field,
                      MessageBlock *hmessage, NamespaceBlock *nsblock) {
        const std::string field_name = field->name();
        const std::string cpp_type = field->cpp_type();
        const std::string setter = gen_setter_name(field_name);
        const std::string appender = gen_append_name(field_name);

        /// declare
        LinesBuilder header_code;
        header_code.stream() << "void " << field_name << "_resize(size_t n);\n";
        header_code.stream() << "void " << field_name << "_reserve(size_t n);\n";
        header_code.stream() << "/// index must be < the field size.";
        header_code.stream() << "void " << setter << "(size_t index, " << field->km_type() << " value);\n";
        header_code.stream() << "void " << appender << "(" << field->km_type() << " value);";

        hmessage->publics_func.push_back(header_code.build());

        /// define
        LinesBuilder source_code;


        source_code.stream() << "void "<<message->km_full_name()<<"::"<< field_name << "_resize(size_t n) {";
        process_mutex("    ", field, &source_code);
        source_code.stream() << "    " << gen_variable_name(field_name)<<".resize(n);";
        source_code.stream() << "}\n";

        source_code.stream() << "void "<<message->km_full_name()<<"::"<< field_name << "_reserve(size_t n) {";
        process_mutex("    ", field, &source_code);
        source_code.stream() << "    " << gen_variable_name(field_name)<<".reserve(n);";
        source_code.stream() << "}\n";

        source_code.stream() << "void "<<message->km_full_name()<<"::"<< setter << "(size_t index, " << field->km_type() << " value) {";
        process_mutex("    ", field, &source_code);
        source_code.stream()<< "    " << gen_variable_name(field_name)<< "[index] = " << " std::move(value);";
        source_code.stream()<< "}\n";

        source_code.stream() << "void "<<message->km_full_name()<<"::"<< appender << "(" << field->km_type() << " value) {";
        process_mutex("    ", field, &source_code);
        source_code.stream() << "    " << gen_variable_name(field_name)<<".push_back(std::move(value));";
        source_code.stream() << "}\n";


        nsblock->blocks.push_back(source_code.build());

    }


    void FieldGenerator::generate_field_normal_setter(const KmgenOption *file,
                      const MessageOption *message,
                      const FieldOption *field,
                      MessageBlock *hmessage, NamespaceBlock *nsblock) {
        const std::string field_name = field->name();
        const std::string cpp_type = field->cpp_type();
        const std::string setter = gen_setter_name(field_name);

        /// declare
        LinesBuilder header_code;
        header_code.stream() << "void " << setter << "(" << cpp_type << " value);\n";
        hmessage->publics_func.push_back(header_code.build());

        /// define
        LinesBuilder source_code;
        source_code.stream() << "void "<<message->km_full_name()<<"::"<< setter << "(" << cpp_type << " value) {";
        process_mutex("", field, &source_code);
        source_code.stream() << "    " << gen_variable_name(field_name)<< " = " << " std::move(value);";
        source_code.stream() << "}\n";

        nsblock->blocks.push_back(source_code.build());
    }

    void FieldGenerator::generate_field_atomic_setter(const KmgenOption *file,
                  const MessageOption *message,
                  const FieldOption *field,
                  MessageBlock *hmessage, NamespaceBlock *nsblock) {
        const std::string field_name = field->name();
        const std::string cpp_type = field->cpp_type();
        const std::string setter = gen_setter_name(field_name);

        /// declare
        LinesBuilder builder;
        builder.stream()<< "void " << setter << "(" << cpp_type << " value);\n";
        hmessage->publics_func.push_back(builder.build());

        /// define
        LinesBuilder source_code;
        source_code.stream() << "void "<<message->km_full_name()<<"::"<< setter << "(" << cpp_type << " value) {";
        source_code.stream()<< "    " << gen_variable_name(field_name)<< ".store(value);";
        source_code.stream()<< "}\n";

        nsblock->blocks.push_back(source_code.build());
    }

    void FieldGenerator::process_mutex(const std::string &prefix, const FieldOption *field, LinesBuilder *builder) {
        if (!field->is_mutex()) {
            return;
        }
        const std::string field_name = field->name();
        builder->stream()<<prefix<<"std::unique_lock<std::mutex> lk("<<gen_variable_mutex_name(field_name)<<");";
    }

    void FieldGenerator::generate_field_getter(const KmgenOption *file,
                              const MessageOption *message,
                              const FieldOption *field,
                              MessageBlock *hmessage, NamespaceBlock *nsblock) {
        if (field->is_atomic()) {
            generate_field_atomic_getter(file, message, field, hmessage, nsblock);
            return;
        }

        if (field->is_repeated()&& !field->is_map()) {
            generate_field_repeat_getter(file, message, field, hmessage, nsblock);
            return;
        }
        generate_field_normal_getter(file, message, field, hmessage, nsblock);
    }

    void FieldGenerator::generate_field_normal_getter(const KmgenOption *file,
                  const MessageOption *message,
                  const FieldOption *field,
                  MessageBlock *hmessage, NamespaceBlock *nsblock) {
        const std::string field_name = field->name();
        const std::string cpp_type = field->cpp_getter_type();
        const std::string getter = gen_getter_name(field_name);

        /// declare
        LinesBuilder header_code;
        header_code.stream() <<cpp_type<<" "<< getter << "() const;\n";
        hmessage->publics_func.push_back(header_code.build());

        /// define
        LinesBuilder source_code;
        source_code.stream() << cpp_type<<" "<<message->km_full_name()<<"::"<< getter << "() const {";
        process_mutex("    ", field, &source_code);
        source_code.stream() << "    return " << gen_variable_name(field_name)<<";";
        source_code.stream() << "}\n";

        nsblock->blocks.push_back(source_code.build());
    }

    void FieldGenerator::generate_field_atomic_getter(const KmgenOption *file,
              const MessageOption *message,
              const FieldOption *field,
              MessageBlock *hmessage, NamespaceBlock *nsblock) {
        const std::string field_name = field->name();
        const std::string cpp_type = field->cpp_getter_type();
        const std::string getter = gen_getter_name(field_name);

        /// declare
        LinesBuilder header_code;
        header_code.stream() <<cpp_type<<" "<< getter << "() const;\n";
        hmessage->publics_func.push_back(header_code.build());

        /// define
        LinesBuilder source_code;
        source_code.stream() <<cpp_type<<" "<<message->km_full_name()<<"::"<< getter << "() const {";
        process_mutex("    ", field, &source_code);
        source_code.stream() << "    return " << gen_variable_name(field_name)<<".load();";
        source_code.stream() << "}\n";

        nsblock->blocks.push_back(source_code.build());
    }

    void FieldGenerator::generate_field_repeat_getter(const KmgenOption *file,
                      const MessageOption *message,
                      const FieldOption *field,
                      MessageBlock *hmessage, NamespaceBlock *nsblock) {
        const std::string field_name = field->name();
        const std::string cpp_type = field->cpp_type();
        const std::string getter = gen_getter_name(field_name);
        const std::string getter_type = turbo::str_format("const %s&", cpp_type);
        /// declare
        LinesBuilder header_code;

        header_code.stream() <<getter_type <<" "<< getter << "() const;\n";
        if (field->is_mutex()) {
            header_code.stream() << "std::mutex& "<< getter << "_mutex();\n";
        }

        hmessage->publics_func.push_back(header_code.build());

        /// define
        LinesBuilder source_code;

        source_code.stream() <<getter_type <<" "<<message->km_full_name()<<"::"<< getter << "() const {";
        source_code.stream()<< "    return "<<gen_variable_name(field_name)<<";";
        source_code.stream()<< "}\n";

        if (field->is_mutex()) {
            source_code.stream() << "std::mutex& "<<message->km_full_name()<<"::"<< getter << "_mutex() {";
            source_code.stream() << "    return "<<gen_variable_name(field_name)<<"_mutex;";
            source_code.stream()<< "}\n";
        }

        nsblock->blocks.push_back(source_code.build());

    }

    void FieldGenerator::generate_field_variable(const KmgenOption *file,
                             const MessageOption *message,
                             const FieldOption *field,
                             MessageBlock *hmessage, NamespaceBlock *nsblock) {
        const std::string field_name = field->name();
        const std::string cpp_type = field->cpp_type();

        LinesBuilder header_code;
        LinesBuilder header_include;

        auto ds = field->get_default_as_string();

        for (auto &it : field->includes()) {
            header_include.stream() << "#include <"<<it<<">";
        }

        bool need_include = false;
        if (field->is_mutex()) {
            header_code.stream()<< "mutable std::mutex "<< gen_variable_name(field_name) << "_mutex;";
            need_include = true;
        }

        if (field->is_atomic()) {
            if (!ds.empty()) {
                header_code.stream()<< "std::atomic<"<<cpp_type<<"> "<< gen_variable_name(field_name)<<"{"<<ds<<"};";
            } else {
                header_code.stream()<< "std::atomic<"<<cpp_type<<"> "<< gen_variable_name(field_name)<<";";
            }
            need_include = true;
        } else {
            if (!ds.empty()) {
                header_code.stream()<< cpp_type<<" "<< gen_variable_name(field_name)<<"{"<<ds<< "};";
            } else {
                header_code.stream()<< cpp_type<<" "<< gen_variable_name(field_name)<<";";
            }
        }
        hmessage->privates_var.push_back(header_code.build());
        if (need_include) {
            auto &gc = CodeWriter::get_instance();
            gc.add_header_include(header_include.build());
        }
    }

    void FieldGenerator::generate_field_pb_serialize(const MessageOption *message, const FieldOption *field, LinesBuilder *builder, const std::string &pb_var) {
        const std::string field_name = field->name();
        const std::string cpp_type = field->cpp_type();
        auto var_name = gen_variable_name(field_name);

        if (!field->is_repeated()) {
            if (field->need_mutable()) {
                builder->stream() <<"*"<<pb_var<<"mutable_"<<field_name<<"() = "<<var_name<<";";
            } else {
                builder->stream() <<pb_var<<".set_"<<field_name<<"("<<var_name<<");";
            }
            return;
        }

        builder->stream()<<"auto *mutable_ptr = "<<pb_var<<".mutable_"<<field_name<<"();";
        builder->stream()<<"mutable_ptr->Reserve("<<var_name<<".size());";
        builder->stream()<<"for(const auto &it : "<<var_name<<") {";
        builder->stream()<<"    *mutable_ptr->Add() = it;";
        builder->stream()<<"}";
    }

    void FieldGenerator::generate_field_pb_deserialize(const MessageOption *message, const FieldOption *field, LinesBuilder *builder, const std::string &pb_var) {
        const std::string field_name = field->name();
        const std::string cpp_type = field->cpp_type();
        auto var_name = gen_variable_name(field_name);

        if (!field->is_repeated()) {
            builder->stream() <<var_name<<" = "<<pb_var<<"."<<field_name<<"();";
            return;
        }

        builder->stream()<<"const auto &ref = "<<pb_var<<"."<<field_name<<"();";
        builder->stream()<<var_name<<".reserve("<<pb_var<<"."<<field_name<<"_size());";
        builder->stream()<<"for(const auto &it : ref) {";
        builder->stream()<<"    "<<var_name<<".push_back(it);";
        builder->stream()<<"}";
    }
}  // namespace shark
